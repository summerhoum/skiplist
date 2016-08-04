/*************************************************************************
	> File Name: skiplist.cpp
	> Author: Summer
	> Mail: 745189913@qq.com
	> Created Time: Mon 25 Jul 2016 03:19:38 PM CST
 ************************************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
using namespace std;


template<class KEY_TYPE, class VAL_TYPE>
class CSkipList
{
private:
    struct Node;
public:
    CSkipList(unsigned int uiMaxLevel);
    ~CSkipList();

    bool Insert(KEY_TYPE key, VAL_TYPE value);
    bool Erase(KEY_TYPE key);
    VAL_TYPE* Search(KEY_TYPE key);
public:
    class Iterator
    {
    public:
        explicit Iterator(const CSkipList* list);

        const KEY_TYPE& Key() const;
        VAL_TYPE* Value() const;
        bool Next();
        bool IsEnd() const;
    private:
        const CSkipList *m_poList;
        struct Node* m_pstNode;
    };
private:
   struct Node *NewNode(int iLevel);
   unsigned int RandomLevel();
private:
    unsigned int m_uiMaxLevel;
    unsigned int m_uiLevel;
    Node *m_pstHead;
};

template<class KEY_TYPE, class VAL_TYPE>
struct CSkipList<KEY_TYPE, VAL_TYPE>::Node
{
    KEY_TYPE key;
    VAL_TYPE value;
    Node* next[1]; // Flexible Array
};

template<class KEY_TYPE, class VAL_TYPE>
CSkipList<KEY_TYPE, VAL_TYPE>::CSkipList(unsigned int uiMaxLevel)
{
    m_uiMaxLevel = uiMaxLevel > 0 ? uiMaxLevel : 2;
    m_uiLevel = 0;
    m_pstHead = NewNode(m_uiMaxLevel - 1);
    assert(m_pstHead); 
    for (unsigned i = 0; i < uiMaxLevel; ++i)
    {
        m_pstHead->next[i] = NULL;
    }
}


template<class KEY_TYPE, class VAL_TYPE>
CSkipList<KEY_TYPE, VAL_TYPE>::~CSkipList()
{
    Node *p = m_pstHead;
    Node *next = NULL;
    while(p)
    {
        next = p->next[0];
        free(p);
        p = next;
    }
}

template<class KEY_TYPE, class VAL_TYPE>
typename CSkipList<KEY_TYPE, VAL_TYPE>::Node* CSkipList<KEY_TYPE, VAL_TYPE>::NewNode(int iLevel)
{
    Node *p = (Node*)malloc(sizeof(Node) + iLevel*sizeof(Node*));
    return p;
}
 

template<class KEY_TYPE, class VAL_TYPE>
unsigned int CSkipList<KEY_TYPE, VAL_TYPE>::RandomLevel()
{
    unsigned int uiLevel = 1;
    while(rand()%2)
    {
        ++uiLevel;
    }
    uiLevel = uiLevel > m_uiMaxLevel ? m_uiMaxLevel : uiLevel;
    return uiLevel;
}


template<class KEY_TYPE, class VAL_TYPE>
bool CSkipList<KEY_TYPE, VAL_TYPE>::Insert(KEY_TYPE key, VAL_TYPE value)
{
    Node *p = NULL, *q = NULL;
    Node *update[m_uiMaxLevel];
    p = m_pstHead;
    int i = m_uiLevel;
    
    for (; i >= 0; --i)
    {
        while((q=p->next[i]) && q->key < key)
        {
            p = q;
        }
        update[i] = p;
    }

    if (q && q->key == key)
    {
        q->value = value;
        return true;
    }

    unsigned int uiLevel = RandomLevel();
    if (uiLevel > m_uiLevel)
    {
        for (i = m_uiLevel; i < uiLevel; ++i)
        {
            update[i] = m_pstHead;
        }
        m_uiLevel = uiLevel;
    }

    q = NewNode(uiLevel);
    if (NULL == q)
    {
        return false;
    }

    q->key = key;
    q->value = value;
    for (i = uiLevel - 1; i >= 0; --i)
    {
        q->next[i] = update[i]->next[i];
        update[i]->next[i] = q;
    }

    return true;
}

  
template<class KEY_TYPE, class VAL_TYPE>
bool CSkipList<KEY_TYPE, VAL_TYPE>::Erase(KEY_TYPE key)
{
    Node *update[m_uiMaxLevel];
    Node *q = NULL, *p = m_pstHead;
    int i = m_uiLevel;
    for (; i >= 0; --i)
    {
        while((q=p->next[i]) && q->key < key)
        {
            p = q;
        }
        update[i] = p;
    }

    if (!q || q->key != key)
    {
        return false;
    }

    for (i = m_uiLevel; i >= 0; --i)
    {
        if (update[i]->next[i] == q)
        {
            update[i]->next[i] = q->next[i];
            if (m_pstHead->next[i] == NULL)
            {
                --m_uiLevel;
            }
        }
    }

    free(q);
    q = NULL;

    return true;
}


template<class KEY_TYPE, class VAL_TYPE>
VAL_TYPE* CSkipList<KEY_TYPE, VAL_TYPE>::Search(KEY_TYPE key)
{
    Node *q = NULL;
    Node *p = m_pstHead;
    int i = m_uiLevel;
    for (; i >= 0; --i)
    {
        while((q=p->next[i]) && q->key < key)
        {
            p =q;
        }

        if (q && key == q->key)
        {
            return &(q->value);
        }
    }
    
    return NULL;
}

template<class KEY_TYPE, class VAL_TYPE>
inline CSkipList<KEY_TYPE,VAL_TYPE>::Iterator::Iterator(const CSkipList* list)
{
    m_poList = list;
    assert(list);
    m_pstNode = list->m_pstHead;
}

template<class KEY_TYPE, class VAL_TYPE>
inline const KEY_TYPE& CSkipList<KEY_TYPE, VAL_TYPE>::Iterator::Key() const
{
    assert(m_pstNode);
    return m_pstNode->key;
}

template<class KEY_TYPE, class VAL_TYPE>
inline VAL_TYPE* CSkipList<KEY_TYPE, VAL_TYPE>::Iterator::Value() const
{
    assert(m_pstNode);
    return &(m_pstNode->value);
}

template<class KEY_TYPE, class VAL_TYPE>
inline bool CSkipList<KEY_TYPE, VAL_TYPE>::Iterator::IsEnd() const
{
    return NULL == m_pstNode;
}


template<class KEY_TYPE, class VAL_TYPE>
inline bool CSkipList<KEY_TYPE, VAL_TYPE>::Iterator::Next()
{
    if (NULL == m_pstNode || NULL == m_pstNode->next[0])
    {
        return false;
    }

    m_pstNode = m_pstNode->next[0];

    return true;
}

int main(int argc, char *argv[])
{
    int iMaxLevel = 5;
    int iEntry = 10;

    CSkipList<int, int> sl(5);

    srand(time(0));
    cout << "test insert: " << sl.Insert(10 , 11) << endl;
    cout << "test search: " << *(sl.Search(10)) << endl;
    cout << "test erase: " << sl.Erase(10) << endl;
    cout << "test search after erase: " << (sl.Search(10) ? "Found" : "Not Found")  << endl;

    for (int i = 0; i < iEntry; ++i)
    {
        int e = rand() % iEntry;
        cout << e << endl;
        sl.Insert(e, e + 1);
    }

    CSkipList<int, int>::Iterator it(&sl);
    while (it.Next())
    {
       cout << "key=" << it.Key() << ", value=" << *(it.Value()) << endl;
    }
    
    return 0;
}

