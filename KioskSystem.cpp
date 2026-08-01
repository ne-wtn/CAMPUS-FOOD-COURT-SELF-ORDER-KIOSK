// ============================================================================
//  File        : KioskSystem.cpp
//  Project     : APU Campus Food Court Self-Order Kiosk System (CT077-3-2-DSTR)
//  Owner       : Team leader  -  SHARED FILE
//  Description : Implementation of the main menu and of the end-to-end
//                workflow that joins the five modules into one system.
//
//  While a module is still a skeleton its contract methods return false. The
//  workflow below reports that clearly instead of pretending it worked, so the
//  team can always see exactly which step is still waiting for which member.
// ============================================================================

#include "KioskSystem.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>

KioskSystem::KioskSystem()
    : nextOrderID(1001)
{
}

// ----------------------------------------------------------------------------
//  Data loading. A missing file is reported but never stops the kiosk.
// ----------------------------------------------------------------------------
void KioskSystem::loadAllData()
{
    std::cout << "  Loading system data..." << std::endl;

    if (!menuModule.loadFromCSV(FILE_MENU_ITEMS))
    {
        ConsoleUI::showMessage(std::string(FILE_MENU_ITEMS) + " not found - using the built-in menu.");
        menuModule.loadDefaultItems();
    }

    if (!stallModule.loadFromCSV(FILE_STALLS))
    {
        ConsoleUI::showMessage(std::string(FILE_STALLS) + " not loaded (Task 2 pending).");
    }

    if (!orderModule.loadFromCSV(FILE_ORDERS))
    {
        ConsoleUI::showMessage(std::string(FILE_ORDERS) + " not loaded (Task 1 pending).");
    }

    if (!layoutModule.loadFromCSV(FILE_LAYOUT))
    {
        ConsoleUI::showMessage(std::string(FILE_LAYOUT) + " not loaded (Task 5 optional).");
    }

    std::cout << std::endl;
}

