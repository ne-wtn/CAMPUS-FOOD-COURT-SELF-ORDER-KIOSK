// ============================================================================
//  File        : OrderQueue.cpp
//  Module      : TASK 1 - Order Queue Management Module
//  Owner       : <member 1 - put your name and TP number here>
//  Description : Implementation of the array-based FIFO order queue.
// ============================================================================
#include "OrderQueue.hpp"

OrderQueue::OrderQueue()
    : front(0), rear(-1), count(0)
{
}

bool OrderQueue::isEmpty() const
{
    return count == 0;
}

bool OrderQueue::isFull() const
{
    return count == MAX_QUEUE_ORDERS;
}

int OrderQueue::size() const
{
    return count;
}

// Circular indexing (the % operator) lets the same fixed-size array be
// reused forever instead of shifting every element down after a dequeue,
// which is what would be needed with a plain non-circular array queue.
bool OrderQueue::enqueue(const Order& order)
{
    if (isFull())
    {
        return false;   // caller reports this as "system overload"
    }

    rear = (rear + 1) % MAX_QUEUE_ORDERS;
    orders[rear] = order;
    count++;
    return true;
}

bool OrderQueue::dequeue(Order& output)
{
    if (isEmpty())
    {
        return false;   // caller reports this as "queue is empty"
    }

    output = orders[front];
    front  = (front + 1) % MAX_QUEUE_ORDERS;
    count--;
    return true;
}

bool OrderQueue::peek(Order& output) const
{
    if (isEmpty())
    {
        return false;
    }

    output = orders[front];
    return true;
}

Order OrderQueue::getAt(int offsetFromFront) const
{
    int index = (front + offsetFromFront) % MAX_QUEUE_ORDERS;
    return orders[index];
}
