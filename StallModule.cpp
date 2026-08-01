// ============================================================================
//  File        : StallModule.cpp
//  Module      : TASK 2 - Stall Assignment Module
//  Owner       : <your name / TP number>
//
//  >>> SKELETON - to be replaced by the real Circular Queue implementation. <<<
// ============================================================================

#include "StallModule.hpp"

#include <iostream>

StallModule::StallModule()
    : placeholderStallCount(0)
{
}

bool StallModule::loadFromCSV(const std::string& fileName)
{
    (void)fileName;
    return false;
}

bool StallModule::saveToCSV(const std::string& fileName) const
{
    (void)fileName;
    return false;
}

void StallModule::loadDefaultStalls()
{
    placeholderStallCount = 0;
}

void StallModule::run()
{
    ConsoleUI::printTitle("TASK 2 : STALL ASSIGNMENT   [ not implemented yet ]");
    ConsoleUI::showMessage("This module is still being written.");
    ConsoleUI::pause();
}

bool StallModule::assignNextStall(Order& order)
{
    (void)order;
    return false;
}

bool StallModule::releaseStall(const std::string& stallName)
{
    (void)stallName;
    return false;
}

void StallModule::displayStallStatus() const
{
    ConsoleUI::showMessage("Task 2 (stall assignment) is not implemented yet.");
}

int StallModule::availableStallCount() const
{
    return 0;
}

int StallModule::stallCount() const
{
    return 0;
}
