#ifndef _ALLOCATOR_H_INCLUDED_
#define _ALLOCATOR_H_INCLUDED_

using namespace std;

template <class T>
class Allocator
{
public:
	Allocator() 
	{
		m_size = 0;
		m_pCache = NULL;
		m_currUsed = 0;
	}
	~Allocator() 
	{
		Release();
	}
	void Release() 
	{
		if (m_pCache != NULL)
		{
			free(m_pCache);
			m_size = 0;
			m_pCache = NULL;
			m_currUsed = 0;
		}
	}
	void Deallocate() { m_currUsed = 0; }
	void Init(int size)	
	{
		if (size > m_size)
		{
			T* pNewCache = (T*)malloc(size*sizeof(T));

			if (m_currUsed > 0)
			{
				memcpy(pNewCache, m_pCache, m_currUsed*sizeof(T));
			}
			Release();
			m_pCache = pNewCache;
			m_size = size;
		}
	}
	T* New()
	{
//		assert(m_currUsed < m_size);
		if (m_currUsed >= m_size)
		{
			cout << "Warning: surpise allocation!!\n";
			assert(false);
			return NULL;
		}
		return &m_pCache[m_currUsed++];
	}
	T* GetCache()
	{
		return m_pCache;
	}
	T& Get(int index)
	{
		return m_pCache[index];
	}
	T* GetPtr(int index)
	{
		return &m_pCache[index];
	}
	void Remove()
	{
		m_currUsed--;
	}
private:
	T* m_pCache;
	int m_size;
	int m_currUsed;
};





#endif // _ALLOCATOR_H_INCLUDED_
