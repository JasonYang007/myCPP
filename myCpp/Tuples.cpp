#include "Tuples.h"

namespace tuples 
{

tNULL ignore;

inline bool operator == (const tNULL & p1, const tNULL & p2) {return true;}

inline bool operator != (const tNULL & p1, const tNULL & p2) {return false;}

inline bool operator < (const tNULL & p1, const tNULL & p2) {return false;}

inline bool operator <= (const tNULL & p1, const tNULL & p2) {return true;}

inline bool operator > (const tNULL & p1, const tNULL & p2) {return false;}

inline bool operator >= (const tNULL & p1, const tNULL & p2) {return true;}


iostream_proxy set_open(char c) 
{
    return iostream_proxy(NULL, c,'\0','\0');
}

iostream_proxy set_close(char c) 
{
    return iostream_proxy(NULL, '\0',c,'\0');
}

iostream_proxy set_delimiter(char c) 
{
    return iostream_proxy(NULL,'\0','\0',c);
}

// istream_proxy for iostream
iostream_proxy operator >> (std::istream & istr, iostream_proxy proxy)
{
    proxy.iosptr = & istr;
    if(proxy.odelim == '\0') proxy.odelim='(';
    if(proxy.cdelim == '\0') proxy.cdelim=')';
    if(proxy.delim  == '\0') proxy.delim=',';
    return proxy;
}

iostream_proxy operator << (std::ostream & ostr, iostream_proxy proxy)
{
    proxy.iosptr = & ostr;
    if(proxy.odelim == '\0') proxy.odelim='(';
    if(proxy.cdelim == '\0') proxy.cdelim=')';
    if(proxy.delim  == '\0') proxy.delim=',';
    return proxy;
}

// manipulator for istream_proxy
iostream_proxy & iostream_proxy::operator << (std::ostream & (*manip)(std::ostream &))
{
    std::ostream & ostr = *dynamic_cast<std::ostream *>(iosptr);
    ostr << manip;
    return *this;
}

iostream_proxy & iostream_proxy::operator << (std::ios_base & (*manip)(std::ios_base &))
{
    std::ostream & ostr = *dynamic_cast<std::ostream *>(iosptr);
    ostr << manip;
    return *this;
}



iostream_proxy & iostream_proxy::operator >> (std::istream & (*manip)(std::istream &))
{
    std::istream & istr = *dynamic_cast<std::istream *>(iosptr);
    istr >> manip;
    return *this;
}

iostream_proxy & iostream_proxy::operator >> (std::ios_base & (*manip)(std::ios_base &))
{
    std::istream & istr = *dynamic_cast<std::istream *>(iosptr);
    istr >> manip;
    return *this;
}

} /* end of namespace */

