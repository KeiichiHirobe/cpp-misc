#include <module1/print_int.hpp>
#include <module2/hoo.hpp>

int main(void)
{
    Foo f(10);
    print_int(f.member_function());
    print_int(f.member_function2());
    print_int(f.member_function3());
}