#include <iostream>

struct Integer
{
    int *ptr;

    friend Integer operator+(const Integer &l, const Integer &r);
    friend Integer operator+(Integer &&l, const Integer &r);
    friend Integer operator+(const Integer &l, Integer &&r);
    friend Integer operator+(Integer &&l, Integer &&r);

public:
    explicit Integer(int value = 0) : ptr(new int(value)) {}

    ~Integer()
    {
        delete ptr;
    }

    Integer(const Integer &r) : ptr(new int(*r.ptr)){}
    Integer &operator=(const Integer &r)
    {
        if (this != &r)
        {
            *ptr = *r.ptr;
        }
        return *this;
    }
    Integer(Integer &&r) : ptr(r.ptr)
    {
        r.ptr = nullptr;
    }
    // 本では返り値が Integer になっていたが誤り
    // (a = Integer(10)) = Integer(15);
    // の結果が10になってしまう。また、returnするときにコピーコンストラクタが呼ばれる
    Integer &operator=(Integer &&r)
    {
        delete ptr;
        ptr = r.ptr;
        r.ptr = nullptr;
        return *this;
    }

    Integer &operator+=(const Integer &r)
    {
        *ptr += *r.ptr;
        return *this;
    }
    Integer &operator-=(const Integer &r)
    {
        *ptr -= *r.ptr;
        return *this;
    }

    // lvalue
    // rvalue版があるので明示的に&が必要
    // 当然ながら返り値はInteger &ではだめ。スタックの参照を返すから
    Integer operator-() const &
    {
        Integer result(-*ptr);
        // compiler do
        // return std::move(result)
        return result;
    }

    // rvalue
    // -(a+a) 等に効果的
    // 意味的には以下と同じ
    // Integer operator-(Integer && THIS) {
    //   Integer * this = &THIS;
    // }
    Integer operator-() &&
    {
        // call move constructor here
        auto result = std::move(*this);
        *result.ptr = -*result.ptr;
        return result;
    }
};

Integer operator+(const Integer &l, const Integer &r)
{
    auto result = l;
    result += r;
    return result;
}

Integer operator+(Integer &&l, const Integer &r)
{
    auto result = std::move(l);
    result += r;
    return result;
}
Integer operator+(const Integer &l, Integer &&r)
{

    auto result = std::move(r);
    result += l;
    return result;
}
Integer operator+(Integer &&l, Integer &&r)
{
    // operator+(Integer&& l, const Integer& r) を呼んでいるだけ
    return std::move(l) + r;
}

int main()
{
    Integer a(100);
    auto b = -(a + a);
    auto c = a + a + a;
    (a = Integer(10)) = Integer(15);
    // 15
    std::cout << *a.ptr << std::endl;
}