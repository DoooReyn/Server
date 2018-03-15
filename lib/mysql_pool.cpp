#include "mysql_pool.h"
#include "logger.h"

MysqlRow::MysqlRow()
{
}
MysqlRow::~MysqlRow()
{

}

void MysqlRow::SetField(uint32 nField)
{
	m_value.resize(nField);
}
void MysqlRow::SetValue(uint32 nField, const char* value, uint32 nLen)
{
	if (nField < m_value.size())
	{
		m_value[nField].put(value, nLen);
	}
}

VarType& MysqlRow::GetValue(uint32 nField)
{
	if (nField < m_value.size())
	{
		return m_value[nField];
	}
	else
	{
		return VAR_NULL;
	}
}

//==========================================================
void DataSet::SetRowAndField(uint32 nRow, uint32 nField)
{
	m_record.resize(nRow);
	for (uint32 i = 0 ; i < nRow; i++)
	{
		m_record[i].SetField(nField);
	}
}

DataSet::~DataSet()
{
	m_record.clear();
	m_field_map.clear();
}

bool DataSet::PutField(uint32 nField, const char* szName)
{
	return m_field_map.insert(make_pair(szName, nField)).second;
}

void DataSet::PutValue(uint32 nRow, uint32 nField, const char* value, uint32 nLen)
{
	if (nRow >= m_record.size())
	{
		ERROR("PutValue nRow is error row:%d size:%ld", nRow, m_record.size());
		return;
	}
	MysqlRow& row = m_record[nRow];
	row.SetValue(nField, value, nLen);
}

VarType& DataSet::GetValue(uint32 nRow, string strName)
{
	map<string, uint32>::iterator iter = m_field_map.find(strName);
	if (iter == m_field_map.end() || nRow >= m_record.size())
	{
		return VAR_NULL;
	}

	MysqlRow& row =  m_record[nRow];
	return row.GetValue(iter->second);
}

uint32 DataSet::Size()
{
	return m_record.size();
}

//============================================

MysqlHandle::MysqlHandle(const MysqlUrl* url, MysqlPool* pool, uint32 id)
{
	SetId(id);
	m_url = url;
	m_state = HandleState_Invalid;
	m_count = 0;
	m_mysql = NULL;
	m_timet_out = 10000L;
	m_mysql_pool = pool;
}
MysqlHandle::~MysqlHandle()
{
	FinalHandle(1);
	m_url = NULL;
}


bool MysqlHandle::InitMysql(int32 way)
{
	//DEBUG("MysqlHandle InitMysql:%d", way);
	FinalHandle(2);
	if (InitHandle() == false)
	{
		FinalHandle(3);
		return false;
	}
	else
	{
		return true;
	}
}

void MysqlHandle::FinalHandle(int32 way)
{
	if (m_mysql)
	{
		//DEBUG("FinalHandle():The mysql connect will been closed... way:%d", way);
		mysql_close(m_mysql);
		m_mysql = NULL;
	}
	m_state = HandleState_Invalid;
	m_count = 0;
	m_last_sql = "";
}

bool MysqlHandle::SetHandle()
{
	if (m_state == HandleState_Used)
	{
		DEBUG("SetHandle 1");
		return false;
	}
	m_use_time.Now();
	if (m_count > 10000 || mysql_ping(m_mysql) != 0)
	{
		if (InitMysql(1) == false)
		{
			DEBUG("SetHandle 2");
			return false;
		}
	}
	m_state = HandleState_Used;
	m_count++;
	return true;
}

void MysqlHandle::FreeHandle()
{
	m_state = HandleState_Valid;
	m_count--;
}

void MysqlHandle::CheckUseTime()
{
	if (m_use_time.Elapse(Time()) > m_timet_out)
	{
		DEBUG("sql语句超时:%ums,描述:%s", m_timet_out, m_last_sql.c_str());
		m_timet_out += 10000L;
	}
}


