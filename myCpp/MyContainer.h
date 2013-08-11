#ifndef __MY_CONTAINER__
#define __MY_CONTAINER__

#include <vector>
#include <iterator>

namespace MySpace
{

template<typename T>
class MyContainer
{
public:
    typedef T                   value_type;
    typedef value_type &        reference;
    typedef const value_type &  const_reference;
    typedef value_type *        pointer;
    typedef const value_type *  const_pointer;
    typedef std::vector<value_type>      T_Container;

    class const_iterator;

    class iterator:public std::iterator<std::bidirectional_iterator_tag, T>
    {
    public:
        iterator(std::vector<T> * ptr, size_t idx=0):  m_vptr(ptr), m_idx(idx) {}
        iterator(const iterator & other): m_vptr(other.m_vptr), m_idx(other.m_idx) {}
        ~iterator(void) {}
        iterator & operator ++() { ++m_idx; return *this;}
        iterator & operator --() { --m_idx; return *this;}

        bool operator ==(const iterator & other) const
        { 
            size_t  myidx=this->m_idx, 
                    otheridx=other.m_idx;

            return this->m_vptr == other.m_vptr && myidx == otheridx;
        }

        bool operator !=(const iterator & other) const
        { return ! (*this == other);
        }

        T & operator *() { return (*m_vptr)[m_idx]; }
        T & operator +=(size_t offset) { m_idx += offset; return (*m_vptr)[m_idx];}

        size_t offset() const { return m_idx;}

    private:
        std::vector<T> * m_vptr;
        size_t m_idx;
    
    friend class const_iterator;
    friend class MyContainer;

    };

    class const_iterator:public std::iterator<std::bidirectional_iterator_tag, T>
    {
    public:
        const_iterator(const std::vector<T> * ptr, size_t idx=UINT_MAX):  m_vptr(ptr), m_idx(idx) {}
        const_iterator(const const_iterator & iter): m_vptr(iter.m_vptr), m_idx(iter.m_idx) {}
        const_iterator(const iterator & iter): m_vptr(iter.m_vptr), m_idx(iter.m_idx) {}
        ~const_iterator(void) {}
        const_iterator & operator ++() { ++m_idx; return *this;}
        const_iterator & operator --() { --m_idx; return *this;}

        bool operator ==(const const_iterator & other) const
        { 
            size_t  myidx=this->m_idx, 
                    otheridx=other.m_idx;
            
            return this->m_vptr == other.m_vptr && myidx == otheridx;
        }

        bool operator !=(const const_iterator & other) const
        { return ! (*this == other);
        }

        const T & operator *() const { return (*m_vptr)[this->m_idx]; }
        T & operator +=(size_t offset) { this->m_idx += offset; return (*m_vptr)[this->midx];}
        size_t offset() const { return this->m_idx;}
    private:
        std::vector<T> * m_vptr;
        size_t m_idx;
        friend class MyContainer;
    };
    
    
    //typedef typename std::vector<T>::iterator iterator;
    //typedef typename std::vector<T>::const_iterator const_iterator;
    MyContainer(void) {}
    MyContainer(std::iterator<std::input_iterator_tag, T> beg_itr, std::iterator<std::input_iterator_tag, T> end_itr) 
    {
        while(beg_itr != end_itr) { add(*beg_itr); ++beg_itr;}
    }

    ~MyContainer(void) {}

    // Container manipulation
    // **  adding elements 
    void
    push_back(const T& item) 
    { 
        m_oVec.push_back(item); 
    }
    
    void
    assign(const T* beg_itr, const T* end_itr) 
    {  
        while(beg_itr != end_itr) { assign(1, *beg_itr); ++beg_itr;}
    }

    void
    assign(size_t n, const T& u)
    {
        for(; n>0; --n){
            m_oVec.push_back(u);
        }
    }
    
    // **  removing elements
    iterator 
    erase(iterator it)
    {
        typename T_Container::iterator vit=m_oVec.begin();
        advance(vit, it.m_idx);

        vit=m_oVec.erase(vit);
        
        return it;
    }
    
    void
    pop_back() 
    { 
        m_oVec.pop_back(this->item); 
    }

    size_t
    size() const {return m_oVec.size();}

    // Iterator related
    iterator 
    begin() { return iterator(&m_oVec, 0);}
    
    const_iterator 
    begin() const { return const_iterator(&m_oVec, 0);}

    iterator 
    end() { return iterator(&m_oVec, m_oVec.size());}
    
    const_iterator 
    end() const { return const_iterator(&m_oVec, m_oVec.size());}


private:
    T_Container m_oVec;
};


} /* namespace MySpace */

#endif /*  __MY_CONTAINER__ */
