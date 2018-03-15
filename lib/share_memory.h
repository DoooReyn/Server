#ifndef __SHARE_MEMORY_H__
#define __SHARE_MEMORY_H__

#include "type_define.h"
#include "singleton.h"
#include "entry.h"
#include "logger.h"

enum ShareObjectStatus
{
	SHARED_NONE,
	SHARED_MODIFY,
	SHARED_DELETE,
};

struct ShareObject
{
	ShareObject()
	{
		m_status = SHARED_NONE, m_nFlag = 0xFE;
	}
	bool IsModify()
	{
		return m_status == SHARED_MODIFY;
	}
	bool IsDelete()
	{
		return m_status == SHARED_DELETE;
	}
	void Modify()
	{
		m_status = SHARED_MODIFY;
	}
	void Destroy()
	{
		m_status = SHARED_DELETE;
	}
	void Reset()
	{
		m_status = SHARED_NONE;
	}

	uint32 m_nFlag;  //检测标志
	ShareObjectStatus m_status;
};


struct _SMBlock
{
	uint32 m_nID = 0;       //数据当前编号
	bool m_bUsed = 0;       //是否在使用true是正在使用，false是没有使用
	bool m_bNew = 0;		//是否是刚刚新创建的区块
};

struct MemoryPage
{
	char*        m_pdata = NULL;	//指定共享内存地址
	_SMBlock*    m_pBlock = NULL;	//数据块的头位置
};


class SharedMemoryBase
{
public:
	SharedMemoryBase(const string& filename, uint32 blocksize, uint32 count, bool bCreate);
	virtual ~SharedMemoryBase();

	bool importPage();
	bool createPage(bool bCreate);		//创建共享内存页
	bool initPage(MemoryPage& memPage);	//初始化新页


	int32 GetRawObjectCount();			//读取共享
	virtual ShareObject* NewObject(bool isNew);
	virtual bool DestoryObject(ShareObject* pObject);

	uint32 GetBlockSize()
	{
		return m_block_size;
	}
	uint32 GetCount()
	{
		return m_pageNum * m_count;
	};
	uint32 GetPage()
	{
		return m_pageNum;
	};
	uint32 GetFreeCount()
	{
		return m_mapFreeSMBlock.size();
	}
	uint32 GetUsedCount()
	{
		return m_mapUsedSMBlock.size();
	}
	virtual _SMBlock* GetSMBbyRawIndex(uint32 index);
	virtual ShareObject*  GetObjectByRawindex(uint32 index);

	//bool  allocSharedMemory(const uint32 id, const void* p, const uint32 size);
	//void  freeSharedMemory();
	//void* setSharedMemory(const uint32 id, bool& exist);
	//const void* getSharedMemory(const uint32 id);
	//void  clearSharedMemory(void* p);

	void final();
private:
	int32 m_fd;
	string m_filename;

	const uint32 m_block_size;
	const uint32 m_count;
	int32 m_pageNum;



	std::vector<MemoryPage> m_vecSharedPage;

	///所有使用了块的数据信息
	typedef std::map<void*, _SMBlock*>  mapUsedSMBlock;
	mapUsedSMBlock m_mapUsedSMBlock;

	///所有空闲的块信息
	typedef std::map<uint32, _SMBlock*> mapFreeSMBlock;
	mapFreeSMBlock m_mapFreeSMBlock;
};


template<typename T>
class SharedMemory : public SharedMemoryBase
{
public:
	SharedMemory(const string& filename, uint32 count, bool bCraete)
		: SharedMemoryBase(filename, sizeof(T), count, bCraete)
	{

	}

	_SMBlock* GetSMBbyRawIndex(int32 index)
	{
		return SharedMemoryBase::GetSMBbyRawIndex(index);
	}

	T*  GetObjectByRawindex(uint32 index)
	{
		return static_cast<T*>(SharedMemoryBase::GetObjectByRawindex(index));
	}

	T* NewObject(bool isNew = false)
	{
		T* pTmp = static_cast<T*>(SharedMemoryBase::NewObject(isNew));
		new(pTmp)(T);
		if (pTmp == NULL)
		{
			ERROR("NewObject is return NULL");
		}

		return pTmp;
	}

	bool DestoryObject(T* pObject)
	{

		return SharedMemoryBase::DestoryObject(pObject);
	}
};



#endif