bool MysqlHandle::ExecProduct(string& sql, string& result, DataSet& dataSet, bool need_errlog)
{
	int nRet = ExecSql(sql, need_errlog);
	if(nRet < 0)
	{
		return false;
	}

	return ExecSelect(result, dataSet);
}

int MysqlHandle::ExecSql(string& sql, bool need_errlog)
{
	return ExecSql(sql.c_str(), sql.length(), need_errlog);
}

int MysqlHandle::ExecSql(const char* szSql, uint32 nLen, bool need_errlog)
{
	if (szSql == NULL || nLen == 0 || m_mysql == NULL)
	{
		return -1;
	}
	m_last_sql = szSql;
	int nRet = mysql_real_query(m_mysql, szSql, nLen);
	if (nRet && need_errlog)
	{
		ERROR("SQL:%s  Error:%s", szSql, mysql_error(m_mysql));
	}
	return nRet;
}


bool MysqlHandle::ExecSelect(string strSql, DataSet& dataSet)
{
	return ExecSelect(strSql.c_str(), strSql.length(), dataSet);
}

bool MysqlHandle::ExecSelect(const char* szSql, unsigned int nLen, DataSet& dataSet)
{
	if (m_mysql == NULL)
	{
		ERROR("NULL m_mysql Error. ---- %s", szSql);
		return false;
	}
	m_select_time.Now();
	if (ExecSql(szSql, nLen))
	{
		return false;
	}

	MYSQL_RES* result = mysql_store_result(m_mysql);
	if (result == NULL)
	{
		ERROR("Result Get Error:%s", mysql_error(m_mysql));
		return false;
	}
	uint32 nRow =  mysql_num_rows(result);
	if (nRow == 0)
	{
		mysql_free_result(result);
		return true;
	}
	uint32 nField = mysql_num_fields(result);
	if (nField == 0)
	{
		mysql_free_result(result);
		return false;
	}

	dataSet.SetRowAndField(nRow, nField);
	MYSQL_FIELD* mysql_fields = mysql_fetch_fields(result);
	for (uint32 i = 0 ; i < nField; i++)
	{
		if (dataSet.PutField(i, mysql_fields[i].name) == false)
		{
			ERROR("error PutField");
			mysql_free_result(result);
			return false;
		}
	}

	MYSQL_ROW row;
	uint32 i = 0;
	while ((row = mysql_fetch_row(result)))
	{
		unsigned long* lengths = mysql_fetch_lengths(result);
		for (uint32 j = 0 ; j < nField; j++)
		{
			//cout << "ROW[j]" << row[j] << endl;
			dataSet.PutValue(i, j, row[j], lengths[j]);
		}
		i++;
	}
	return true;
}

char* MysqlHandle::escapeString(const char* szSrc, char* szDest, unsigned int size)
{
	if (szSrc == NULL || szDest == NULL || m_mysql == NULL)
	{
		return NULL;
	}
	char* end = szDest;
	mysql_real_escape_string(m_mysql, end, szSrc, size == 0 ? strlen(szSrc) : size);
	return szDest;
}

string& MysqlHandle::escapeString(const std::string& src, string& dest)
{
	if (m_mysql == NULL)
	{
		return dest;
	}
	char buff[2 * src.length() + 1];
	bzero(buff, sizeof(buff));
	mysql_real_escape_string(m_mysql, buff, src.c_str(), src.length());
	dest = buff;
	return dest;
}

int MysqlHandle::GetLastInsertId()
{
	int ret = mysql_insert_id(m_mysql);
	return ret;
}

