// ============================================================================
//  File        : KioskSystem.hpp
//  Project     : APU Campus Food Court Self-Order Kiosk System (CT077-3-2-DSTR)
//  Owner       : Team leader  -  SHARED FILE
//  Description : The integration layer. It owns one instance of every module,
//                shows the main menu, and runs the end-to-end workflow that
//                makes the five modules behave as ONE system.
//
//  >>> RULES FOR THIS FILE <<<
//  1. This file is the ONLY place where one module is allowed to call another.
//     Inside your own module you never include another member's header.
//  2. Adding a call to your module here needs a one-line message to the team,
//     because everyone rebuilds from this file.
// ============================================================================

#ifndef KIOSK_SYSTEM_HPP
#define KIOSK_SYSTEM_HPP

#include "Common.hpp"
#include "OrderModule.hpp"     // Task 1
#include "StallModule.hpp"     // Task 2
#include "SessionModule.hpp"   // Task 3
#include "MenuModule.hpp"      // Task 4
#include "LayoutModule.hpp"    // Task 5 (optional)

class KioskSystem
{
public:
    KioskSystem();

    // Loads every data file. Missing files are reported but do not stop the
    // kiosk, so the team can keep working while a data file is unfinished.
    void loadAllData();

    // Shows the main menu until the operator exits.
    void run();

    // Jumps straight into one module, used for the individual 5-minute
    // recordings:   kiosk.exe 4
    void runSingleModule(int taskNumber);

private:
    OrderModule   orderModule;      // Task 1
    StallModule   stallModule;      // Task 2
    SessionModule sessionModule;    // Task 3
    MenuModule    menuModule;       // Task 4
    LayoutModule  layoutModule;     // Task 5

    int nextOrderID;                // running number given to each new order

    void displayMainMenu() const;

    // ------------------------------------------------------------------
    //  THE END-TO-END WORKFLOW (the 7 steps in the assignment brief)
    //    1. student scans their ID          -> Task 3 starts a session
    //    2. an order is created and queued  -> Task 4 look-up, Task 1 queue
    //    3. a stall is assigned by rotation -> Task 2
    //    4. the item is located             -> Task 4
    //    5. session steps are recorded      -> Task 3
    //    6. the student can navigate back   -> Task 3
    //    7. the order is fulfilled          -> Task 1
    // ------------------------------------------------------------------
    void workflowPlaceOrder();      // steps 1 - 3
    void workflowServeNextOrder();  // steps 3 and 7
    void workflowSystemStatus();    // one screen showing all four modules
};

#endif // KIOSK_SYSTEM_HPP
