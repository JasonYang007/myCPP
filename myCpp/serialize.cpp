#include <iostream>
#include <sstream>
#include "Serialize.h"

TRegex Serializer::m_oRegEx("\\<\\s*(\\w+)\\s+name\\s*=\\s*\\\"(.*)\\\"\\s*\\>");

Serializer::TreeNode::TreeNode(const std::string & _ID, const std::string & _type, TreeNode *parent):
    Type(_type), ID(_ID), m_pParent(parent)
{
}

void 
Serializer::TreeNode::addChild(const std::string & _ID, const std::string & _type)
{
    TreeNode * ptr = new TreeNode(_type, _ID, this);
    m_oList.push_back(TSharedPtr(ptr));
    m_oMap[ptr->ID]=ptr;
}

void 
Serializer::TreeNode::print(ostream & os)
{
    os << "<" << this->Type << " name=\"" << this->ID << "\">" << std::endl;
    if(this->isLeaf())
    {
	    os << this->Data << std::endl;
    }
    else
    {
	    for(TreeNode::iterator it=this->begin(); it != this->end(); ++it)
	    {
		    (*it)->print(os);
	    }
    }
    os << "</" << this->Type << ">" << std::endl;
}


void Serializer::beginNode(const std::string & _ID, const std::string & _type)
{
	m_pActive->addChild(_type, _ID);
	m_pActive = m_pActive->getLastChild().get();
}

void Serializer::setData(const std::string & data)
{
	m_pActive->Data=data;
}

void Serializer::endNode(void)
{
	m_pActive=m_pActive->getParent();
}

void Serializer::print(ostream & os)
{
	m_pRoot->print(os);
}

template<typename T>
std::string getNameType(const std::string & name, const T &)
{
    std::string ss(name);
    ss += "@";
    ss += std::string(typeid(T).name());
    return ss;
}


void tree_test()
{
    
    ENUM_Alpha ea=A;
    ENUM_Digit ed=Three;

    std::cout << ed;
    

	Serializer oSerObj;

    std::string name("Dbl");
    oSerObj.beginNode(getNameType(name, double()));
    oSerObj.setData("1.234");
    oSerObj.endNode();

	oSerObj.beginNode(getNameType("Vect:double",std::vector<double>()));
	oSerObj.setData("1.234 3.141 5.678");
	oSerObj.endNode();

	oSerObj.beginNode(getNameType("Vect:Vector:double",std::vector<std::vector<double> >()));
		oSerObj.beginNode(getNameType("Vector:double",std::vector<double>()));
		oSerObj.setData("1.234 3.141 5.678");
		oSerObj.endNode();

		oSerObj.beginNode(getNameType("Vector:double",std::vector<double>()));
		oSerObj.setData("0.123 0.567 0.446");
		oSerObj.endNode();
	oSerObj.endNode();

	oSerObj.print(std::cout);
}


typedef enum {T_int, T_double, T_CC} ENUM_TYPE;

struct int_tag {};
struct double_tag {};
struct myCC_tag {};

struct myCC {};

template<ENUM_TYPE>
class C_traits;

template<> struct C_traits<T_int>:    int_tag    {typedef int type;};
template<> struct C_traits<T_double>: double_tag {typedef double type;};
template<> struct C_traits<T_CC>:     myCC_tag   {typedef myCC type;};

template<ENUM_TYPE E> void * getNew(void) {return new typename C_traits<E>::type();}

typedef void * TFactory(void) ;
TFactory * factoryArr[]={getNew<T_int>, getNew<T_double>, getNew<T_CC>};

void * GetNew(const ENUM_TYPE Te) {return factoryArr[Te]();}

void serialize_test()
{
    //int * iptr = (int *)GetNew(T_int);

    //G gg;
    //std::stringstream sstr(std::ios_base::out|std::ios_base::in);

    //{
    //    boost::archive::text_oarchive oa(sstr);
    //    oa << gg;
    //}

    //G new_gg;
    //{
    //    boost::archive::text_iarchive ia(sstr);
    //    ia >> new_gg;
    //}

    //std::cout << "typeid: G ==> \""<< typeid(G).name() << "\"" << std::endl;
    //void * ptr = NULL;
    //std::cout << "typeid: NULL ptr ==> \""<< typeid(ptr).name() << "\"" << std::endl;
}

void regex_test()
{
    TRegex rx("\\<\\s*(\\w+)\\s+name\\s*=\\s*\\\"(.*)\\\"\\s*\\>");
    TCmatch res;
    const char * str="<  data    name  =    \"ABC\"   >";

    regex_search(str, res, rx);
    std::cout << "{" << res[1] << "} " <<
                 "{" << res[2] << "} " <<
                 std::endl;
}