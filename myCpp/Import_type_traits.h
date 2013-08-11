#ifndef __IMPORT_TYPE_TRAITS_H__
#define __IMPORT_TYPE_TRAITS_H__

#ifdef __GNUC__
    #include <tr1/type_traits>
    namespace TT=std::tr1;
#else
    #include <type_traits>
    namespace TT=std;
#endif
    
using TT::is_base_of;
using TT::is_same;
using TT::false_type;
using TT::remove_cv;
using TT::remove_reference;
using TT::add_const;
using TT::true_type;
using TT::false_type;
using TT::is_class;
using TT::is_function;
using TT::is_void;

#endif