bool MysqlHandle::InitHandle()
{
	if (m_mysql)
	{
		printf("InitHandle():The mysql connect will been closed...\n");
		mysql_close(m_mysql);
		m_mysql = NULL;
	}

	m_mysql = mysql_init(NULL);
	if (m_mysql == NULL)
	{
		printf("InitHandle():Initiate mysql MERROR...\n");
		return false;
	}

	if (mysql_real_connect(m_mysql, m_url->m_host.c_str(), m_url->m_user.c_str(), m_url->m_passwd.c_str(), m_url->m_dbname.c_str(), m_url->m_port, NULL, CLIENT_COMPRESS | CLIENT_INTERACTIVE) == NULL)
	{
		printf("InitHandle():connect mysql://%s:%u/%s failed...\n", m_url->m_host.c_str(), m_url->m_port, m_url->m_dbname.c_str());
		return false;
	}

	char value = 1;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &value);

	if (mysql_set_character_set(m_mysql, "utf8"))
	{
		printf("mysql_set_character_set 错误, %s\n", mysql_error(m_mysql));
	}

	//printf("initMysql():connect mysql://%s:%u/%s successful...\n", m_url->m_host.c_str(), m_url->m_port, m_url->m_dbname.c_str());
	m_state = HandleState_Valid;
	m_life_time.Now();
	m_count = 0;
	return true;
}

//============================================
MysqlPool::MysqlPool(uint32 mMaxHandle)
{
	m_max_handle = mMaxHandle;
	//printf("Version of the mysql libs is %s\n", mysql_get_client_info());
	if (!mysql_thread_safe())
	{
		printf("The mysql libs is not thread safe...\n");
	}
}

MysqlPool::~MysqlPool()
{
	m_mum.RemoveAllEntry();
	m_murl.RemoveAllEntry();
}


bool MysqlPool::PutUrl(const char* szUrl, const uint32 id)
{
	MysqlUrl* mysql_url = new MysqlUrl(szUrl);
	if (mysql_url == NULL)
	{
		return false;
	}
	mysql_url->SetId(id);
	m_murl.AddEntry(mysql_url);

	MysqlHandle* handle = new MysqlHandle(mysql_url, this, id);
	if (handle == NULL)
	{
		m_murl.RemoveEntry(mysql_url);
		return false;
	}

	if (handle->InitMysql(2) == false)
	{
		m_murl.RemoveEntry(mysql_url);
		return false;
	}

	if (m_mum.AddEntry(handle) == false)
	{
		m_murl.RemoveEntry(mysql_url);
		handle->FinalHandle(4);
		delete handle;
		handle = NULL;
		return false;
	}
	return true;
}


MysqlHandle* MysqlPool::GetHandle(uint32 id)
{

	struct GetHandleExec : public Callback<MysqlHandle>
	{
		MysqlHandle* _handle;
		GetHandleExec(): _handle(NULL) {}
		bool exec(MysqlHandle* entry)
		{
			switch (entry->m_state)
			{
				case HandleState_Valid:
				case HandleState_Invalid:
				{
					if (entry->SetHandle())
					{
						_handle = entry;
						return false;
					}
				}
				break;
				case HandleState_Used:
				{
					entry->CheckUseTime();
				}
				break;
				default:
					break;
			}
			return true;
		}
	};
	GetHandleExec Exec;
	while (true)
	{
		m_mum.execEveryEntry<>(Exec);
		if (Exec._handle)
		{
			return Exec._handle;
		}
		if (m_mum.Size() < m_max_handle)
		{
			MysqlUrl* url = m_murl.GetEntryById(id);
			if (url == NULL)
			{
				return NULL;
			}

			MysqlHandle* handle = new MysqlHandle(url, this, id);
			if (handle == NULL)
			{
				return NULL;
			}

			if (handle->InitMysql(3) == false)
			{
				delete handle;
				handle = NULL;
				return NULL;
			}

			if (m_mum.AddEntry(handle) == false)
			{
				handle->FinalHandle(5);
				delete handle;
				handle = NULL;
				return NULL;
			}
			return handle;
		}
		usleep(1000 * 50);

	}
	return NULL;
}

void MysqlPool::PutHandle(MysqlHandle* handle)
{
	if (handle)
	{
		handle->FreeHandle();
	}
}
