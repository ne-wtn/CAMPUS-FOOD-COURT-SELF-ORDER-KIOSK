// ============================================================================
//  File        : LayoutModule.cpp
//  Module      : TASK 5 (OPTIONAL) - Food Court Layout and Navigation Module
//  Owner       : <member 5, or nobody>
//
//  >>> SKELETON <<<  Set 'implemented' to true once the real tree is written.
// ============================================================================

#include "LayoutModule.hpp"

#include <iostream>

LayoutModule::LayoutModule()
    : implemented(false)
{
}

bool LayoutModule::loadFromCSV(const std::string& fileName)
{
    (void)fileName;
    return false;
}

void LayoutModule::run()
{
    ConsoleUI::printTitle("TASK 5 : FOOD COURT LAYOUT AND NAVIGATION   [ optional module ]");
    ConsoleUI::showMessage("This optional module has not been implemented.");
    ConsoleUI::pause();
}

void LayoutModule::displayLayout() const
{
    ConsoleUI::showMessage("Task 5 (food court layout) is not implemented.");
}

bool LayoutModule::findRoute(const std::string& fromLocation,
                             const std::string& toLocation) const
{
    (void)fromLocation;
    (void)toLocation;
    return false;
}

std::string LayoutModule::locateStall(const std::string& stallName) const
{
    (void)stallName;
    return "";
}

bool LayoutModule::isAvailable() const
{
    return implemented;
}
