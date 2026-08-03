#ifndef STALL_MODULE_HPP
#define STALL_MODULE_HPP

#include "Common.hpp"

class StallModule
{
public:
    StallModule();

    
    static const int MAX_STALLS = 20;

    // Data
    bool loadFromCSV(const std::string& fileName);
    bool saveToCSV(const std::string& fileName) const;
    void loadDefaultStalls();

    
    void run();                                     // the Task 2 sub-menu

   
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
        int         capacity;      
        bool        isOpen;        
        int         currentLoad;   
        int         totalServed;   
        StallRecord();
    };

    StallRecord stalls[MAX_STALLS];
    int         total;     // how many stalls are actually registered
    int         current;   // the circular queue's rotation pointer

    // Returns the array index of the stall with this name, or -1.
    int findStallByName(const std::string& name) const;
};

#endif 
