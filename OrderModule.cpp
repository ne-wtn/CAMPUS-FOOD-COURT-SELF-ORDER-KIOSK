// ============================================================================
//  File        : OrderModule.cpp
//  Module      : TASK 1 - Order Queue Management Module
//  Owner       : <member 1 - put your name and TP number here>
//  Description : Screens and CSV persistence for the order queue. The queue
//                logic itself lives in OrderQueue.hpp/.cpp - this file never
//                touches the array/front/rear directly, only through
//                OrderQueue's public methods.
// ============================================================================
#include "OrderModule.hpp"

#include <iostream>
#include <iomanip>

OrderModule::OrderModule()
    : completedOrderCount(0), hasCurrentlyServing(false), nextTestOrderID(9000)
{
}

// ----------------------------------------------------------------------------
//  CSV FORMAT (orders.csv)
//  One order per line:
//    OrderID,StudentID,Status,AssignedStall,ArrivalSequence,LineCount,
//    <itemID,itemName,unitPrice,quantity> repeated LineCount times
//  Extra comma-separated fields after LineCount are simply read LineCount
//  times, so the line can hold any number of items without a nested format.
// ----------------------------------------------------------------------------

bool OrderModule::loadFromCSV(const std::string& fileName)
{
    std::ifstream file(fileName.c_str());
    if (!file.is_open())
    {
        return false;
    }

    std::string line;
    bool loadedAtLeastOne = false;

    while (std::getline(file, line))
    {
        if (CsvUtil::isHeaderOrBlank(line, "OrderID"))
        {
            continue;
        }

        std::string field;
        int pos = 0;
        bool valid = false;

        pos = CsvUtil::nextField(line, pos, field);
        int orderID = TextUtil::toInteger(field, valid);
        if (!valid)
        {
            continue;   // a corrupted row is skipped, not allowed to crash the load
        }

        pos = CsvUtil::nextField(line, pos, field);
        std::string studentID = field;

        pos = CsvUtil::nextField(line, pos, field);
        int statusValue = TextUtil::toInteger(field, valid);
        OrderStatus status = valid ? static_cast<OrderStatus>(statusValue) : ORDER_PENDING;

        pos = CsvUtil::nextField(line, pos, field);
        std::string assignedStall = field;

        pos = CsvUtil::nextField(line, pos, field);
        int arrivalSequence = TextUtil::toInteger(field, valid);

        pos = CsvUtil::nextField(line, pos, field);
        int lineCount = TextUtil::toInteger(field, valid);
        if (!valid || lineCount < 0)
        {
            lineCount = 0;
        }

        Order order(orderID, studentID, arrivalSequence);
        order.status        = status;
        order.assignedStall = assignedStall;

        for (int i = 0; i < lineCount && pos != -1; i++)
        {
            std::string itemID, itemName, priceText, qtyText;
            pos = CsvUtil::nextField(line, pos, itemID);
            pos = CsvUtil::nextField(line, pos, itemName);
            pos = CsvUtil::nextField(line, pos, priceText);
            pos = CsvUtil::nextField(line, pos, qtyText);

            bool priceValid = false;
            bool qtyValid   = false;
            double price = TextUtil::toDecimal(priceText, priceValid);
            int    qty   = TextUtil::toInteger(qtyText, qtyValid);

            order.addLine(OrderLine(itemID, itemName, price, qty));
        }

        // A saved order that was already completed goes into the history;
        // anything else rejoins the pending queue exactly as it left off.
        if (order.status == ORDER_COMPLETED && completedOrderCount < MAX_COMPLETED_ORDERS)
        {
            completedOrders[completedOrderCount] = order;
            completedOrderCount++;
        }
        else
        {
            pendingQueue.enqueue(order);
        }

        loadedAtLeastOne = true;
    }

    file.close();
    return loadedAtLeastOne;
}

