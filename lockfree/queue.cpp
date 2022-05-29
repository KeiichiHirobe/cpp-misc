#include <unistd.h> // usleep
#include <iostream>
#include <stdlib.h> // rand
#include <atomic>
#include <string>

// https://github.com/kumagi/lockfree/blob/master/queue.hpp
// をもとに、atomicを利用したり、ABA対応し、少しコードを変更しただけ

namespace lockfree
{

    template <typename T>
    class queue
    {
    private:
        class Node
        {
        public:
            const T mValue;
            std::atomic<Node *> mNext;

            Node(const T &v) : mValue(v), mNext(nullptr) {}
            Node() : mValue(), mNext(nullptr) {}
            Node(const Node &) = delete;
            Node &operator=(const Node &) = delete;
        };
        std::atomic<Node *> mHead, mTail;

    public:
        queue(const queue &) = delete;
        queue &operator=(const queue &) = delete;
        queue()
        {
            Node *sentinel = new Node();
            mHead.store(sentinel);
            mTail.store(sentinel);
        }

        void enq(const T &v)
        {
            Node *node = new Node(v);
            while (1)
            {
                Node *last = mTail.load();
                Node *next = last->mNext;
                if (next == nullptr)
                {
                    if ((last->mNext).compare_exchange_weak(next, node))
                    {
                        mTail.compare_exchange_weak(last, node);
                        return;
                    }
                }
                else
                {
                    mTail.compare_exchange_weak(last, next);
                }
            }
        }
        T deq()
        {
            while (1)
            {
                Node *first = mHead.load();
                Node *last = mTail.load();
                Node *next = first->mNext;

                if (first == last)
                {
                    if (next == nullptr)
                    {
                        while (mHead.load()->mNext == nullptr)
                        {
                            usleep(1);
                        }
                        continue;
                    }
                    mTail.compare_exchange_weak(last, next);
                }
                else
                {
                    // mHeadが指す先はsentinelなのでそのnextを返す
                    T result = next->mValue;
                    // sentinelを移動させる
                    if (mHead.compare_exchange_weak(first, next))
                    {
                        delete first;
                        return result;
                    }
                }
            }
        }

        bool deq_delete()
        {
            while (1)
            {
                Node *first = mHead.load();
                Node *last = mTail.load();
                Node *next = first->mNext;

                if (first == last)
                {
                    if (next == nullptr)
                    {
                        return false;
                    }
                    mTail.compare_exchange_weak(last, next);
                }
                else
                {
                    T result = next->mValue;
                    if (mHead.compare_exchange_weak(first, next))
                    {
                        delete first;
                        return true;
                    }
                }
            }
        }
        bool empty() const
        {
            return mHead.load()->mNext == nullptr;
        }
        bool size() const
        {
            const Node *it = mHead.load();
            int num;
            while (it != NULL)
            {
                it = it->mNext;
                num++;
            }
            return num - 1;
        }

        ~queue()
        {
            while (deq_delete())
                ;
            delete mHead.load();
        }
    };
};

int main(void)
{
    struct person
    {
        int id;
        std::string name;
    };

    lockfree::queue<person> que;
    person p1{1, "b"};
    person p2{2, "b"};
    que.enq(p1);
    que.enq(p2);

    std::cout << que.deq().id << std::endl;
    std::cout << que.deq().id << std::endl;
}