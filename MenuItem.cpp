// ============================================================================
//  File        : MenuItem.cpp
//  Module      : TASK 4 - Menu Item Search and Management Module
//  Description : Implementation of the MenuItem record and of the
//                self-created MenuItemList dynamic array.
//
//  The string helpers this module uses (toLower, trim, comparePrefix) live in
//  the shared Common.hpp, because all five modules need them.
// ============================================================================

#include "MenuItem.hpp"

// ============================================================================
//  MenuItem
// ============================================================================

// Default record - used when a variable must exist before it is filled in.
MenuItem::MenuItem()
    : itemID(""), name(""), stallName(""), category(""),
      price(0.0), available(true)
{
}

// Full record - the constructor used everywhere else in the module.
MenuItem::MenuItem(const std::string& id,
                   const std::string& itemName,
                   const std::string& stall,
                   const std::string& itemCategory,
                   double             itemPrice,
                   bool               isAvailable)
    : itemID(id), name(itemName), stallName(stall), category(itemCategory),
      price(itemPrice), available(isAvailable)
{
}

// ============================================================================
//  MenuItemList - self-created dynamic array
// ============================================================================

MenuItemList::MenuItemList(int initialCapacity)
    : items(0), count(0), capacity(initialCapacity)
{
    if (capacity < 1)
    {
        capacity = 1;           // never allocate a zero-sized array
    }
    items = new MenuItem[capacity];
}

// Copy constructor - a deep copy is required because 'items' is a raw pointer.
// Without it, two lists would share one array and both would try to delete it.
MenuItemList::MenuItemList(const MenuItemList& other)
    : items(0), count(other.count), capacity(other.capacity)
{
    items = new MenuItem[capacity];
    for (int i = 0; i < count; i++)
    {
        items[i] = other.items[i];
    }
}

// Copy assignment - same reasoning as the copy constructor.
MenuItemList& MenuItemList::operator=(const MenuItemList& other)
{
    if (this == &other)
    {
        return *this;           // guard against "list = list"
    }

    delete[] items;             // release the old array first

    count    = other.count;
    capacity = other.capacity;
    items    = new MenuItem[capacity];

    for (int i = 0; i < count; i++)
    {
        items[i] = other.items[i];
    }
    return *this;
}

MenuItemList::~MenuItemList()
{
    delete[] items;
    items = 0;
}

// Doubles the capacity, copies the existing elements across, frees the old
// array. Doubling (instead of adding one slot) keeps insertion amortised O(1).
void MenuItemList::grow()
{
    int        newCapacity = capacity * 2;
    MenuItem*  newItems    = new MenuItem[newCapacity];

    for (int i = 0; i < count; i++)
    {
        newItems[i] = items[i];
    }

    delete[] items;
    items    = newItems;
    capacity = newCapacity;
}

// Appends one item to the end of the list.
void MenuItemList::add(const MenuItem& item)
{
    if (count == capacity)
    {
        grow();
    }
    items[count] = item;
    count++;
}

// Read-only access by position. An out-of-range index returns a shared empty
// record instead of crashing the kiosk - defensive programming for the demo.
const MenuItem& MenuItemList::get(int index) const
{
    static const MenuItem emptyItem;

    if (index < 0 || index >= count)
    {
        return emptyItem;
    }
    return items[index];
}

int MenuItemList::size() const
{
    return count;
}

bool MenuItemList::isEmpty() const
{
    return count == 0;
}

// Logically empties the list. The allocated array is kept so that a list which
// is reused for many searches does not keep re-allocating memory.
void MenuItemList::clear()
{
    count = 0;
}
