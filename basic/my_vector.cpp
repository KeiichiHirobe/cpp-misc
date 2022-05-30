#include <iostream>
#include <array>

template <typename T, typename Allocator = std::allocator<T>>
class vector
{
public:
    using value_type = T;
    using pointer = T *;
    using const_pointer = const pointer;
    using reference = value_type &;
    using const_reference = const value_type &;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    vector(const allocator_type &alloc) noexcept : alloc(alloc)
    {
    }
    vector() : vector(allocator_type()) {}
    vector(size_type size, const allocator_type &alloc = allocator_type()) : vector(alloc)
    {
        resize(size);
    }
    vector(size_type size, const_reference value, const allocator_type &alloc = allocator_type()) : vector(alloc)
    {
        resize(size, value);
    }
    template <typename InputIterator>
    vector(InputIterator first, InputIterator last, const allocator_type &alloc = allocator_type()) : vector(alloc)
    {
        reserve(std::distance(first, last));
        for (auto i = first; i != last; ++i)
        {
            push_back(*i);
        }
    }
    vector(std::initializer_list<value_type> init, const allocator_type &alloc = allocator_type()) : vector(alloc)
    {
        vector(std::begin(init), std::end(init), alloc);
    }

    // destructorはデフォルトだと空
    // classのdestructor -> memberのfieldの定義逆順にdestructor の順で呼ばれる
    // newで確保したオブジェクトは自前でdestructorやメモリ解放が必要
    ~vector()
    {
        // 要素を末尾から先頭に向かう順番で破棄
        clear();
        // 生のメモリを解放
        deallocate();
    }

    //   copy constructor
    vector(const vector &r) : alloc(traits::select_on_container_copy_construction(r.alloc))
    {
        reserve(r.size());
        for (auto dest = first, src = r.begin(), last = r.end();
             src != last;
             ++dest, ++src)
        {
            construct(dest, *src);
        }
        last = first + r.size();
    }

    // move constructor
    vector(vector &&r) : alloc(r.alloc), first(r.first), last(r.last), reserved_last(r.reserved_last)
    {
        r.first = nullptr;
        r.last = nullptr;
        r.reserved_last = nullptr;
    }

    // copyは自分自身へのcopyをcheckする
    vector &operator=(const vector &r)
    {
        if (this == &r)
        {
            return *this;
        }
        if (size() == r.size())
        {
            copy(r.begin(), r.end(), begin());
        }
        else if (capacity() >= r.size() && r.size() >= size())
        {
            copy(r.begin(), r.begin() + size(), begin());
            for (auto src_iter = r.begin() + size(), src_end = r.end(); src_iter != src_end; ++src_iter, ++last)
            {
                construct(last, *src_iter);
            }
        }
        else if (capacity() >= r.size() && r.size() < size())
        {
            copy(r.begin(), r.end(), begin());
            resize(r.size());
        }
        else
        {
            clear();
            reserve(r.size());

            for (auto src_iter = r.begin(), src_end = r.end(), dest_iter = begin();
                 src_iter != src_end; ++src_iter, ++dest_iter, ++last)
            {
                construct(dest_iter, *src_iter);
            }
        }
        return *this;
    }

    // moveは自分自身へのmoveはcheckしないのが通例
    vector &operator=(vector &&r)
    {
        clear();
        deallocate();
        alloc = r.alloc;
        first = r.first;
        last = r.last;
        reserved_last = r.reserved_last;
        r.first = nullptr;
        r.last = nullptr;
        r.reserved_last = nullptr;
    }

    void resize(size_type sz)
    {
        if (sz < size())
        {
            auto diff = size() - sz;
            destroy_until(rbegin() + diff);
            last = first + sz;
        }
        else if (sz > size())
        {
            reserve(sz);
            for (; last != reserved_last; ++last)
            {
                construct(last);
            }
        }
    }
    void resize(size_type sz, const_reference value)
    {
        if (sz < size())
        {
            auto diff = size() - sz;
            destroy_until(rbegin() + diff);
            last = first + sz;
        }
        else if (sz > size())
        {
            reserve(sz);
            for (; last != reserved_last; ++last)
            {
                construct(last, value);
            }
        }
    }