void OrderModule::writeOrderLine(std::ofstream& file, const Order& order) const
{
    file << order.orderID << ","
         << order.studentID << ","
         << static_cast<int>(order.status) << ","
         << order.assignedStall << ","
         << order.arrivalSequence << ","
         << order.lineCount;

    for (int i = 0; i < order.lineCount; i++)
    {
        const OrderLine& line = order.lines[i];
        file << "," << line.itemID
             << "," << line.itemName
             << "," << TextUtil::formatPrice(line.unitPrice)
             << "," << line.quantity;
    }

    file << std::endl;
}

bool OrderModule::saveToCSV(const std::string& fileName) const
{
    std::ofstream file(fileName.c_str());
    if (!file.is_open())
    {
        return false;
    }

    file << "OrderID,StudentID,Status,AssignedStall,ArrivalSequence,LineCount,Items..." << std::endl;

    int pendingSize = pendingQueue.size();
    for (int i = 0; i < pendingSize; i++)
    {
        writeOrderLine(file, pendingQueue.getAt(i));
    }

    for (int i = 0; i < completedOrderCount; i++)
    {
        writeOrderLine(file, completedOrders[i]);
    }

    file.close();
    return true;
}

// ============================================================================
//  CONTRACT METHODS used by KioskSystem
// ============================================================================

// step 2: join the queue. O(1) - see OrderQueue::enqueue.
bool OrderModule::enqueueOrder(const Order& order)
{
    return pendingQueue.enqueue(order);
}

// O(1) - see OrderQueue::peek.
bool OrderModule::peekNextOrder(Order& output) const
{
    return pendingQueue.peek(output);
}

// step 3: hand to a stall. O(1) - see OrderQueue::dequeue.
// The dequeued order is also kept as "currently serving" so that the
// completeOrder() call KioskSystem makes right after assigning a stall
// knows which order it is closing out.
bool OrderModule::dequeueNextOrder(Order& output)
{
    if (!pendingQueue.dequeue(output))
    {
        return false;
    }

    currentlyServing    = output;
    hasCurrentlyServing = true;
    return true;
}

// step 7: mark fulfilled. O(1).
// Only succeeds if it matches the order that was just dequeued - this
// module does not search the whole history for an arbitrary ID, since the
// workflow always calls dequeueNextOrder() immediately before this.
bool OrderModule::completeOrder(int orderID)
{
    if (!hasCurrentlyServing || currentlyServing.orderID != orderID)
    {
        return false;
    }

    currentlyServing.status = ORDER_COMPLETED;

    if (completedOrderCount < MAX_COMPLETED_ORDERS)
    {
        completedOrders[completedOrderCount] = currentlyServing;
        completedOrderCount++;
    }

    hasCurrentlyServing = false;
    return true;
}

int OrderModule::pendingCount() const
{
    return pendingQueue.size();
}

int OrderModule::completedCount() const
{
    return completedOrderCount;
}

void OrderModule::displayOneOrder(const Order& order) const
{
    std::cout << "   Order #" << order.orderID
               << " | Student: " << std::left << std::setw(10) << order.studentID
               << " | Status: "  << std::setw(10) << order.statusText()
               << " | Stall: "   << std::setw(12) << (order.assignedStall.empty() ? "-" : order.assignedStall)
               << " | Total: RM " << TextUtil::formatPrice(order.totalAmount())
               << std::endl;
}

void OrderModule::displayPendingOrders() const
{
    ConsoleUI::printTitle("PENDING ORDERS (Task 1 - Order Queue)");

    int total = pendingQueue.size();
    if (total == 0)
    {
        ConsoleUI::showMessage("No pending orders.");
        return;
    }

    for (int i = 0; i < total; i++)
    {
        displayOneOrder(pendingQueue.getAt(i));
    }
}

