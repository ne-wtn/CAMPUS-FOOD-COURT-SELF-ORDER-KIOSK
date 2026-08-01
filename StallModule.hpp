// ============================================================================
//  File        : StallModule.hpp
//  Module      : TASK 2 - Stall Assignment Module
//  Owner       : <your name / TP number - taken on in place of the absent member>
//  Suggested   : CIRCULAR QUEUE - cyclic, balanced allocation across stalls
//
//  >>> SKELETON - being implemented in the next stage. <<<
//
//  The PUBLIC METHODS BELOW ARE A TEAM CONTRACT: KioskSystem.cpp already calls
//  them. Do not rename or change a signature without telling the team.
// ============================================================================

#ifndef STALL_MODULE_HPP
#define STALL_MODULE_HPP

#include "Common.hpp"

class StallModule
{
public:
    StallModule();

    // --- Data --------------------------------------------------------------
    bool loadFromCSV(const std::string& fileName);
    bool saveToCSV(const std::string& fileName) const;
    void loadDefaultStalls();

    // --- Entry point -------------------------------------------------------
    void run();                                     // the Task 2 sub-menu

    // --- CONTRACT used by KioskSystem --------------------------------------
    // Rotates to the next usable stall, writes its name into order.assignedStall
    // and sets the order status to ORDER_ASSIGNED. Returns false when every
    // stall is closed or at capacity.
    bool assignNextStall(Order& order);

    // The order has been collected, so the stall has one slot free again.
    bool releaseStall(const std::string& stallName);

    void displayStallStatus() const;
    int  availableStallCount() const;
    int  stallCount() const;

private:
    // TODO: the circular queue of stalls goes here, in its own
    //       StallCircularQueue.hpp / .cpp files.
    //
    //   Functional requirements from the brief:
    //     - keep every stall and its status (available / busy / closed)
    //     - assign incoming orders in a continuous rotation
    //     - skip stalls that are closed or at capacity
    //     - track the assignments made to each stall
    //     - never restart the cycle - it carries on from where it stopped
    int placeholderStallCount;
};

#endif // STALL_MODULE_HPP
