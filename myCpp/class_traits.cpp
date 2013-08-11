#include <iostream>
#include <vector>
#include "class_traits.h"

#define ARR_END(arr)   ((arr)+sizeof(arr)/sizeof((arr)[0]))

namespace Serialize{

void XML_BEGIN(std::ostream &os, const char * NodeName) {
os << "<data name=\"" << NodeName  << "\">\n";
}

void XML_END(std::ostream &os) {
    os << "\n</data>\n";
}

} /* end of namespace Serialize */

std::string XLM_ostream::default_tag("data");

//************* TreeNode *************
// TreeNode::iterator::
TreeNode::iterator & TreeNode::iterator::operator +=(size_t offset)
{
    return *this;
}



bool DDD::Serialize(std::ostream & os) const 
{
    return XML_OUT(os, "DATA", ii);
}

void class_traits_test()
{
    int i=123;
    double dd=0.456;

    XML_OUT(std::cout, "INTEGER", i);
    XML_OUT(std::cout, "DOUBLE", dd);

    double darr[]={0.123, 0.456, 0.789};
    std::vector<double> dvec(darr, ARR_END(darr));
    XML_OUT(std::cout, "DBL VECTOR", dvec);

    int * iptr=&i;
    XML_OUT(std::cout, "INT PTR", iptr);

    double * dptr=&dd;
    XML_OUT(std::cout, "DBL PTR", dptr);

    TSHARED_PTR<int> isptr(new int(12345));
    XML_OUT(std::cout, "INT SHARED_PTR", isptr);

    DDD dclass;
    XML_OUT(std::cout, "ISERIALIZE", (ISerialize) dclass);

    std::vector<std::vector<double> > ddvec;
    ddvec.push_back(dvec);
    XML_OUT(std::cout, "DBL VEC_VEC", ddvec);

    XML_OUT(std::cout, "NULL", (char *)NULL);

}

