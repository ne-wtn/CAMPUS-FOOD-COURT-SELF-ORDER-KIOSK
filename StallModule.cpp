// ============================================================================
//  File        : StallModule.cpp
//  Module      : TASK 2 - Stall Assignment Module
//  Owner       : <your name / TP number - taken on in place of the absent member>
//
//  A CIRCULAR QUEUE, implemented as a fixed-size array (stalls) plus a
//  rotation pointer (current). No STL containers are used - this is a
//  hand-written array-based circular queue, exactly like the brief asks.
// ============================================================================

#include "StallModule.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>

namespace
{
    const int MENU_VIEW_STATUS   = 1;
    const int MENU_ASSIGN_DEMO   = 2;
    const int MENU_RELEASE_STALL = 3;
    const int MENU_OPEN_STALL    = 4;
    const int MENU_CLOSE_STALL   = 5;
    const int MENU_EXIT          = 0;
}

// A freshly constructed record is closed and empty until it is filled in
// by loadDefaultStalls() or loadFromCSV().
StallModule::StallRecord::StallRecord()
    : name(""), capacity(0), isOpen(false), currentLoad(0), totalServed(0)
{
}

// Starts with the built-in default stalls already loaded, so the module
// always has something to demonstrate even before loadFromCSV() runs, or
// if "stalls.csv" is ever missing.
// This is O(1) here; the real work happens in loadDefaultStalls(), which is
// O(n) in the fixed number of built-in stalls.
StallModule::StallModule()
    : total(0), current(0)
{
    loadDefaultStalls();
}

// Resets the circular queue to a small built-in set of stalls, using the
// same stall names already found in menu_items.csv so the two data files
// agree with each other.
// This is O(n) in the number of default stalls, all fixed and small.
void StallModule::loadDefaultStalls()
{
    total   = 0;
    current = 0;

    const char* defaultNames[]      = { "Selera Malaya", "Wok Master", "Western Corner",
                                         "Spice Route",   "Drinks Bar", "Sweet Treats" };
    const int   defaultCapacities[] = { 3, 3, 3, 3, 4, 2 };
    const int   defaultTotal        = 6;

    for (int i = 0; i < defaultTotal && total < MAX_STALLS; i++)
    {
        stalls[total].name        = defaultNames[i];
        stalls[total].capacity    = defaultCapacities[i];
        stalls[total].isOpen      = true;
        stalls[total].currentLoad = 0;
        stalls[total].totalServed = 0;
        total++;
    }
}

// Loads the stall list from a CSV file shaped "StallName,Capacity,Status".
// Replaces whatever stalls were loaded before (including the defaults) and
// resets the rotation pointer, exactly like MenuModule::loadFromCSV does
// for Task 4. Returns false only when the file itself cannot be opened -
// a badly formed row is rejected and counted, not treated as a fatal error.
// This is O(n) in the number of lines in the file.
bool StallModule::loadFromCSV(const std::string& fileName)
{
    std::ifstream inputFile(fileName.c_str());

    if (!inputFile.is_open())
    {
        return false;
    }

    total   = 0;
    current = 0;
    int rejected = 0;

    std::string line;
    while (std::getline(inputFile, line))
    {
        // Skips blank lines and the "StallName,Capacity,Status" header row.
        if (CsvUtil::isHeaderOrBlank(line, "StallName"))
        {
            continue;
        }

        std::string name, capacityText, statusText;

        int position = CsvUtil::nextField(line, 0, name);
        position = CsvUtil::nextField(line, position, capacityText);
        CsvUtil::nextField(line, position, statusText);

        bool capacityIsValid = false;
        int  capacity = TextUtil::toInteger(capacityText, capacityIsValid);

        if (name.empty() || !capacityIsValid || capacity <= 0 || total >= MAX_STALLS)
        {
            rejected++;
            continue;
        }

        stalls[total].name        = name;
        stalls[total].capacity    = capacity;
        stalls[total].isOpen      = !TextUtil::equalsIgnoreCase(statusText, "Closed");
        stalls[total].currentLoad = 0;
        stalls[total].totalServed = 0;
        total++;
    }

    inputFile.close();

    std::cout << "  " << total << " stall(s) loaded from \"" << fileName << "\"." << std::endl;
    if (rejected > 0)
    {
        std::cout << "  " << rejected
                  << " row(s) rejected (missing name, bad capacity, or too many stalls)."
                  << std::endl;
    }

    return true;
}

// Writes the current stall list back out in the same "StallName,Capacity,
// Status" shape it is read in, so the file can be reloaded later.
// Deliberately does NOT save currentLoad or the rotation pointer - a saved
// file describes the stalls themselves, not one moment of a live queue.
// This is O(n) in the number of registered stalls.
bool StallModule::saveToCSV(const std::string& fileName) const
{
    std::ofstream outputFile(fileName.c_str());

    if (!outputFile.is_open())
    {
        return false;
    }

    outputFile << "StallName,Capacity,Status" << std::endl;

    for (int i = 0; i < total; i++)
    {
        outputFile << stalls[i].name << "," << stalls[i].capacity << ","
                   << (stalls[i].isOpen ? "Open" : "Closed") << std::endl;
    }

    outputFile.close();
    return true;
}

