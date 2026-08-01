// ============================================================================
//  File        : OrderModule.cpp
//  Module      : TASK 1 - Order Queue Management Module
//  Owner       : <member 1 - put your name and TP number here>
//
//  >>> SKELETON <<<  Every method below is a stub that keeps the project
//  compiling and running for the rest of the team. Replace the bodies with
//  your real queue implementation - do not change the signatures.
// ============================================================================

#include "OrderModule.hpp"

#include <iostream>

OrderModule::OrderModule()
    : placeholderOrderCount(0)
{
}

bool OrderModule::loadFromCSV(const std::string& fileName)
{
    // TODO (Task 1 owner): read the sample orders from the CSV file.
    (void)fileName;
    return false;
}

bool OrderModule::saveToCSV(const std::string& fileName) const
{
    // TODO (Task 1 owner): write the pending and completed orders back out.
    (void)fileName;
    return false;
}

void OrderModule::run()
{
    ConsoleUI::printTitle("TASK 1 : ORDER QUEUE MANAGEMENT   [ not implemented yet ]");
    ConsoleUI::showMessage("This module is still being written by its owner.");
    ConsoleUI::pause();
}

bool OrderModule::enqueueOrder(const Order& order)
{
    // TODO (Task 1 owner): add the order to the back of the queue.
    (void)order;
    placeholderOrderCount++;
    return false;
}

bool OrderModule::peekNextOrder(Order& output) const
{
    // TODO (Task 1 owner): copy the front order into 'output'.
    (void)output;
    return false;
}

bool OrderModule::dequeueNextOrder(Order& output)
{
    // TODO (Task 1 owner): remove the front order and copy it into 'output'.
    (void)output;
    return false;
}

bool OrderModule::completeOrder(int orderID)
{
    // TODO (Task 1 owner): move the order into the completed history.
    (void)orderID;
    return false;
}

int OrderModule::pendingCount() const
{
    return 0;
}

int OrderModule::completedCount() const
{
    return 0;
}

void OrderModule::displayPendingOrders() const
{
    ConsoleUI::showMessage("Task 1 (order queue) is not implemented yet.");
}