    void shrink_to_fit()
    {
        if (size() == capacity())
        {
            return;
        }
        auto ptr = allocate(size());
        auto current_size = size();
        for (auto raw_ptr = ptr, iter = begin(), iter_end = end();
             iter != iter_end;
             ++iter, ++raw_ptr)
        {
            construct(raw_ptr, *iter);
        }
        clear();
        deallocate();
        first = ptr;
        last = ptr + current_size;
        reserved_last = last;
    }
    // v.push_back(std::string("b"));
    // push_backの場合は値コピーまたはムーブ処理が発生する
    // 一時オブジェクトstd::string("b")のデストラクタも呼ばれる
    void push_back(const_reference value)
    {
        if (size() + 1 > capacity())
        {
            auto c = size();
            if (c == 0)
            {
                c = 1;
            }
            else
            {
                c *= 2;
            }
            reserve(c);
        }
        construct(last, value);
        ++last;
    }

    reference operator[](std::size_t i)
    {
        return first[i];
    }
    const_reference operator[](std::size_t i) const
    {
        return first[i];
    }
    reference at(std::size_t i)
    {
        if (i >= size())
        {
            throw std::out_of_range("index is out of range.");
        }
        return first[i];
    }
    const_reference at(std::size_t i) const
    {
        if (i >= size())
        {
            throw std::out_of_range("index is out of range.");
        }
        return first[i];
    }

    iterator begin() noexcept
    {
        return first;
    }
    iterator end() noexcept
    {
        return last;
    }
    const_iterator begin() const noexcept
    {
        return first;
    }
    // fixme
    iterator end() const noexcept
    {
        return last;
    }
    const_iterator cbegin() const noexcept
    {
        return first;
    }
    const_iterator cend() const noexcept
    {
        return last;
    }
    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator{last};
    }
    reverse_iterator rend() noexcept
    {
        return reverse_iterator{first};
    }
    size_type size() const noexcept
    {
        return end() - begin();
        // return std::distance(begin(),end());
    }
    bool empty() const noexcept
    {
        return size() == 0;
    }
    size_type capacity() const noexcept
    {
        return reserved_last - first;
    }
    reference front()
    {
        return *first;
    }
    reference front() const
    {
        return *first;
    }
    reference back()
    {
        return *(last - 1);
    }
    reference back() const
    {
        return *(last - 1);
    }
    void clear() noexcept
    {
        destroy_until(rend());
    }

private:
    pointer first = nullptr;
    pointer last = nullptr;
    pointer reserved_last = nullptr;
    allocator_type alloc;
    using traits = std::allocator_traits<allocator_type>;

    pointer allocate(size_type n)
    {
        return traits::allocate(alloc, n);
    }
    void deallocate()
    {
        traits::deallocate(alloc, first, capacity());
    }
    void construct(pointer ptr)
    {
        traits::construct(alloc, ptr);
    }
    void construct(pointer ptr, const_reference value)
    {
        traits::construct(alloc, ptr, value);
    }
    void construct(pointer ptr, value_type &&value)
    {
        traits::construct(alloc, ptr, std::move(value));
    }
    void destroy(pointer ptr)
    {
        traits::destroy(alloc, ptr);
    }
    void destroy_until(reverse_iterator rend)
    {
        for (auto riter = rbegin(); riter != rend; ++riter, --last)
        {
            destroy(&*riter);
        }
    }

    void reserve(size_type sz)
    {
        if (sz <= capacity())
        {
            return;
        }
        auto ptr = allocate(sz);

        auto old_first = first;
        auto old_last = last;
        auto old_capacity = capacity();

        first = ptr;
        last = first;
        reserved_last = first + sz;

        for (auto old_iter = old_first; old_iter != old_last; ++old_iter, ++last)
        {
            construct(last, std::move(*old_iter));
        }

        for (auto riter = reverse_iterator(old_last), rend = reverse_iterator(old_first); riter != rend; ++riter)
        {
            destroy(&*riter);
        }

        traits::deallocate(alloc, old_first, old_capacity);
    }
};

int main()
{
    vector<int> v(10);
    std::cout << "=============" << std::endl;
    /*
    for (auto i = 0; i != 10; ++i)
    {
        v[i] = i;
    }
    */
    // v.resize(13);
    std::cout << v.front() << std::endl;

    /*
    std::for_each(v.begin(), v.end(),
                  [](auto x)
                  { std::cout << x; });

    vector<int> w = {1, 2, 3, 4, 5};

    std::array<int, 5> a{1, 2, 3, 4, 5};

    std::for_each(w.begin(), w.end(),
                  [](auto x)
                  { std::cout << x; });
    */
}