// ============================================================================
//  WORKFLOW 1 - place a new order  (brief steps 1 to 3)
// ============================================================================
void KioskSystem::workflowPlaceOrder()
{
    ConsoleUI::printTitle("PLACE A NEW ORDER");

    // --- Step 1: the student scans their ID -> Task 3 opens a session -------
    std::string studentID = ConsoleUI::readRequiredText("  Scan student ID (e.g. TP012345) : ");
    sessionModule.startSession(studentID);
    sessionModule.recordStep("Login: " + studentID);

    Order order(nextOrderID, studentID, nextOrderID);

    // --- Step 2 and 4: find the items -> Task 4 ----------------------------
    bool stillOrdering = true;
    while (stillOrdering && order.lineCount < MAX_ITEMS_PER_ORDER)
    {
        std::string search = ConsoleUI::readRequiredText(
            "\n  Item ID, or the first letters of a name (or DONE) : ");

        if (TextUtil::equalsIgnoreCase(search, "DONE"))
        {
            stillOrdering = false;
            continue;
        }

        // Try the primary key first: an exact Item ID is an O(log n) search.
        const MenuItem* chosen = menuModule.getDatabase().searchByID(search);

        // Not an ID, so fall back on the name index and offer what matches.
        if (chosen == 0)
        {
            MenuItemList matches;
            menuModule.getDatabase().searchByPrefix(search, matches);

            if (matches.isEmpty())
            {
                ConsoleUI::showMessage("No menu item matches \"" + search + "\".");
                continue;
            }

            std::cout << std::endl;
            for (int i = 0; i < matches.size(); i++)
            {
                std::cout << "   " << (i + 1) << ". "
                          << std::left << std::setw(28) << matches.get(i).name
                          << "RM " << TextUtil::formatPrice(matches.get(i).price)
                          << "   " << matches.get(i).stallName << std::endl;
            }

            int pick = ConsoleUI::readInteger("  Choose a number (0 to cancel) : ",
                                              0, matches.size());
            if (pick == 0)
            {
                continue;
            }
            chosen = menuModule.getDatabase().searchByID(matches.get(pick - 1).itemID);
        }

        if (chosen == 0)
        {
            continue;
        }
        if (!chosen->available)
        {
            ConsoleUI::showMessage("\"" + chosen->name + "\" is sold out today.");
            continue;
        }

        int quantity = ConsoleUI::readInteger("  Quantity (1-20) : ", 1, 20);

        if (!order.addLine(OrderLine(chosen->itemID, chosen->name,
                                     chosen->price, quantity)))
        {
            ConsoleUI::showMessage("This order is full.");
            stillOrdering = false;
            continue;
        }

        // --- Step 5: the session records every action -> Task 3 ------------
        std::ostringstream step;
        step << "Added " << quantity << " x " << chosen->name;
        sessionModule.recordStep(step.str());

        std::cout << "  Added: " << quantity << " x " << chosen->name
                  << "  (RM " << TextUtil::formatPrice(chosen->price * quantity)
                  << ")" << std::endl;
    }

    if (order.lineCount == 0)
    {
        ConsoleUI::showMessage("No items were selected, so no order was created.");
        sessionModule.endSession();
        return;
    }

    // --- The receipt -------------------------------------------------------
    std::cout << std::endl;
    ConsoleUI::printLine('-');
    std::cout << "  ORDER #" << order.orderID << "   Student: " << order.studentID
              << std::endl;
    ConsoleUI::printLine('-');
    for (int i = 0; i < order.lineCount; i++)
    {
        const OrderLine& line = order.lines[i];
        std::cout << "   " << std::left << std::setw(6) << line.itemID
                  << std::setw(30) << line.itemName
                  << std::right << std::setw(3) << line.quantity << " x RM "
                  << std::setw(7) << TextUtil::formatPrice(line.unitPrice)
                  << "  =  RM " << TextUtil::formatPrice(line.lineTotal())
                  << std::endl;
    }
    ConsoleUI::printLine('-');
    std::cout << "  TOTAL : RM " << TextUtil::formatPrice(order.totalAmount())
              << std::endl;
    ConsoleUI::printLine('-');

    if (!ConsoleUI::readYesNo("  Confirm this order? (Y/N) : "))
    {
        ConsoleUI::showMessage("Order cancelled.");
        sessionModule.recordStep("Order cancelled");
        return;
    }

    // --- Step 2: the order joins the queue -> Task 1 -----------------------
    if (orderModule.enqueueOrder(order))
    {
        ConsoleUI::showMessage("Order queued. Position in queue: "
                               + TextUtil::toText(orderModule.pendingCount()));
        nextOrderID++;
    }
    else
    {
        ConsoleUI::showMessage("[Task 1 pending] The order queue is not implemented yet, "
                               "so this order was not stored.");
        nextOrderID++;
    }

    // --- Step 3: a stall is assigned by rotation -> Task 2 -----------------
    if (stallModule.assignNextStall(order))
    {
        ConsoleUI::showMessage("Assigned to stall: " + order.assignedStall);
    }
    else
    {
        ConsoleUI::showMessage("[Task 2 pending] No stall could be assigned yet.");
    }

    sessionModule.recordStep("Order #" + TextUtil::toText(order.orderID) + " confirmed");
}

// ============================================================================
//  WORKFLOW 2 - serve the next order  (brief steps 3 and 7)
// ============================================================================
void KioskSystem::workflowServeNextOrder()
{
    ConsoleUI::printTitle("SERVE THE NEXT ORDER");

    Order next;
    if (!orderModule.dequeueNextOrder(next))
    {
        ConsoleUI::showMessage("[Task 1 pending] There is no order queue to serve from yet.");
        return;
    }

    std::cout << "  Now serving order #" << next.orderID
              << " for " << next.studentID << std::endl;

    if (stallModule.assignNextStall(next))
    {
        std::cout << "  Prepared by : " << next.assignedStall << std::endl;
    }

    if (orderModule.completeOrder(next.orderID))
    {
        ConsoleUI::showMessage("Order marked as completed and removed from the active queue.");
        stallModule.releaseStall(next.assignedStall);
    }
}

