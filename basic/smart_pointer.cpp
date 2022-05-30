#include <iostream>

template <typename T>
class unique_ptr
{
    T *ptr = nullptr;

public:
    unique_ptr() {}
    explicit unique_ptr(T *ptr) : ptr(ptr) {}
    ~unique_ptr()
    {
        std::cout << "-------------(unique_ptr) destructor called-----------" << std::endl;
        delete ptr;
    }
    unique_ptr(const unique_ptr &) = delete;
    unique_ptr &operator=(const unique_ptr &) = delete;

    unique_ptr(unique_ptr &&r) : ptr(r.ptr)
    {
        r.ptr = nullptr;
    }
    unique_ptr &operator=(unique_ptr &&r)
    {
        delete ptr;
        ptr = r.ptr;
        r.ptr = nullptr;
        return *this;
    }
    T &operator*() noexcept { return *ptr; }
    T *operator->() noexcept { return ptr; }
    T *get() noexcept { return ptr; }
};

template <typename T, class... Args>
unique_ptr<T>
make_unique(Args &&...args)
{
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
class shared_ptr
{

    T *ptr = nullptr;
    std::size_t *count = nullptr;

    void release()
    {
        if (count == nullptr)
        {
            return;
        }
        --*count;
        if (*count == 0)
        {
            delete ptr;
            ptr = nullptr;
            delete count;
            count = nullptr;
        }
    }

public:
    shared_ptr() {}
    explicit shared_ptr(T *ptr) : ptr(ptr), count(new std::size_t(1))
    {
    }
    ~shared_ptr()
    {
        release();
        std::cout << "-------------(shared_ptr) destructor called-----------" << std::endl;
    }
    shared_ptr(const shared_ptr &r) : ptr(r.ptr), count(r.count)
    {
        ++*count;
    }
    shared_ptr &operator=(const shared_ptr &r)
    {
        if (*this == &r)
            return *this;

        release();
        ptr = r.ptr;
        count = r.count;
        ++*count;
    }
    shared_ptr(shared_ptr &&r) : ptr(r.ptr), count(r.count)
    {
        r.ptr == nullptr;
        r.count = nullptr;
    }
    shared_ptr &operator=(shared_ptr &&r)
    {
        release();
        ptr = r.ptr;
        count = r.count;
        r.ptr = nullptr;
        r.count = nullptr;
    }
    T &operator*() noexcept { return *ptr; }
    T *operator->() noexcept { return ptr; }
    T *get() noexcept { return ptr; }
};

template <typename T, class... Args>
shared_ptr<T>
make_shared(Args &&...args)
{
    return shared_ptr<T>(new T(std::forward<Args>(args)...));
}

int main()
{
    {
        unique_ptr<std::pair<int, int>> p;
        {
            auto q = make_unique<std::pair<int, int>>(2, 3);
            p = std::move(q);
        }
        std::cout << p->first << ':' << p->second << std::endl;
    }
    std::cout << "============================" << std::endl;
    {
        shared_ptr<std::pair<int, int>> sp = make_shared<std::pair<int, int>>(2, 3);
        {
            auto spq = sp;
            spq->first *= 2;
            spq->second *= 2;
        }
        std::cout << sp->first << ':' << sp->second << std::endl;
    }
}