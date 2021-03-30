# lock_free_queue
无锁队列

#include <iostream>
#include "lock_free_queue.hpp"

int main()
{
    lock_free_queue<int>que;
    for (int i =1;i<10;++i){
        que.enqueue(i);
    }

    int value = 0;

    que.dequeue(value);
}