// ============================================================================
//  WORKFLOW 3 - one screen showing the state of the whole system
// ============================================================================
void KioskSystem::workflowSystemStatus()
{
    ConsoleUI::printTitle("SYSTEM STATUS OVERVIEW");

    std::cout << "  Task 1  Order queue      : " << orderModule.pendingCount()
              << " pending, " << orderModule.completedCount() << " completed"
              << std::endl;
    std::cout << "  Task 2  Stalls           : " << stallModule.availableStallCount()
              << " available of " << stallModule.stallCount() << std::endl;
    std::cout << "  Task 3  Active session   : "
              << (sessionModule.hasActiveSession()
                      ? sessionModule.currentStudentID() + " ("
                        + TextUtil::toText(sessionModule.stepCount()) + " steps)"
                      : std::string("none"))
              << std::endl;
    std::cout << "  Task 4  Menu items       : "
              << menuModule.getDatabase().size() << " stored, tree height "
              << menuModule.getDatabase().height() << std::endl;
    std::cout << "  Task 5  Layout           : "
              << (layoutModule.isAvailable() ? "loaded" : "not implemented")
              << std::endl;

    ConsoleUI::printLine('-');
    orderModule.displayPendingOrders();
    stallModule.displayStallStatus();
}

// ============================================================================
//  MAIN MENU
// ============================================================================

void KioskSystem::displayMainMenu() const
{
    std::cout << std::endl;
    ConsoleUI::printLine('=');
    std::cout << "   APU CAMPUS FOOD COURT - SELF ORDER KIOSK SYSTEM" << std::endl;
    ConsoleUI::printLine('=');
    std::cout << "   KIOSK OPERATIONS                    INDIVIDUAL MODULES"      << std::endl;
    std::cout << "    1. Place a new order                4. Task 1 - Order Queue      (Queue)"     << std::endl;
    std::cout << "    2. Serve the next order             5. Task 2 - Stall Assignment (Circular Q)" << std::endl;
    std::cout << "    3. System status overview           6. Task 3 - Session History  (Stack)"     << std::endl;
    std::cout << "                                        7. Task 4 - Menu Search      (BST)"       << std::endl;
    std::cout << "    9. Save all data                    8. Task 5 - Food Court Layout (Tree)"     << std::endl;
    std::cout << "    0. Exit"                                                     << std::endl;
    ConsoleUI::printLine('=');
}

void KioskSystem::run()
{
    int choice = -1;

    while (choice != 0)
    {
        displayMainMenu();
        choice = ConsoleUI::readInteger("  Select an option (0-9) : ", 0, 9);

        switch (choice)
        {
            case 1: workflowPlaceOrder();      ConsoleUI::pause(); break;
            case 2: workflowServeNextOrder();  ConsoleUI::pause(); break;
            case 3: workflowSystemStatus();    ConsoleUI::pause(); break;
            case 4: orderModule.run();                             break;
            case 5: stallModule.run();                             break;
            case 6: sessionModule.run();                           break;
            case 7: menuModule.run();                              break;
            case 8: layoutModule.run();                            break;
            case 9:
                menuModule.saveToCSV(FILE_MENU_ITEMS);
                stallModule.saveToCSV(FILE_STALLS);
                orderModule.saveToCSV(FILE_ORDERS);
                ConsoleUI::showMessage("All available data files have been saved.");
                ConsoleUI::pause();
                break;
            case 0:
                std::cout << std::endl << "  Shutting down the kiosk. Thank you."
                          << std::endl;
                break;
            default:
                break;
        }
    }
}

// Used by the individual recordings:  kiosk.exe 4
void KioskSystem::runSingleModule(int taskNumber)
{
    switch (taskNumber)
    {
        case 1: orderModule.run();   break;
        case 2: stallModule.run();   break;
        case 3: sessionModule.run(); break;
        case 4: menuModule.run();    break;
        case 5: layoutModule.run();  break;
        default:
            ConsoleUI::showMessage("Unknown task number. Use 1 to 5.");
            break;
    }
}
