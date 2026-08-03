//circular queue is implemented here 
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


StallModule::StallRecord::StallRecord()
    : name(""), capacity(0), isOpen(false), currentLoad(0), totalServed(0)
{
}


StallModule::StallModule()
    : total(0), current(0)
{
    loadDefaultStalls();
}


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
        // Skips blank lines and the StallName,Capacity,Status header row.
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

// Counts stalls that are open AND have a free slot right now 
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

int StallModule::stallCount() const
{
    return total;
}

// Linear search for a stall by name (case-insensitive, so "wok master"
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
