#ifndef __SHARED_ARRAY_H_
#define __SHARED_ARRAY_H_

namespace memory
{

template<typename T>
struct ArrayDeleter
{
    void operator()(T *ptr)
    {
        delete [] ptr;
    }
};

template<typename T, typename D=void> struct CtrlBlk;
template<typename T> class shared_ptr;

template<typename T>
struct CtrlBlk<T,void>
{
    CtrlBlk(T * _ptr): m_oObjPtr(_ptr), m_dCount(1) {}
    void increment(void) { ++m_dCount; }
    void release(void) 
    { 
        if(m_dCount <= 0) return; 
        --m_dCount; 
        if(m_dCount==0) _delete(); 
    }

    virtual void _delete(void)
    {
        delete m_oObjPtr;
        m_oObjPtr=NULL;
    }

    virtual void * get_deleter(void) { return NULL; }

    T * get(void) const { return m_oObjPtr;}

    size_t use_count(void) const { return m_dCount;}

    virtual ~CtrlBlk(void) { release(); }

protected:
    size_t m_dCount;
    T * m_oObjPtr;

private:
    CtrlBlk(const CtrlBlk &);
    CtrlBlk & operator = ( const CtrlBlk &);

    friend class shared_ptr<T>;
};

template<typename T, typename D>
struct CtrlBlk: CtrlBlk<T>
{
    CtrlBlk(T * _ptr, D d): CtrlBlk<T>(_ptr), m_oDeleter(d) {}

    virtual void _delete(void)
    {
        this->m_oDeleter(this->m_oObjPtr);
    }

    virtual void * get_deleter(void) { return &m_oDeleter; }

private:
    D m_oDeleter;

    CtrlBlk(const CtrlBlk &);
    CtrlBlk & operator = ( const CtrlBlk &);
};

template<typename T>
class shared_ptr
{
public:
    typedef T element_type;

    shared_ptr(void): ctrlPtr(NULL) {}

    template<typename Y>
    explicit shared_ptr(Y * ptr): ctrlPtr(new CtrlBlk<T>(ptr)) {}

    template<typename Y, typename D>
    shared_ptr(Y * ptr, const D & _deleter): ctrlPtr(new CtrlBlk<T,D>(ptr,_deleter)) {}
    
    ~shared_ptr(void) {release();}

    shared_ptr(const shared_ptr & other): ctrlPtr(other.ctrlPtr) {ctrlPtr->increment();}

    template<typename Y>
    shared_ptr(const shared_ptr<Y> & other): ctrlPtr(other.ctrlPtr) {ctrlPtr->increment();}

    template<typename Y>
    shared_ptr(const shared_ptr<Y> & other, T * ptr): ctrlPtr(other.ctrlPtr) {ctrlPtr->m_oObjPtr = ptr;}
    
    shared_ptr & operator = (const shared_ptr & other)
    {
        if(ctrlPtr == other.ctrlPtr) return *this;
        copy(other);
        return *this;
    }

    template<typename Y>
    shared_ptr & operator = (const shared_ptr<Y> & other)
    {
        if(ctrlPtr == other.ctrlPtr) return *this;
        copy(other);
        return *this;
    }

    void swap(shared_ptr & other)
    {
        if(ctrlPtr == other.ctrlPtr) return;
        std::swap(ctrlPtr, other.ctrlPtr);
    }

    void reset(void)
    {
        release();
        ctrlPtr = NULL;
    }

    template<typename Y>
    void reset(Y * ptr)
    {
        shared_ptr<T>(ptr).swap(*this);
    }

    template<typename Y, typename D>
    void reset(Y * ptr, D d)
    {
        shared_ptr<T>(ptr,d).swap(*this);
    }

    template<typename Y>
    void reset(const shared_ptr<Y> & other)
    {
        shared_ptr(other).swap(*this);
    }

    template<typename Y>
    void reset(const shared_ptr<Y> & other, T * ptr)
    {
        shared_ptr(other, ptr).swap(*this);
    }

    T & operator *() const { return *(ctrlPtr->get());}
    T * operator ->() const { return ctrlPtr->get();}
    T * get() const { return ctrlPtr==NULL ? NULL : ctrlPtr->get();}

    size_t use_count() const { return ctrlPtr->use_count();}

    bool unique() const { return use_count() == 1;}

    operator bool () const { return get() != NULL;}

private:
    void release(void)
    {
        if(ctrlPtr==NULL) return;
        ctrlPtr->release();
        if(use_count()==0) 
        {
            delete ctrlPtr;
            ctrlPtr=NULL;
        }
    }
    
    template<typename Y>
    void copy(const shared_ptr<Y> & other)
    {
        release();
        ctrlPtr = other.ctrlPtr;
        if(ctrlPtr!=NULL) ctrlPtr->increment();
    }

    CtrlBlk<T> * ctrlPtr;
    
    static const bool debug = true;
    
};

template<typename T>
class shared_array: public shared_ptr<T>
{
public:
    typedef T element_type;

    explicit shared_array(T * ptr=NULL): shared_ptr<T>(ptr, ArrayDeleter<T>()) {}

    template<typename D>
    shared_array(T * ptr, const D & _deleter): shared_ptr<T>(ptr, _deleter) {}
    
    shared_array(const shared_array & other): shared_ptr<T>(other) {}
    
    shared_array & operator = (const shared_array & other) { shared_ptr<T>::operator=(other); return *this;}

    T & operator[] (int idx) { return *(this->get()+idx);}
    
    const T & operator[] (int idx) const { return *(this->get()+idx);}
    
};


template<typename T, typename U>
bool operator == (const shared_ptr<T> & a, const shared_ptr<U> & b)
{
    return a.get() == b.get();
}

template<typename T, typename U, typename D>
bool operator != (const shared_ptr<T> & a, const shared_ptr<U> & b)
{
    return a.get() != b.get();
}

template<typename T, typename U, typename D>
bool operator < (const shared_ptr<T> & a, const shared_ptr<U> & b)
{
    return a.get() < b.get();
}

template<typename T>
void swap(shared_ptr<T> & a, shared_ptr<T> & b)
{
    a.swap(b);
}

template<typename T>
T * get_pointer(const shared_ptr<T> & other)
{
    return other.get();
}

template<typename T, typename U>
shared_ptr<T> static_pointer_cast(const shared_ptr<U>  & other)
{
    return shared_ptr<T>(static_cast<T*>(other.get()));
}

template<typename T, typename U>
shared_ptr<T> const_pointer_cast(const shared_ptr<U>  & other)
{
    return shared_ptr<T>(const_cast<T*>(other.get()));
}

template<typename T, typename U>
shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U>  & other)
{
    return shared_ptr<T>(dynamic_cast<T*>(other.get()));
}

template<typename E, typename T, typename Y>
std::basic_ostream<E,T> & operator << (std::basic_ostream<E,T> & os, const shared_ptr<Y> & p)
{
    os << p.get();
}

template<class D, class T>
D * get_deleter(const shared_ptr<T> & p)
{
    void * ptr = p->get_deleter();
    return static_cast<D *>(ptr);
}

} /* end of namespace */

#endif /* __SHARED_ARRAY_H_ */

