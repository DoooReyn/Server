#include "var_type.h"
#include "mysql_pool.h"

void TestVarType()
{
	/*
	uint32 nRow = 7;
	uint32 nField = 5;
	char szName[10] = {0};

	DataSet* ret_set = new DataSet(nRow, nField);
	if(ret_set == nullptr)
	{
		printf("error dataset new\n");
	}

	for(uint32 i = 0 ; i < nField; i++)
	{
		sprintf(szName, "Field%d", i);
		if(ret_set->PutField(i, szName) == false)
		{
			printf("error PutField\n");
			return;
		}
	}


	// IterFeild iter1 = ret_set->m_field_set.begin();
	// IterFeild iter2 = ret_set->m_field_set.end();
	// while(iter1  != iter2)
	// {
	// 	cout << (*iter1).nIndex << ":" << (*iter1).szName << endl;
	// 	iter1++;
	//}
	//cout << ret_set->m_field_set.size() << " " << ret_set->m_record.Size() << endl;

	for(uint32 i = 0 ; i < nRow; i++)
	{
		for(uint32 j = 0 ; j < nField; j++)
		{
			sprintf(szName, "value_%d_%d", i, j);
			ret_set->PutValue(i, j, szName, strlen(szName) + 1);
		}
	}

	for(uint32 i = 0 ; i < nRow; i++)
	{
		for(uint32 j = 0 ; j < nField; j++)
		{
			sprintf(szName, "Field%d", j);
			const char* value = ret_set->GetValue(i, szName);
			printf("%d--%s:%s\n",i, szName,value);
		}

	}
	delete ret_set;
	ret_set = nullptr;
	*/
}

int main(int argc, char const* argv[])
{
	//TestVarType();
	return 0;
}
