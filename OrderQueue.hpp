// ============================================================================
//  File        : OrderQueue.hpp
//  Module      : TASK 1 - Order Queue Management Module
//  Owner       : <member 1 - put your name and TP number here>
//  Description : The data structure itself - a First-In-First-Out queue of
//                Order records. No cout in this file: screens and CSV file
//                handling belong in OrderModule, this file only holds and
//                moves data. Mirrors how Task 4 splits MenuBST from MenuModule.
// ============================================================================
#ifndef ORDER_QUEUE_HPP
#define ORDER_QUEUE_HPP

#include "Common.hpp"

// Fixed capacity of the queue. Reaching it is the "system overload" case the
// brief asks the module to handle without crashing.
const int MAX_QUEUE_ORDERS = 100;

// A first-in-first-out queue of Order records, implemented with a plain
// array and two moving indices (front/rear) - no STL container involved.
//
// Chosen for Task 1 because student orders must be served strictly in the
// order they arrive at the kiosk. A Queue's only two operations - enqueue at
// the back, dequeue from the front - are exactly that fairness guarantee,
// which is why it is the natural fit over a Stack (LIFO, wrong order) or a
// tree/BST (ordered by key, not by arrival time).
class OrderQueue
{
public:
    OrderQueue();

    bool isEmpty() const;             // O(1)
    bool isFull() const;              // O(1)
    int  size() const;                // O(1)

    // Adds an order to the back of the queue.
    // O(1) - circular indexing means no existing element ever has to move.
    bool enqueue(const Order& order);

    // Removes the order at the front of the queue into 'output'.
    // O(1) - only the front index moves; nothing is shifted.
    bool dequeue(Order& output);

    // Copies the front order into 'output' without removing it.
    // O(1).
    bool peek(Order& output) const;

    // Read-only access to the order 'offsetFromFront' places behind the
    // front (0 = the front order itself). Used only so OrderModule can print
    // the whole pending list without exposing the array directly.
    // O(1) - direct index arithmetic, no traversal.
    Order getAt(int offsetFromFront) const;

private:
    Order orders[MAX_QUEUE_ORDERS];
    int   front;   // index of the order that has been waiting longest
    int   rear;    // index of the most recently added order
    int   count;   // how many orders are currently in the queue
};

#endif // ORDER_QUEUE_HPP
