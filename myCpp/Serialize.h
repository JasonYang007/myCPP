#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <regex>
#include <memory>

//
//class G
//{
//public:
//    G(int ii=123, double dd=3.14): ival(ii), dval(dd) {}
//
//private:
//    friend class boost::serialization::access;
//
//    template<class Archive>
//    void serialize(Archive & ar, const unsigned int version)
//    {
//        ar & ival;
//        ar & dval;
//    }
//    int ival;
//    double dval;
//};


//************* Tree implementation *******************
using namespace std;

typedef std::tr1::regex  TRegex;
typedef std::tr1::cmatch TCmatch;
using std::tr1::regex_search;
using std::tr1::regex_match;

class Serializer
{
public:
	struct TreeNode
	{
		typedef std::shared_ptr<TreeNode> TSharedPtr;
		typedef std::list<TSharedPtr> LIST;
        typedef std::map<std::string, TreeNode *> MAP;
		typedef LIST::iterator iterator;
		typedef LIST::const_iterator const_iterator;

        std::string Type;
        std::string ID;
		std::string Data;

		LIST m_oList;
        MAP  m_oMap;
		TreeNode * m_pParent;

        TreeNode(const std::string & _ID, const std::string & _type, TreeNode *parent);
        void addChild(const std::string & _ID, const std::string & _type);
		TSharedPtr getLastChild(void) {return m_oList.back();}
        TSharedPtr operator [](const std::string & key) {return TSharedPtr(m_oMap[key]);}
		TreeNode * getParent(void) {return m_pParent;}
        bool isLeaf(void) {return m_oList.empty();}
        void print(ostream & os);

		iterator begin(void)  {return m_oList.begin();}
		const_iterator begin(void) const {return m_oList.begin();}
		iterator end(void)  {return m_oList.end();}
		const_iterator end(void) const {return m_oList.end();}
	};

	Serializer(void): m_pRoot(new TreeNode("ROOT", "object", NULL)), m_pActive(m_pRoot.get()) {}

    void beginNode(const std::string & _ID, const std::string & _type=std::string("data"));
	void setData(const std::string & data);
	void endNode(void);
	void print(ostream & os);
    
    static TRegex m_oRegEx;

private:
	void print(ostream & os, TreeNode::TSharedPtr nodePtr);
	TreeNode::TSharedPtr m_pRoot;
	TreeNode * m_pActive;
};

typedef enum {A, B } ENUM_Alpha;
typedef enum {One, Two, Three} ENUM_Digit;

ostream& operator << (ostream & os, ENUM_Alpha vv)
{
    os << (vv==A? "A": "B");
    return os;
}

ostream& operator << (ostream & os, ENUM_Digit vv)
{
    os << (vv==One? "One": vv==Two? "Two": "Three");
    return os;
}


#endif
