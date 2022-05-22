#include <iostream>
#include <thread>
#include <atomic>

// http://mdf356.blogspot.com/2015/06/the-difficulty-of-lock-free-programming.html
// をもとに一部改修。このブログの主題としては、
// atomicがサポートされるC++11以前に書いたコードにおいて、16byteのatomicなwriteはcmpxchgにて行うことができるが、atomicなreadはCPU命令として存在しない。
// そのため、readしたデータの中で、8byteは新であるが、8byteは旧ということがあり得る。筆者はpopの時のみversionを更新していていたが、pushの時にも更新することで解決したとのこと。
// なお、このコードはatomicを使っているのでpopのみで問題ない。また、16byteに該当するのは、atomic_itemクラスのことである。

template <typename any_t, any_t *any_t::*next>
struct atomic_recycling_stack
{

    struct atomic_item
    {
        any_t *head;
        uintptr_t nonce;
    };

    std::atomic<atomic_item> item;

    void push(any_t *elem)
    {
        atomic_try_update_unsafe(&item,
                                 [elem](atomic_item *ref_v) -> bool
                                 {
                                     elem->*next = ref_v->head;
                                     ref_v->head = elem;
                                     return true;
                                 });
    }

    any_t *pop()
    {
        any_t *oldhead;
        atomic_try_update_unsafe(&item,
                                 [&oldhead](atomic_item *ref_v) -> bool
                                 {
                                     oldhead = ref_v->head;
                                     if (!oldhead)
                                     {
                                         return false;
                                     }
                                     ref_v->head = oldhead->*next;
                                     ref_v->nonce++;
                                     return true;
                                 });
        return oldhead;
    }
};

template <typename any_t, typename lambda_t>
bool atomic_try_update_unsafe(
    std::atomic<any_t> *item,
    lambda_t func)
{
    any_t old = item->load();
    any_t newer;
    do
    {
        newer = old;
        if (!func(&newer))
        {
            return false;
        }
    } while (!item->compare_exchange_weak(old, newer));
    return true;
}

int main(void)
{
    struct person
    {
        int id;
        std::string name;
        person *nextPerson;
    };

    atomic_recycling_stack<person, &person::nextPerson> s;

    s.push(new person{2, "b"});
    s.push(new person{1, "a"});
    delete s.pop();
    person *poped = s.pop();
    poped->name = "c";
    poped->id = 3;
    s.push(poped);
    // 3
    std::cout << s.pop()->id << std::endl;
}