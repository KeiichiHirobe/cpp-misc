#ifndef FOO_HPP
#define FOO_HPP
struct Foo
{
    int data_member;
    Foo(int n) : data_member(n){};
    // cppに書く
    int member_function() const noexcept;
    // クラス定義の中に定義されたメンバ関数は自動的にインライン関数になる
    int member_function2() const noexcept
    {
        return data_member;
    }
    int member_function3() const noexcept;
};
// 高速化のためにヘッダにメンバ関数を定義する場合はinlineが必要
// ただしinlineの指定はinlineするために指定するものではなく、ODR(One Definition Rule 全翻訳単位に1つしか定義できないルール)回避のためである
inline int Foo::member_function3() const noexcept
{
    return data_member;
}
#endif