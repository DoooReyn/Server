#include "share_memory.h"


SharedMemoryBase::SharedMemoryBase(const string& filename, uint32 blocksize, uint32 count, bool bCreate)
	: m_filename(filename)
	, m_block_size(blocksize)
	, m_count(count)
	, m_pageNum(0)

{
	if (bCreate == false)
	{
		importPage();
	}
	else
	{
		createPage(bCreate);
	}
}

SharedMemoryBase::~SharedMemoryBase()
{
	final();
}

void SharedMemoryBase::final()
{
	m_mapFreeSMBlock.clear();
	m_mapUsedSMBlock.clear();
	m_vecSharedPage.clear();
	for (int32 i = 0; i < m_pageNum; i++)
	{
		char szfilename[1024] = { 0 };
		sprintf(szfilename, "%s_%u", m_filename.c_str(), i);
		shm_unlink(szfilename);
	}

}

bool SharedMemoryBase::importPage()
{
	bool ret = false;
	do
	{
		ret = createPage(false);
	}
	while (ret);

	return true;
}

bool SharedMemoryBase::createPage(bool bCreate)
{
	//创建或打开一个共享内存,成功返回一个整数的文件描述符，错误返回-1
	int32 flag = O_RDWR;
	if (bCreate)
	{
		flag |= O_CREAT;
	}
	char szfilename[1024] = { 0 };
	sprintf(szfilename, "%s_%u", m_filename.c_str(), m_pageNum);

	m_fd = ::shm_open(szfilename, flag, S_IRUSR | S_IWUSR);
	if (m_fd == -1)
	{
		if (bCreate)
		{
			printf("打开文件失败 file:%s errno:%s\n", szfilename, strerror(errno));
		}
		return false;
	}

	uint32 totol_size = (m_block_size + sizeof(_SMBlock)) * m_count;
	struct stat sb;
	//获取文件状态信息
	if (::fstat(m_fd, &sb) == -1)
	{
		printf("获取文件信息失败 errno:%s\n", strerror(errno));
		shm_unlink(szfilename);
		return false;
	}

	if (uint32(sb.st_size) != totol_size)
	{
		if (bCreate == false)
		{
			printf("共享内存页读取失败 file:%s cur_size:%lu set_size:%u errno:%s\n", szfilename, sb.st_size, totol_size, strerror(errno));
			shm_unlink(szfilename);
			return false;
		}

		//设置文件长度
		int32 nLen = ftruncate(m_fd, totol_size);
		if (nLen == -1)
		{
			printf("设置文件长度失败 file:%s cur_size:%lu set_size:%u errno:%s\n", szfilename, sb.st_size, totol_size, strerror(errno));
			shm_unlink(szfilename);
			return false;
		}
	}

	//将一个文件或者其它对象映射进内存
	char* pSharedMem = static_cast<char*>(::mmap(0, totol_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0));
	if (pSharedMem == MAP_FAILED)
	{
		printf("初始化共享内存失败:%s,%s\n", szfilename, strerror(errno));
		shm_unlink(m_filename.c_str());
		return false;
	}

	if (bCreate)
	{
		memset(pSharedMem, 0, totol_size);
	}

	MemoryPage memPage;
	memPage.m_pBlock = (_SMBlock*)pSharedMem;
	memPage.m_pdata = pSharedMem + sizeof(_SMBlock) * m_count;
	initPage(memPage);
	m_vecSharedPage.push_back(memPage);
	m_pageNum++;

	printf("shared:%s success\n", szfilename);
	return true;
}


bool SharedMemoryBase::initPage(MemoryPage& memPage)
{
	for (uint32 i = 0; i < m_count; i++)
	{
		_SMBlock* ptem = &(memPage.m_pBlock[i]);
		uint32 id = i + (m_pageNum * m_count);
		new(ptem)(_SMBlock);
		ptem->m_nID = id;
		m_mapFreeSMBlock.insert(std::make_pair(id, ptem));
	}
	return true;
}


int32 SharedMemoryBase::GetRawObjectCount()
{
	importPage();
	return GetCount();
}

ShareObject* SharedMemoryBase::NewObject(bool isNew)
{
	if (m_mapFreeSMBlock.size() == 0)
	{
		bool bRet = createPage(true);
		if (bRet)
		{
			return NewObject(isNew);
		}
		else
		{
			printf("NewObject Error 1 \n");
			return NULL;
		}
	}

	mapFreeSMBlock::iterator it = m_mapFreeSMBlock.begin();
	while (it != m_mapFreeSMBlock.end())
	{
		_SMBlock* pBlock = it->second;
		ShareObject* pObject = GetObjectByRawindex(pBlock->m_nID);
		if (pObject == NULL)
		{
			it++;
			printf("NewObject Error 2 \n");
			continue;
		}


		m_mapUsedSMBlock.insert(std::make_pair(pObject, pBlock));
		m_mapFreeSMBlock.erase(it);

		pBlock->m_bUsed = true;
		pBlock->m_bNew = isNew;

		return pObject;


		it++;
	}
	printf("NewObject Error 3\n");
	return NULL;
}

bool SharedMemoryBase::DestoryObject(ShareObject* pObject)
{
	if (pObject == NULL)
	{
		return false;
	}
	pObject->Reset();

	mapUsedSMBlock::iterator itFind = m_mapUsedSMBlock.find(pObject);
	if (itFind == m_mapUsedSMBlock.end())
	{
		return false;
	}
	_SMBlock* pBlock = itFind->second;
	pBlock->m_bNew = false;
	m_mapUsedSMBlock.erase(itFind);
	m_mapFreeSMBlock.insert(std::make_pair(pBlock->m_nID, pBlock));
	return true;
}


_SMBlock* SharedMemoryBase::GetSMBbyRawIndex(uint32 index)
{
	if (index < GetCount())
	{
		uint32 pageNum = index / m_count;
		uint32 pageindex = index % m_count;
		if (pageNum > m_vecSharedPage.size() - 1)
		{
			printf("GetSMBbyRawIndex filename:%s index:%d count:%d\n", m_filename.c_str(), index, m_count);
			return NULL;
		}

		MemoryPage& mempage = m_vecSharedPage[pageNum];
		return &(mempage.m_pBlock[pageindex]);
	}
	else
	{
		printf("GetSMBbyRawIndex filename:%s index:%d count:%d page:%d \n", m_filename.c_str(), index, m_count, m_pageNum);
	}
	return NULL;
}


ShareObject* SharedMemoryBase::GetObjectByRawindex(uint32 index)
{
	if (index < GetCount())
	{
		uint32 pageNum = index / m_count;
		uint32 pageindex = index % m_count;
		if (pageNum > m_vecSharedPage.size() - 1)
		{
			printf("GetObjectByRawindex index:%d count:%d\n", index, m_count);
			return NULL;
		}

		MemoryPage& mempage = m_vecSharedPage[pageNum];
		return reinterpret_cast<ShareObject*>(mempage.m_pdata + m_block_size * pageindex);
	}
	else
	{
		printf("GetObjectByRawindex filename:%s index:%d count:%d page:%d \n", m_filename.c_str(), index, m_count, m_pageNum);
	}
	return NULL;
}