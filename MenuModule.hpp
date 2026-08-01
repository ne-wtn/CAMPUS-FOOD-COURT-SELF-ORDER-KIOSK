// ============================================================================
//  File        : MenuModule.hpp
//  Module      : TASK 4 - Menu Item Search and Management Module
//  Owner       : <your name / TP number>
//  Description : The user-facing layer of Task 4. It owns the MenuBST, loads
//                and saves the menu database file, and drives the kiosk's
//                menu-management screens.
//
//  The class is deliberately separated from MenuBST so that the data structure
//  contains only data-structure logic, while all input, validation and screen
//  formatting live here. This keeps each file short, readable and testable.
// ============================================================================

#ifndef MENU_MODULE_HPP
#define MENU_MODULE_HPP

#include "MenuBST.hpp"

class MenuModule
{
public:
    MenuModule();

    // --- Persistence -------------------------------------------------------
    // Loads "ItemID,Name,Stall,Category,Price,Available" records. Returns
    // false if the file cannot be opened, in which case the caller can fall
    // back on loadDefaultItems() so the kiosk still has a menu to show.
    bool loadFromCSV(const std::string& fileName);
    bool saveToCSV(const std::string& fileName) const;
    void loadDefaultItems();

    // --- Entry point -------------------------------------------------------
    void run();                       // shows the Task 4 menu until the user exits

    // Gives the other modules read access to the same menu database, so that
    // the order module can look up an item that a student has selected.
    const MenuBST& getDatabase() const;

private:
    MenuBST     menuDatabase;         // the Binary Search Tree of Task 4
    std::string dataFileName;         // file the menu is loaded from / saved to
    int         recordsRejected;      // duplicate or malformed CSV rows

    // --- Screen output -----------------------------------------------------
    void displayMenuOptions() const;
    void displayTableHeader() const;
    void displayItemRow(int rowNumber, const MenuItem& item) const;
    void displayList(const MenuItemList& list, const std::string& title) const;
    void displaySingleItem(const MenuItem& item) const;
    void reportSearchCost(const std::string& description) const;

    // --- Menu actions ------------------------------------------------------
    void actionDisplaySortedByID();
    void actionDisplaySortedByName();
    void actionSearchByID();
    void actionSearchByName();
    void actionSearchByPrefix();
    void actionSearchByCategory();
    void actionSearchByStall();
    void actionSearchByPriceRange();
    void actionAddItem();
    void actionUpdateItem();
    void actionChangeAvailability();
    void actionRemoveItem();
    void actionSaveToFile();
    void actionTreeStatistics();

    // All keyboard input goes through the shared ConsoleUI helpers in
    // Common.hpp, so every module of the system validates input the same way.
};

#endif // MENU_MODULE_HPP
