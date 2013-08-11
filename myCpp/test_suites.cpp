#include "test_suites.h"

namespace test1
{

int level=0;

std::string get_indent(void)
{
    return std::string(2*level,' ');
}

void check_const(const B & bb)
{
    const B * ptr = & bb;
    B * bptr = const_cast<B *>(& bb);
}

template<typename E>
D<E> creatD(E & expr, int id)
{
    return D<E>(expr, id);
}

template<typename E>
D<D<E> > app(E expr)
//D<E> app(E & expr)
{
    int id = expr.id;

    return D<D<E> >(D<E>(expr, id+1), id+2);
}


B const_test(void)
{
    struct _dummy { ~_dummy() { std::cout << "dummy destructor called" << std::endl;} } Dummy;

    return B();
}

void test(void)
{
    const_test();
    B bb;

    check_const(bb);
    // applyD<3>(applyD<2>(applyD<1>(bb))).action();
    //app(app(bb)).action();
    int id = bb.id;
    app(bb).action();
    id = bb.id;
}

}

void test(void)
{
    test1::test();
}