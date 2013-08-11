#ifndef __MY_DESIGN_PATTERN__
#define __MY_DESIGN_PATTERN__

#include <memory>
#include <boost/format.hpp>

#ifdef __GNUC__
    #include <tr1/memory>
    #define TSHARED_PTR std::tr1::shared_ptr
#else
    #include <memory>
    #define TSHARED_PTR std::shared_ptr
#endif


//*************************************************
// Visitor design pattern
//*************************************************
enum ENUM_TYPE 
{
    ENUM_A,
    ENUM_B
};

class User;
class BB;
class D1;
class D2;
template<typename T> class BB_CRTP;
template<int> struct ENUM_CLASS;

struct BTYPE{
    typedef TSHARED_PTR<BTYPE> TSharedPtr;
    int ii;
    virtual const char *  ID()  { return "BTYPE";}
    virtual BTYPE *clone()      {return new BTYPE(*this);}
    virtual const std::string str()   {return boost::str(boost::format("BTYPE: ii(%1%), type(%2%)")%ii%"Base");}
    virtual void Combine(User * ptr);
};

struct DTYPE: public BTYPE{
    virtual const char *  ID()  {return "DTYPE";}
    virtual DTYPE *clone()      {return new DTYPE(*this);}
    virtual const std::string str()   {return boost::str(boost::format("DTYPE: ii(%1%), type(%2%)")%ii%"Derived");}     virtual void Combine(User * ptr);
};

class User
{
public:
    User():m_pBBptr((BB *)NULL), m_eType(ENUM_A), m_pDType((DTYPE *)NULL) {}
    ~User() {}
    void Attach(const BB * ptr) { m_pBBptr=ptr;}
    void DoIt(void);
    void action(const ENUM_CLASS<ENUM_A>&) {std::cout << "ENUM_A\n";}
    void action(const ENUM_CLASS<ENUM_B>&) {std::cout << "ENUM_B\n";}
    void action();
    void SetType(ENUM_TYPE etype) {m_eType=etype;}
    void SetDType(DTYPE & obj) {m_pDType = DTYPE::TSharedPtr(obj.clone());}
    BTYPE & GetDType() {return *m_pDType;}
    virtual void Combine(BTYPE * ptr) { return ptr->Combine(this);}
    virtual void Combine(DTYPE * ptr);
private:
    void DoIt(const D1 *) { std::cout << "User: D1 attached\n"; }
    void DoIt(const D2 *) { std::cout << "User: D2 attached\n"; }
private:
    const BB * m_pBBptr;
    ENUM_TYPE m_eType;
    
    DTYPE::TSharedPtr m_pDType;
    // friend declaration
    friend class BB_CRTP<D1>; 
    friend class BB_CRTP<D2>;
};

class BBB
{
};

class BB:public BBB
{
public:
    BB(void) {}
    virtual void DoIt(User * ptr) const=0;
    virtual ~BB(void) {}
};


template <typename Derived>
class BB_CRTP: public BB
{
public:
    virtual void DoIt(User * ptr) const;
};

template <typename Derived>
void BB_CRTP<Derived>::DoIt(User * ptr) const
{ 
    ptr->DoIt(static_cast<const Derived * const>(this)); 
}

class D1: public BB_CRTP<D1>
{
//public:
//    D1(void){}
//    virtual ~D1(void) {}
};

class D2: public BB_CRTP<D2>
{
//public:
//    D2(void){}
//    virtual ~D2(void) {}
};


//*******************************
//*   Bridge pattern
//*******************************
class BridgeImpl;
class BridgeImplA;
class BridgeImplB;

enum ENUM_CONTEXT{
    CONTEXT_NULL, CONTEXT_A, CONTEXT_B
};

struct Context{
    ENUM_CONTEXT Type;
    Context(ENUM_CONTEXT _type): Type(_type) {}
};

struct ContextA: Context{
    ContextA(): Context(CONTEXT_A) {}
};

struct ContextB: Context{
    ContextB(): Context(CONTEXT_B) {}
};

class BridgeImpl;

class BridgeBase{
public:
    typedef TSHARED_PTR<BridgeImpl> TSharedPtrImpl;
    typedef TSHARED_PTR<Context> TSharedPtrContext;
    
    void func(void);
    
    void SetContext(const Context & oContext); 

