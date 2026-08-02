// ============================================================================
//  File        : StallModule.hpp
//  Module      : TASK 2 - Stall Assignment Module
//  Owner       : <your name / TP number - taken on in place of the absent member>
//  Structure   : CIRCULAR QUEUE - cyclic, balanced allocation across stalls
//
//  The stalls live in a fixed-size array. "current" is the classic circular
//  queue pointer: it always moves forward and wraps back to index 0 with
//  (current + 1) % total, and it is NEVER reset to 0 on its own - so the
//  rotation keeps going from wherever it stopped, exactly as the brief asks
//  ("uninterrupted order allocation without restarting the assignment
//  cycle"). No STL containers are used anywhere below.
//
//  THIS FILE'S PUBLIC METHODS ARE A TEAM CONTRACT: KioskSystem.cpp already
//  calls loadFromCSV, saveToCSV, assignNextStall, releaseStall,
//  displayStallStatus, availableStallCount, stallCount and run(). Their
//  names and signatures below are UNCHANGED from the skeleton.
// ============================================================================

#ifndef STALL_MODULE_HPP
#define STALL_MODULE_HPP

#include "Common.hpp"

class StallModule
{
public:
    StallModule();

    // Fixed upper bound for the stall list.
    static const int MAX_STALLS = 20;

    // --- Data ------------------------------------------------------------
    bool loadFromCSV(const std::string& fileName);
    bool saveToCSV(const std::string& fileName) const;
    void loadDefaultStalls();

    // --- Entry point -------------------------------------------------------
    void run();                                     // the Task 2 sub-menu

    // --- CONTRACT used by KioskSystem (signatures unchanged) ---------------
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
    // One row of the circular queue: a stall and everything needed to
    // decide whether it can currently take another order.
    struct StallRecord
    {
        std::string name;
        int         capacity;      // maximum orders it can hold at once
        bool        isOpen;        // false = closed for the day
        int         currentLoad;   // orders it is holding right now
        int         totalServed;   // running total, for reporting only

        StallRecord();
    };

    StallRecord stalls[MAX_STALLS];
    int         total;     // how many stalls are actually registered
    int         current;   // the circular queue's rotation pointer

    // Returns the array index of the stall with this name, or -1.
    int findStallByName(const std::string& name) const;
};

#endif // STALL_MODULE_HPP