// ============================================================================
//  TASK 1 SUB-MENU  (standalone demo for the individual recording)
//  This module never includes another member's header, so orders placed here
//  are entered manually rather than looked up through Task 4's menu database.
//  The real, fully integrated flow is KioskSystem::workflowPlaceOrder().
// ============================================================================
void OrderModule::run()
{
    int choice = -1;

    while (choice != 0)
    {
        ConsoleUI::printTitle("TASK 1 : ORDER QUEUE MANAGEMENT (Queue - FIFO)");
        std::cout << "   1. Place a test order (manual entry)"          << std::endl;
        std::cout << "   2. Serve next order (dequeue - starts processing)" << std::endl;
        std::cout << "   3. Complete current order (mark fulfilled)"    << std::endl;
        std::cout << "   4. Display pending orders"                     << std::endl;
        std::cout << "   5. Display completed order history"           << std::endl;
        std::cout << "   6. Display the order currently being processed" << std::endl;
        std::cout << "   7. Save orders to " << FILE_ORDERS             << std::endl;
        std::cout << "   0. Back to main menu"                          << std::endl;
        ConsoleUI::printLine('-');

        choice = ConsoleUI::readInteger("  Select an option (0-7) : ", 0, 7);

        switch (choice)
        {
            case 1:
            {
                std::string studentID = ConsoleUI::readRequiredText("  Student ID : ");
                std::string itemName  = ConsoleUI::readRequiredText("  Item name  : ");
                double price = ConsoleUI::readDecimal("  Unit price (RM) : ", 0.01, MAX_ITEM_PRICE);
                int    qty   = ConsoleUI::readInteger("  Quantity : ", 1, 20);

                nextTestOrderID++;
                Order order(nextTestOrderID, studentID, nextTestOrderID);
                order.addLine(OrderLine("T" + TextUtil::toText(nextTestOrderID), itemName, price, qty));

                if (enqueueOrder(order))
                {
                    ConsoleUI::showMessage("Order #" + TextUtil::toText(order.orderID) + " queued.");
                }
                else
                {
                    ConsoleUI::showMessage("Queue is full - order rejected (system overload).");
                }
                break;
            }
            case 2:
            {
                Order served;
                if (!dequeueNextOrder(served))
                {
                    ConsoleUI::showMessage("No pending orders. Queue is empty.");
                }
                else
                {
                    std::cout << "  Now processing order #" << served.orderID
                               << " for " << served.studentID
                               << " - use option 3 to mark it fulfilled." << std::endl;
                }
                break;
            }
            case 3:
            {
                if (!hasCurrentlyServing)
                {
                    ConsoleUI::showMessage("No order is currently being processed. Use option 2 first.");
                }
                else
                {
                    int orderID = currentlyServing.orderID;
                    completeOrder(orderID);
                    ConsoleUI::showMessage("Order #" + TextUtil::toText(orderID) + " marked as completed.");
                }
                break;
            }
            case 4:
                displayPendingOrders();
                break;
            case 5:
            {
                ConsoleUI::printTitle("COMPLETED ORDER HISTORY");
                if (completedOrderCount == 0)
                {
                    ConsoleUI::showMessage("No completed orders yet.");
                }
                else
                {
                    for (int i = 0; i < completedOrderCount; i++)
                    {
                        displayOneOrder(completedOrders[i]);
                    }
                }
                break;
            }
            case 6:
            {
                ConsoleUI::printTitle("ORDER CURRENTLY BEING PROCESSED");
                if (!hasCurrentlyServing)
                {
                    ConsoleUI::showMessage("No order is currently being processed.");
                }
                else
                {
                    displayOneOrder(currentlyServing);
                }
                break;
            }
            case 7:
            {
                if (saveToCSV(FILE_ORDERS))
                {
                    ConsoleUI::showMessage("Orders saved.");
                }
                else
                {
                    ConsoleUI::showMessage("Could not save orders.");
                }
                break;
            }
            case 0:
                break;
            default:
                break;
        }

        if (choice != 0)
        {
            ConsoleUI::pause();
        }
    }
}