// Shows the Task 2 sub-menu so the circular queue can be demonstrated on
// its own, without needing Task 1's order queue to be finished first.
void StallModule::run()
{
    int choice = -1;

    while (choice != MENU_EXIT)
    {
        ConsoleUI::printTitle("TASK 2 : STALL ASSIGNMENT (CIRCULAR QUEUE)");

        std::cout << "  Stalls available : " << availableStallCount()
                  << " of " << stallCount() << std::endl;

        ConsoleUI::printLine('-');
        std::cout << "  1. View stall status overview" << std::endl;
        std::cout << "  2. Assign a demo order to the next available stall" << std::endl;
        std::cout << "  3. Release a stall (order collected)" << std::endl;
        std::cout << "  4. Open a stall" << std::endl;
        std::cout << "  5. Close a stall" << std::endl;
        std::cout << "  0. Return to the main menu" << std::endl;
        ConsoleUI::printLine('-');

        choice = ConsoleUI::readInteger("  Select an option (0-5) : ", 0, 5);

        if (choice == MENU_VIEW_STATUS)
        {
            displayStallStatus();
            ConsoleUI::pause();
        }
        else if (choice == MENU_ASSIGN_DEMO)
        {
            Order demoOrder(0, "DEMO", 0);

            if (assignNextStall(demoOrder))
            {
                ConsoleUI::showMessage("Order assigned to: " + demoOrder.assignedStall);
            }
            else
            {
                ConsoleUI::showMessage("Every stall is closed or at capacity right now.");
            }
            ConsoleUI::pause();
        }
        else if (choice == MENU_RELEASE_STALL)
        {
            std::string name = ConsoleUI::readRequiredText("  Stall name to release : ");

            if (releaseStall(name))
            {
                ConsoleUI::showMessage("Released one slot at " + name + ".");
            }
            else
            {
                ConsoleUI::showMessage("That stall was not found, or already has no active orders.");
            }
            ConsoleUI::pause();
        }
        else if (choice == MENU_OPEN_STALL || choice == MENU_CLOSE_STALL)
        {
            std::string name  = ConsoleUI::readRequiredText("  Stall name : ");
            int         index = findStallByName(name);

            if (index == -1)
            {
                ConsoleUI::showMessage("That stall was not found.");
            }
            else
            {
                stalls[index].isOpen = (choice == MENU_OPEN_STALL);
                ConsoleUI::showMessage(name + " is now "
                                       + (stalls[index].isOpen ? "OPEN." : "CLOSED."));
            }
            ConsoleUI::pause();
        }
    }
}

// Walks the circular queue starting at "current", checking at most one
// full lap. The first stall that is open and under capacity gets the
// order; "current" only moves past a stall once it has actually been used,
// so the rotation is fair and never restarts on its own.
// This is O(n) worst case (every stall closed or full), O(1) best case.
bool StallModule::assignNextStall(Order& order)
{
    if (total <= 0)
    {
        return false;
    }

    for (int step = 0; step < total; step++)
    {
        int index = (current + step) % total;

        if (stalls[index].isOpen && stalls[index].currentLoad < stalls[index].capacity)
        {
            stalls[index].currentLoad++;
            stalls[index].totalServed++;

            order.assignedStall = stalls[index].name;
            order.status        = ORDER_ASSIGNED;

            current = (index + 1) % total;  // next call resumes right after this stall
            return true;
        }
    }

    return false;  // every stall is closed or at capacity - nothing to assign
}

// Frees one slot at the named stall once its order has been collected.
// This is O(n) to find the stall by name, O(1) to update it.
bool StallModule::releaseStall(const std::string& stallName)
{
    int index = findStallByName(stallName);

    if (index == -1 || stalls[index].currentLoad <= 0)
    {
        return false;
    }

    stalls[index].currentLoad--;
    return true;
}

// Prints every stall's open/closed state, current load vs. capacity, and
// how many orders it has served in total, with the next stall in the
// rotation clearly marked.
// This is O(n) because every stall must be visited once.
void StallModule::displayStallStatus() const
{
    ConsoleUI::printLine('-');
    std::cout << "  STALL STATUS OVERVIEW" << std::endl;
    ConsoleUI::printLine('-');

    if (total == 0)
    {
        ConsoleUI::showMessage("No stalls are registered yet.");
        return;
    }

    for (int i = 0; i < total; i++)
    {
        std::string state = !stalls[i].isOpen
                                 ? "CLOSED"
                                 : (stalls[i].currentLoad >= stalls[i].capacity ? "FULL" : "OPEN");

        std::cout << "  " << std::left << std::setw(18) << stalls[i].name
                  << std::setw(8) << state
                  << "load " << stalls[i].currentLoad << "/" << stalls[i].capacity
                  << "   served " << stalls[i].totalServed;

        if (i == current)
        {
            std::cout << "   <-- next in rotation";
        }
        std::cout << std::endl;
    }
}

// Counts stalls that are open AND have a free slot right now - the
// real-time figure, not just how many stalls exist.
// This is O(n) because every stall must be checked.
int StallModule::availableStallCount() const
{
    int count = 0;

    for (int i = 0; i < total; i++)
    {
        if (stalls[i].isOpen && stalls[i].currentLoad < stalls[i].capacity)
        {
            count++;
        }
    }
    return count;
}

// Returns how many stalls are registered in total, open or not.
// This is O(1) because the count is stored directly.
int StallModule::stallCount() const
{
    return total;
}

// Linear search for a stall by name (case-insensitive, so "wok master"
// matches "Wok Master"). Returns -1 when nothing matches.
// This is O(n) because the stalls are held in a plain array, not indexed.
int StallModule::findStallByName(const std::string& name) const
{
    for (int i = 0; i < total; i++)
    {
        if (TextUtil::equalsIgnoreCase(stalls[i].name, name))
        {
            return i;
        }
    }
    return -1;
}
