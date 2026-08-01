// ============================================================================
//  File        : LayoutModule.hpp
//  Module      : TASK 5 (OPTIONAL) - Food Court Layout and Navigation Module
//  Owner       : <member 5, or leave unimplemented if the team has 4 members>
//  Suggested   : GENERAL TREE - zones -> stalls -> counters
//
//  >>> THIS IS A SKELETON. <<<
//  Task 5 is optional. If nobody takes it, leave this file exactly as it is:
//  the system still builds and runs, and the main menu simply reports that the
//  module was not part of the team's scope.
// ============================================================================

#ifndef LAYOUT_MODULE_HPP
#define LAYOUT_MODULE_HPP

#include "Common.hpp"

class LayoutModule
{
public:
    LayoutModule();

    bool loadFromCSV(const std::string& fileName);
    void run();                                     // the Task 5 sub-menu

    // --- CONTRACT used by KioskSystem --------------------------------------
    void displayLayout() const;                     // the tree, drawn as text
    bool findRoute(const std::string& fromLocation,
                   const std::string& toLocation) const;
    std::string locateStall(const std::string& stallName) const;
    bool isAvailable() const;                       // false while unimplemented

private:
    // TODO (Task 5 owner): the layout tree goes here, in its own
    //       LayoutTree.hpp / .cpp files.
    //
    //   Functional requirements from the brief:
    //     - model the food court as zones, stalls and counters
    //     - define the connections between locations
    //     - provide a route from one point to another
    //     - traverse every section
    //     - work together with the session history module
    bool implemented;
};

#endif // LAYOUT_MODULE_HPP
