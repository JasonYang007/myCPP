#ifndef __EXPR_TEMPLATE_H_
#define __EXPR_TEMPLATE_H_

#include <iostream>
#include <string>

extern void test(void);

namespace test1
{

extern int level;
std::string get_indent(void);

template<typename T>
std::ostream & print_obj(const char * name, T * objptr, const char *comment)
{
    std::cout << get_indent() << name << "(" << objptr << "): " << comment << std::endl;
    return std::cout;
}

template<typename T>
std::ostream & print_obj(const char * name, int id, T * objptr, const char *comment)
{
    std::cout << get_indent() << name << "[" << id << "]" << "(" << objptr << "): " << comment << std::endl;
    return std::cout;
}

struct B
{
    enum { lvalue =true };

    B(): id(0) { print_obj("B",this,"constructed"); }
    ~B() { print_obj("B",this,"destroyed"); }
    B(const B&) { print_obj("B",this,"copy-constructed"); }

    void print_info(const char * tag)
    {
        print_obj("B",this,tag);
    }
    void action(void)
    {
        print_info("BEGIN");
        ++id;
        print_info("END");
    }
    int id;
};

template<typename E>
struct D
{
    enum { lvalue = false };

    D(E _obj, int _id):
        obj(_obj), ptr_obj(NULL), id(_id)
    { 
        if(E::lvalue) ptr_obj = &_obj;
        print_obj("D", id, this,"constructed");
    }
    
    /*D(D & other): 
        obj(other.obj), ptr_obj(other.ptr_obj), id(other.id) 
    { 
        print_obj("D",id, this, "copy-constructed"); 
    }*/
    
    ~D() { print_obj("D", id, this,"destroyed");}

    void action(void)
    {
        print_info("BEGIN");
        ++level;

        std::cout << get_indent() << "<obj>" << std::endl;
        obj.action();
        std::cout << get_indent()<< "<ptr_obj>" << std::endl;
        if(ptr_obj != NULL) ptr_obj->action();

        --level;
        print_info("END");
    }

    void print_info(const char * tag)
    {
        //B tmp;
        print_obj("D",id,this,tag);
    }

    E obj;
    E * ptr_obj;
    int id;
};

template<int id, typename E>
D<E> applyD(E e)
{
    return D<E>(e, id);
}


} /* end of namespace */

#endif /* __EXPR_TEMPLATE_H_ */
