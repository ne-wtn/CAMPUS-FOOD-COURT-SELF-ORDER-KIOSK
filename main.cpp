// ============================================================================
//  File        : main.cpp
//  Project     : APU Campus Food Court Self-Order Kiosk System
//  Subject     : CT077-3-2-DSTR  -  Lab Evaluation Work #2
//  Group       : <group number>
//  Members     : <name / TP number>  - Task 1  Order Queue Management  (Queue)
//                <name / TP number>  - Task 2  Stall Assignment        (Circular Queue)
//                <name / TP number>  - Task 3  Session History         (Stack)
//                <name / TP number>  - Task 4  Menu Search             (BST)
//                <name / TP number>  - Task 5  Food Court Layout       (Tree, optional)
//
//  Owner       : Team leader  -  SHARED FILE
//  Description : The one and only entry point of the system. There must never
//                be a second main() anywhere in the project, or the build
//                fails with "multiple definition of main".
//
//  BUILD
//  -----
//      Windows :  build.bat            (or press Ctrl+Shift+B in VS Code)
//      Others  :  make
//      By hand :  g++ -std=c++11 -Wall -Wextra *.cpp -o kiosk
//
//  RUN
//  ---
//      kiosk            -> the full integrated system
//      kiosk 4          -> jumps straight into Task 4, for the individual
//                          5-minute recording
// ============================================================================

#include "KioskSystem.hpp"

#include <iostream>
#include <cstdlib>

int main(int argumentCount, char* argumentValues[])
{
    std::cout << std::endl;
    ConsoleUI::printLine('=');
    std::cout << "   APU CAMPUS FOOD COURT - SELF ORDER KIOSK SYSTEM" << std::endl;
    std::cout << "   CT077-3-2-DSTR  |  Lab Evaluation Work #2"       << std::endl;
    ConsoleUI::printLine('=');
    std::cout << std::endl;

    KioskSystem kiosk;
    kiosk.loadAllData();

    // "kiosk 4" opens one module directly, which keeps each member's
    // demonstration recording short and focused on their own work.
    if (argumentCount > 1)
    {
        int taskNumber = std::atoi(argumentValues[1]);
        std::cout << "  Single-module mode: Task " << taskNumber << std::endl;
        kiosk.runSingleModule(taskNumber);
        return 0;
    }

    kiosk.run();
    return 0;
}
