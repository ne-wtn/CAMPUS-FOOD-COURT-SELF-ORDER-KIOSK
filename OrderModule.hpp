// ============================================================================
//  File        : OrderModule.hpp
//  Module      : TASK 1 - Order Queue Management Module
//  Owner       : <member 1 - put your name and TP number here>
//  Suggested   : QUEUE (FIFO) - fair, first-come-first-served processing
//
//  >>> THIS IS A SKELETON. The owner of Task 1 fills it in. <<<
//  Nobody else edits this file.
//
//  The PUBLIC METHODS BELOW ARE A TEAM CONTRACT: KioskSystem.cpp already calls
//  them. You may add as many methods as you like, but do not rename or change
//  the signature of an existing one without telling the team, or the build
//  breaks for everybody.
// ============================================================================

#ifndef ORDER_MODULE_HPP
#define ORDER_MODULE_HPP

#include "Common.hpp"

class OrderModule
{
public:
    OrderModule();

    // --- Data --------------------------------------------------------------
    bool loadFromCSV(const std::string& fileName);
    bool saveToCSV(const std::string& fileName) const;

    // --- Entry point -------------------------------------------------------
    void run();                                     // the Task 1 sub-menu

    // --- CONTRACT used by KioskSystem --------------------------------------
    bool enqueueOrder(const Order& order);          // step 2: join the queue
    bool peekNextOrder(Order& output) const;        // who is served next
    bool dequeueNextOrder(Order& output);           // step 3: hand to a stall
    bool completeOrder(int orderID);                // step 7: mark fulfilled
    int  pendingCount() const;
    int  completedCount() const;
    void displayPendingOrders() const;

private:
    // TODO (Task 1 owner): declare your own queue here.
    //   Create OrderQueue.hpp / OrderQueue.cpp for the data structure itself
    //   and keep this file for the screens and the CSV file handling, the same
    //   way Task 4 splits MenuBST from MenuModule.
    //
    //   Functional requirements from the brief:
    //     - accept and record new student orders
    //     - maintain an ordered list of all incoming requests
    //     - process orders in arrival order
    //     - remove an order once a stall has been assigned to it
    //     - display pending and completed orders
    //     - handle an empty queue and a full queue without crashing
    int placeholderOrderCount;
};

#endif // ORDER_MODULE_HPP
