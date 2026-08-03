// ============================================================================
//  File        : OrderModule.hpp
//  Module      : TASK 1 - Order Queue Management Module
//  Owner       : <member 1 - put your name and TP number here>
//  Data used   : QUEUE (FIFO) - fair, first-come-first-served processing
//
//  The public methods below are the team contract: KioskSystem.cpp already
//  calls them, and their names/signatures must not change. Everything in
//  the private section is mine to design, split into:
//    - OrderQueue.hpp/.cpp   the data structure itself (no cout in it)
//    - this file             the screens and CSV file handling
//  the same way Task 4 splits MenuBST (data structure) from MenuModule
//  (menus and CSV).
// ============================================================================
#ifndef ORDER_MODULE_HPP
#define ORDER_MODULE_HPP

#include "Common.hpp"
#include "OrderQueue.hpp"
#include <fstream>

class OrderModule
{
public:
    OrderModule();

    // --- Data ----------------------------------------------------------
    bool loadFromCSV(const std::string& fileName);
    bool saveToCSV(const std::string& fileName) const;

    // --- Entry point -----------------------------------------------------
    void run();                                     // the Task 1 sub-menu

    // --- CONTRACT used by KioskSystem -------------------------------------
    bool enqueueOrder(const Order& order);          // step 2: join the queue
    bool peekNextOrder(Order& output) const;        // who is served next
    bool dequeueNextOrder(Order& output);           // step 3: hand to a stall
    bool completeOrder(int orderID);                // step 7: mark fulfilled
    int  pendingCount() const;
    int  completedCount() const;
    void displayPendingOrders() const;

private:
    static const int MAX_COMPLETED_ORDERS = 100;

    OrderQueue pendingQueue;                        // Task 1 data structure

    // A plain history list. Not itself a required data structure - just an
    // append-only record of what has already left the queue, so the module
    // can show "Completed order history" per the brief.
    Order completedOrders[MAX_COMPLETED_ORDERS];
    int   completedOrderCount;

    // The order most recently taken off the front of the queue. Held here
    // because dequeueNextOrder() and completeOrder() are two separate calls
    // in the workflow (KioskSystem assigns a stall in between them), so the
    // module needs to remember which order is "in progress" across the gap.
    Order currentlyServing;
    bool  hasCurrentlyServing;

    int nextTestOrderID;   // only used by the run() sub-menu's manual test orders

    // --- internal helpers ---------------------------------------------
    void displayOneOrder(const Order& order) const;
    void writeOrderLine(std::ofstream& file, const Order& order) const;
};

#endif // ORDER_MODULE_HPP