    ENUM_CONTEXT GetContextType() const;

private:
    TSharedPtrImpl m_pImpl;
    TSharedPtrContext m_pContext;

    friend class BridgeImpl;
    friend class BridgeImplA;
    friend class BridgeImplB;
};

class BridgeImpl{
public:
    BridgeImpl(BridgeBase * ptr): m_oBridgePtr(ptr) {}
    virtual ~BridgeImpl() {};
    virtual void func () = 0;

protected:
    BridgeBase * m_oBridgePtr;
};

class BridgeImplA:public BridgeImpl{
public:
    BridgeImplA(BridgeBase * ptr): BridgeImpl(ptr) {}
    ~BridgeImplA(void) {}
    virtual void func() { std::cout << "BridgeImplA with context " << m_oBridgePtr->GetContextType() << std::endl;}
};

class BridgeImplB:public BridgeImpl{
public:
    BridgeImplB(BridgeBase * ptr): BridgeImpl(ptr) {}
    ~BridgeImplB(void) {}
    virtual void func() { std::cout << "BridgeImplB with context " << m_oBridgePtr->GetContextType() <<std::endl;}
};


//******************************************

void User::DoIt(void) 
{
    //this->m_pBBptr->DoIt(this);
    this->m_pBBptr->DoIt(this);
}


template<>
struct ENUM_CLASS<ENUM_A>
{
    void action(User *ptr)
    {   
        ptr->action(ENUM_CLASS<ENUM_A>());
    }
};

template<>
struct ENUM_CLASS<ENUM_B>
{
    void action(User *ptr)
    {   
        ptr->action(ENUM_CLASS<ENUM_B>());
    }
};

void User::action()
{
    if(m_eType==ENUM_A){
        action(ENUM_CLASS<ENUM_A>());
    }
    else if(m_eType==ENUM_B){
        action(ENUM_CLASS<ENUM_B>());
    } 
}

void BTYPE::Combine(User * ptr)
{
    ptr->Combine(this);
}

void DTYPE::Combine(User * ptr)
{
    ptr->Combine(this);
}

void User::Combine(DTYPE * ptr)
{
    std::cout << "User::Combine(DTYPE *)" << std::endl;
}

//===================

ENUM_CONTEXT BridgeBase::GetContextType() const{
    return m_pContext->Type;
}

void BridgeBase::SetContext(const Context & oContext){
    switch(oContext.Type){
    case CONTEXT_A:
        m_pImpl = TSharedPtrImpl(new BridgeImplA(this));
        break;
    case CONTEXT_B:
        m_pImpl = TSharedPtrImpl(new BridgeImplB(this));
        break;
    }
    m_pContext=TSharedPtrContext(new Context(oContext));
}

void BridgeBase::func(void){ // generic interface layer
    m_pImpl->func();
}


//**** Replacement of dynamic_cast

struct Master;

struct BaseData
{
    virtual ~BaseData() {}
    virtual void Do(int x, double y, Master *ptr)=0;

    virtual bool IsDerivedData1(void) { return false;}
    virtual bool IsDerivedData2(void) { return false;}
};

template<typename D>
struct CRTP: BaseData
{
    virtual void Do(int x, double y, Master *ptr); 
};

struct DerivedData1: CRTP<DerivedData1>
{
    void f(int x) {}

    virtual bool IsDerivedData1(void) { return true;}
};


struct DerivedData2: CRTP<DerivedData2>
{
    void g(double x) {}

    virtual bool IsDerivedData2(void) { return true;}
};

struct Master
{
    void Do(int x, double y, BaseData *ptr)
    {
        // ptr->Do(x,y,this);
        if(ptr->IsDerivedData1())
            Do(x,y,static_cast<DerivedData1 *>(ptr));
        else if(ptr->IsDerivedData2())
            Do(x,y,static_cast<DerivedData2 *>(ptr));
    }
    
    void Do(int x, double y, DerivedData1 *ptr)
    {
        std::cout << "Derived 1" << std::endl;
        ptr->f(1);
    }

    void Do(int x, double y, DerivedData2 *ptr)
    {
        std::cout << "Derived 2" << std::endl;
        ptr->g(3.14);
    }
};


template<typename D>
void CRTP<D>::Do(int x, double y, Master *ptr) { ptr->Do(x,y, static_cast<D *>(this));}

//====================

#endif /* __MY_DESIGN_PATTERN__ */

