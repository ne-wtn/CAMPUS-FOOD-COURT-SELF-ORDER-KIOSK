// ============================================================================
//  File        : MenuItem.hpp
//  Module      : TASK 4 - Menu Item Search and Management Module
//  Owner       : <your name / TP number>
//  Description : Declares the MenuItem record and MenuItemList, a self-created
//                dynamic array used to hold search results.
//
//  NOTE ON CONTAINERS
//  ------------------
//  No STL container is used anywhere in this module (no <vector>, <list>,
//  <map>, <set>, <queue>, <stack>, ...). Every collection is built by hand
//  from raw arrays and pointers, as required by the brief.
// ============================================================================

#ifndef MENU_ITEM_HPP
#define MENU_ITEM_HPP

#include "Common.hpp"       // TextUtil, CsvUtil, ConsoleUI - shared by the team

#include <string>

// ----------------------------------------------------------------------------
//  MenuItem : one record in the food court menu database.
//  itemID is the primary key - it is unique and never repeated.
// ----------------------------------------------------------------------------
struct MenuItem
{
    std::string itemID;     // Unique key, e.g. "M001"
    std::string name;       // Item name, e.g. "Nasi Lemak Ayam"
    std::string stallName;  // Stall that sells the item, e.g. "Selera Malaya"
    std::string category;   // Cuisine category, e.g. "Malay"
    double      price;      // Selling price in RM
    bool        available;  // true = on sale, false = sold out

    MenuItem();
    MenuItem(const std::string& id,
             const std::string& itemName,
             const std::string& stall,
             const std::string& itemCategory,
             double             itemPrice,
             bool               isAvailable = true);
};

// ----------------------------------------------------------------------------
//  MenuItemList : self-created dynamic array (the replacement for std::vector).
//
//  Why an array and not a linked list?
//  Search results are written once and then read many times while printing,
//  so contiguous storage with O(1) random access is the better fit.
//  The array doubles its capacity when full, giving amortised O(1) insertion.
// ----------------------------------------------------------------------------
class MenuItemList
{
public:
    explicit MenuItemList(int initialCapacity = 8);
    MenuItemList(const MenuItemList& other);              // deep copy
    MenuItemList& operator=(const MenuItemList& other);   // deep copy
    ~MenuItemList();

    void            add(const MenuItem& item);   // append to the end
    const MenuItem& get(int index) const;        // read-only access by position
    int             size() const;
    bool            isEmpty() const;
    void            clear();                     // logically empty the list

private:
    MenuItem* items;      // heap array holding the elements
    int       count;      // number of elements currently stored
    int       capacity;   // number of slots currently allocated

    void grow();          // doubles the capacity and copies the old elements
};

#endif // MENU_ITEM_HPP
