#ifndef __SERDES_H__
#define __SERDES_H__

#include <string>
#include <map>
#include <cctype>

template<typename E> struct enum_traits;

template<typename E>
void fill_map(const char * keys, const E * enums, size_t len, std::map<std::string, E> & enumMap)
{
    std::string str(keys);
    size_t start = 0; 
    for(int ii=0; ii < len; ++ii)
    {
        size_t next = str.find(",", start);
        size_t next_ = next;
        assert(next != std::string::npos);
        /* trim the white space of the key */
        while(std::isspace(keys[start]) && start < next) ++start;
        while(std::isspace(keys[--next]) && start <= next) ;
        ++next;
        std::string key(keys+start, keys+next);
        
        //std::stringstream trimmer(std::stringstream::in | std::stringstream::out);
        //trimmer << key;
        //trimmer >> key;
        /* add the key, enum pair */
        enumMap[key]=enums[ii];
        start = next_ + 1;
    }
}

#define REGISTER_ENUM(CLASS, ENUM, ...)\
    template<>\
    struct enum_traits<CLASS::ENUM>:private CLASS\
    {\
        typedef std::map<std::string, ENUM> TEnumMap;\
        static TEnumMap & getEnumMap(void)\
        {\
            static char keys[] = #__VA_ARGS__ ",";\
            static const ENUM enums[] = {__VA_ARGS__};\
            static TEnumMap enumMap;\
            \
            if(enumMap.size()==0) fill_map(keys, enums, sizeof(enums)/sizeof(enums[0]), enumMap);\
            return enumMap;\
        }\
    private:\
        enum_traits(void);\
    };

#define REGISTER_GLOBAL_ENUM(ENUM, ...)\
    template<>\
    struct enum_traits<ENUM>\
    {\
        typedef std::map<std::string, ENUM> TEnumMap;\
        static TEnumMap & getEnumMap(void)\
        {\
            static char keys[] = #__VA_ARGS__ ",";\
            static const ENUM enums[] = {__VA_ARGS__};\
            static TEnumMap enumMap;\
            \
            if(enumMap.size()==0) fill_map(keys, enums, sizeof(enums)/sizeof(enums[0]), enumMap);\
            return enumMap;\
        }\
    private:\
        enum_traits(void);\
    };

template<typename E>
typename enum_traits<E>::TEnumMap & getEnumMap(void)
{
    return enum_traits<E>::getEnumMap();
}

template<typename E>
void deserialize_enum(E & eval, const char * key)
{
    eval = enum_traits<E>::getEnumMap()[key];
}

template<typename E>
void deserialize_enum(E & eval, const std::string & key)
{
    eval =  enum_traits<E>::getEnumMap()[key];
}

template<typename E>
void serialize_enum(const E & eval, std::string & str)
{
    static std::map<E, std::string> enumMapRev;

    if(enumMapRev.size()==0)
    {
        typedef typename enum_traits<E>::TEnumMap TEnumMap;
        TEnumMap & enumMap = getEnumMap<E>();

        for(typename TEnumMap::const_iterator it = enumMap.begin(); it != enumMap.end(); ++it)
        {
            enumMapRev[it->second]=it->first;
        }

    }
    assert(enumMapRev.count(eval) > 0);
    str = enumMapRev[eval];
}

#endif /* __SERDES_H__ */

