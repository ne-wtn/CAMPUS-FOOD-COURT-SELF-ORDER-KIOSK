// ============================================================================
//  File        : MenuModule.cpp
//  Module      : TASK 4 - Menu Item Search and Management Module
//  Description : Implementation of the Task 4 user interface, the CSV
//                persistence routines and all input validation.
// ============================================================================

#include "MenuModule.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>

// Column widths used by every table printed in this module.
// They add up to CONSOLE_WIDTH so the tables line up with the shared
// separators drawn by ConsoleUI::printLine().
static const int WIDTH_NUMBER    = 5;
static const int WIDTH_ID        = 8;
static const int WIDTH_NAME      = 26;
static const int WIDTH_STALL     = 20;
static const int WIDTH_CATEGORY  = 12;
static const int WIDTH_PRICE     = 10;
static const int WIDTH_STATUS    = 11;

// ============================================================================
//  Construction
// ============================================================================

MenuModule::MenuModule()
    : menuDatabase(), dataFileName(FILE_MENU_ITEMS), recordsRejected(0)
{
}

const MenuBST& MenuModule::getDatabase() const
{
    return menuDatabase;
}

// ============================================================================
//  CSV PERSISTENCE
// ============================================================================

// Loads the menu database from a CSV file.
// Layout: ItemID,Name,Stall,Category,Price,Available   (1 = on sale, 0 = sold out)
// The first line is a header and is skipped.
bool MenuModule::loadFromCSV(const std::string& fileName)
{
    std::ifstream inputFile(fileName.c_str());

    if (!inputFile.is_open())
    {
        return false;
    }

    menuDatabase.clear();
    recordsRejected = 0;

    std::string line;
    int         loaded = 0;

    while (std::getline(inputFile, line))
    {
        // Skips blank lines and the "ItemID,Name,..." column-header row.
        if (CsvUtil::isHeaderOrBlank(line, "ItemID"))
        {
            continue;
        }

        std::string id, name, stall, category, priceText, availableText;

        int position = CsvUtil::nextField(line, 0, id);
        position = CsvUtil::nextField(line, position, name);
        position = CsvUtil::nextField(line, position, stall);
        position = CsvUtil::nextField(line, position, category);
        position = CsvUtil::nextField(line, position, priceText);
        CsvUtil::nextField(line, position, availableText);

        bool   priceIsValid = false;
        double price        = TextUtil::toDecimal(priceText, priceIsValid);

        // A row is only accepted when the compulsory fields are present and
        // the price really is a number. Bad rows are counted, not silently
        // dropped, so the operator knows the data file needs fixing.
        if (id.empty() || name.empty() || !priceIsValid)
        {
            recordsRejected++;
            continue;
        }

        bool available = (availableText != "0");

        MenuItem item(id, name, stall, category, price, available);

        if (menuDatabase.insert(item))
        {
            loaded++;
        }
        else
        {
            recordsRejected++;                      // duplicate Item ID in file
        }
    }

    inputFile.close();

    std::cout << "  " << loaded << " menu item(s) loaded from \""
              << fileName << "\"." << std::endl;
    if (recordsRejected > 0)
    {
        std::cout << "  " << recordsRejected
                  << " row(s) rejected (duplicate ID or incomplete record)."
                  << std::endl;
    }

    // A file that was written in sorted order would build a skewed tree, so
    // the tree is balanced immediately after loading.
    menuDatabase.rebalance();
    dataFileName = fileName;
    return true;
}

// Writes the whole database back to disk in ascending Item ID order.
bool MenuModule::saveToCSV(const std::string& fileName) const
{
    std::ofstream outputFile(fileName.c_str());

    if (!outputFile.is_open())
    {
        return false;
    }

    MenuItemList allItems;
    menuDatabase.toListByID(allItems);              // in-order = sorted output

    outputFile << "ItemID,Name,Stall,Category,Price,Available" << std::endl;

    for (int i = 0; i < allItems.size(); i++)
    {
        const MenuItem& item = allItems.get(i);
        outputFile << item.itemID    << ','
                   << item.name      << ','
                   << item.stallName << ','
                   << item.category  << ','
                   << std::fixed << std::setprecision(2) << item.price << ','
                   << (item.available ? 1 : 0) << std::endl;
    }

    outputFile.close();
    return true;
}

// Backup menu used when the CSV file is missing, so the kiosk can still run.
void MenuModule::loadDefaultItems()
{
    menuDatabase.clear();
    menuDatabase.insert(MenuItem("M004", "Nasi Lemak Ayam",   "Selera Malaya",  "Malay",   8.50, true));
    menuDatabase.insert(MenuItem("M002", "Mee Goreng Mamak",  "Selera Malaya",  "Malay",   7.00, true));
    menuDatabase.insert(MenuItem("C003", "Chicken Rice",      "Wok Master",     "Chinese", 9.00, true));
    menuDatabase.insert(MenuItem("W001", "Beef Burger",       "Western Corner", "Western", 12.90, true));
    menuDatabase.insert(MenuItem("B005", "Teh Tarik",         "Drinks Bar",     "Beverage", 2.50, true));
    menuDatabase.rebalance();
    std::cout << "  Default menu loaded (" << menuDatabase.size()
              << " items)." << std::endl;
}

// ============================================================================
//  SCREEN OUTPUT
// ============================================================================

void MenuModule::displayTableHeader() const
{
    ConsoleUI::printLine('-');
    std::cout << std::left
              << std::setw(WIDTH_NUMBER)   << "No"
              << std::setw(WIDTH_ID)       << "ID"
              << std::setw(WIDTH_NAME)     << "Item Name"
              << std::setw(WIDTH_STALL)    << "Stall"
              << std::setw(WIDTH_CATEGORY) << "Category"
              << std::right
              << std::setw(WIDTH_PRICE)    << "Price(RM)"
              << std::setw(WIDTH_STATUS)   << "Status"
              << std::endl;
    ConsoleUI::printLine('-');
}

void MenuModule::displayItemRow(int rowNumber, const MenuItem& item) const
{
    std::cout << std::left
              << std::setw(WIDTH_NUMBER)   << rowNumber
              << std::setw(WIDTH_ID)       << item.itemID
              << std::setw(WIDTH_NAME)     << item.name
              << std::setw(WIDTH_STALL)    << item.stallName
              << std::setw(WIDTH_CATEGORY) << item.category
              << std::right << std::fixed  << std::setprecision(2)
              << std::setw(WIDTH_PRICE)    << item.price
              << std::setw(WIDTH_STATUS)
              << (item.available ? "Available" : "Sold Out")
              << std::endl;
}

void MenuModule::displayList(const MenuItemList& list, const std::string& title) const
{
    std::cout << std::endl << "  " << title << std::endl;

    if (list.isEmpty())
    {
        std::cout << "  >> No matching menu item was found." << std::endl;
        return;
    }

    displayTableHeader();
    for (int i = 0; i < list.size(); i++)
    {
        displayItemRow(i + 1, list.get(i));
    }
    ConsoleUI::printLine('-');
    std::cout << "  Total: " << list.size() << " item(s)." << std::endl;
}

void MenuModule::displaySingleItem(const MenuItem& item) const
{
    ConsoleUI::printLine('-');
    std::cout << "  Item ID   : " << item.itemID    << std::endl;
    std::cout << "  Name      : " << item.name      << std::endl;
    std::cout << "  Stall     : " << item.stallName << std::endl;
    std::cout << "  Category  : " << item.category  << std::endl;
    std::cout << "  Price     : RM " << std::fixed << std::setprecision(2)
              << item.price << std::endl;
    std::cout << "  Status    : "
              << (item.available ? "Available" : "Sold Out") << std::endl;
    ConsoleUI::printLine('-');
}

// Shows how much work the last search actually cost. This is what proves,
// during the demonstration, that the BST is searching and not scanning.
void MenuModule::reportSearchCost(const std::string& description) const
{
    std::cout << "  [ " << description << " : "
              << menuDatabase.getLastComparisonCount()
              << " comparison(s) for " << menuDatabase.size()
              << " item(s) ]" << std::endl;
}

// ============================================================================
//  MENU ACTIONS
// ============================================================================

void MenuModule::actionDisplaySortedByID()
{
    MenuItemList allItems;
    menuDatabase.toListByID(allItems);
    displayList(allItems, "FULL MENU - sorted by Item ID (in-order traversal)");
}

void MenuModule::actionDisplaySortedByName()
{
    MenuItemList allItems;
    menuDatabase.toListByName(allItems);
    displayList(allItems, "FULL MENU - sorted by Item Name (in-order on the name index)");
}

void MenuModule::actionSearchByID()
{
    std::string id = ConsoleUI::readRequiredText("  Enter Item ID to search : ");

    const MenuItem* found = menuDatabase.searchByID(id);

    std::cout << std::endl;
    if (found == 0)
    {
        std::cout << "  >> Item \"" << id << "\" does not exist in the menu."
                  << std::endl;
    }
    else
    {
        std::cout << "  ITEM FOUND" << std::endl;
        displaySingleItem(*found);
    }
    reportSearchCost("Search by Item ID");
}

void MenuModule::actionSearchByName()
{
    std::string name = ConsoleUI::readRequiredText("  Enter exact item name : ");

    MenuItemList results;
    menuDatabase.searchByName(name, results);

    displayList(results, "SEARCH RESULT - exact name \"" + name + "\"");
    reportSearchCost("Search by name index");
}

void MenuModule::actionSearchByPrefix()
{
    std::string prefix = ConsoleUI::readRequiredText("  Enter the first few letters : ");

    MenuItemList results;
    menuDatabase.searchByPrefix(prefix, results);

    displayList(results, "SEARCH RESULT - names starting with \"" + prefix + "\"");
    reportSearchCost("Prefix search with subtree pruning");
}

void MenuModule::actionSearchByCategory()
{
    std::string category = ConsoleUI::readRequiredText("  Enter category (Malay / Chinese / Western / Beverage / Dessert) : ");

    MenuItemList results;
    menuDatabase.searchByField(FILTER_CATEGORY, category, results);

    displayList(results, "SEARCH RESULT - category \"" + category + "\"");
    reportSearchCost("Full traversal on a non-key field");
}

void MenuModule::actionSearchByStall()
{
    std::string stall = ConsoleUI::readRequiredText("  Enter stall name : ");

    MenuItemList results;
    menuDatabase.searchByField(FILTER_STALL, stall, results);

    displayList(results, "SEARCH RESULT - stall \"" + stall + "\"");
    reportSearchCost("Full traversal on a non-key field");
}

void MenuModule::actionSearchByPriceRange()
{
    double minimumPrice = ConsoleUI::readDecimal("  Enter minimum price (RM) : ", 0.0, MAX_ITEM_PRICE);
    double maximumPrice = ConsoleUI::readDecimal("  Enter maximum price (RM) : ", 0.0, MAX_ITEM_PRICE);

    if (minimumPrice > maximumPrice)
    {
        double temporary = minimumPrice;            // be forgiving: swap them
        minimumPrice = maximumPrice;
        maximumPrice = temporary;
        std::cout << "  >> Values swapped so that the range is valid."
                  << std::endl;
    }

    MenuItemList results;
    menuDatabase.searchByPriceRange(minimumPrice, maximumPrice, results);

    displayList(results, "SEARCH RESULT - price range");
    reportSearchCost("Range filter on a non-key field");
}

void MenuModule::actionAddItem()
{
    std::cout << std::endl << "  ADD NEW MENU ITEM" << std::endl;

    std::string id = ConsoleUI::readRequiredText("  Item ID    : ");

    // The uniqueness of the primary key is checked BEFORE the rest of the
    // record is typed in, so the user does not waste time on a rejected item.
    if (menuDatabase.searchByID(id) != 0)
    {
        std::cout << "  >> Item ID \"" << id
                  << "\" is already used. Item IDs must be unique." << std::endl;
        return;
    }

    std::string name      = ConsoleUI::readRequiredText("  Item Name  : ");
    std::string stall     = ConsoleUI::readRequiredText("  Stall Name : ");
    std::string category  = ConsoleUI::readRequiredText("  Category   : ");
    double      price     = ConsoleUI::readDecimal("  Price (RM) : ", 0.0, MAX_ITEM_PRICE);
    bool        available = ConsoleUI::readYesNo("  Available now? (Y/N) : ");

    MenuItem newItem(id, name, stall, category, price, available);

    if (menuDatabase.insert(newItem))
    {
        std::cout << std::endl << "  >> Item added successfully." << std::endl;
        displaySingleItem(newItem);
        std::cout << "  [ Insertion cost: " << menuDatabase.getLastComparisonCount()
                  << " comparison(s). Menu now holds " << menuDatabase.size()
                  << " item(s). ]" << std::endl;
    }
    else
    {
        std::cout << "  >> Insertion failed: duplicate Item ID." << std::endl;
    }
}

void MenuModule::actionUpdateItem()
{
    std::string id = ConsoleUI::readRequiredText("  Enter the Item ID to update : ");

    const MenuItem* existing = menuDatabase.searchByID(id);
    if (existing == 0)
    {
        std::cout << "  >> Item \"" << id << "\" was not found." << std::endl;
        return;
    }

    std::cout << std::endl << "  CURRENT RECORD" << std::endl;
    displaySingleItem(*existing);
    std::cout << "  Press ENTER at any field to keep the current value."
              << std::endl << std::endl;

    // The existing values are copied first because the record itself may be
    // moved inside the tree while the update is being applied.
    std::string newName     = existing->name;
    std::string newStall    = existing->stallName;
    std::string newCategory = existing->category;
    double      newPrice    = existing->price;

    std::string typed = ConsoleUI::readLine("  New name     (blank = keep) : ");
    if (!typed.empty())
    {
        newName = typed;
    }
    typed = ConsoleUI::readLine("  New stall    (blank = keep) : ");
    if (!typed.empty())
    {
        newStall = typed;
    }
    typed = ConsoleUI::readLine("  New category (blank = keep) : ");
    if (!typed.empty())
    {
        newCategory = typed;
    }
    typed = ConsoleUI::readLine("  New price    (blank = keep) : ");
    if (!typed.empty())
    {
        bool   priceIsValid = false;
        double value        = TextUtil::toDecimal(typed, priceIsValid);

        if (priceIsValid && value >= 0.0 && value <= MAX_ITEM_PRICE)
        {
            newPrice = value;
        }
        else
        {
            ConsoleUI::showMessage("Invalid price ignored, the old price is kept.");
        }
    }

    bool newAvailability = ConsoleUI::readYesNo("  Available now? (Y/N) : ");

    if (menuDatabase.updateItem(id, newName, newStall, newCategory,
                                newPrice, newAvailability))
    {
        std::cout << std::endl << "  >> Record updated." << std::endl;
        displaySingleItem(*menuDatabase.searchByID(id));
    }
    else
    {
        std::cout << "  >> Update failed: the item no longer exists."
                  << std::endl;
    }
}

void MenuModule::actionChangeAvailability()
{
    std::string id = ConsoleUI::readRequiredText("  Enter the Item ID : ");

    const MenuItem* existing = menuDatabase.searchByID(id);
    if (existing == 0)
    {
        std::cout << "  >> Item \"" << id << "\" was not found." << std::endl;
        return;
    }

    bool newStatus = !existing->available;
    menuDatabase.setAvailability(id, newStatus);

    std::cout << "  >> \"" << existing->name << "\" is now marked as "
              << (newStatus ? "AVAILABLE" : "SOLD OUT") << "." << std::endl;
}

void MenuModule::actionRemoveItem()
{
    std::string id = ConsoleUI::readRequiredText("  Enter the Item ID to remove : ");

    const MenuItem* existing = menuDatabase.searchByID(id);
    if (existing == 0)
    {
        std::cout << "  >> Item \"" << id << "\" was not found." << std::endl;
        return;
    }

    std::cout << std::endl << "  RECORD TO BE REMOVED" << std::endl;
    displaySingleItem(*existing);

    if (!ConsoleUI::readYesNo("  Confirm removal? (Y/N) : "))
    {
        std::cout << "  >> Removal cancelled." << std::endl;
        return;
    }

    if (menuDatabase.remove(id))
    {
        std::cout << "  >> Item removed. The menu now holds "
                  << menuDatabase.size() << " item(s)." << std::endl;
    }
    else
    {
        std::cout << "  >> Removal failed." << std::endl;
    }
}

void MenuModule::actionSaveToFile()
{
    std::string fileName = ConsoleUI::readLine("  File name (blank = " + dataFileName + ") : ");
    if (fileName.empty())
    {
        fileName = dataFileName;
    }

    if (saveToCSV(fileName))
    {
        std::cout << "  >> " << menuDatabase.size()
                  << " item(s) written to \"" << fileName << "\"." << std::endl;
        dataFileName = fileName;
    }
    else
    {
        std::cout << "  >> The file \"" << fileName
                  << "\" could not be opened for writing." << std::endl;
    }
}

// Reports the shape of the tree and offers to rebalance it. Being able to show
// the height before and after is the clearest way to demonstrate why the
// choice of a BST is only efficient while the tree stays reasonably balanced.
void MenuModule::actionTreeStatistics()
{
    if (menuDatabase.isEmpty())
    {
        std::cout << "  >> The menu database is empty." << std::endl;
        return;
    }

    MenuItemList allItems;
    menuDatabase.toListByID(allItems);

    int    availableCount = 0;
    double totalPrice     = 0.0;
    double cheapest       = allItems.get(0).price;
    double dearest        = allItems.get(0).price;

    for (int i = 0; i < allItems.size(); i++)
    {
        const MenuItem& item = allItems.get(i);

        if (item.available)
        {
            availableCount++;
        }
        totalPrice += item.price;

        if (item.price < cheapest)
        {
            cheapest = item.price;
        }
        if (item.price > dearest)
        {
            dearest = item.price;
        }
    }

    int currentHeight = menuDatabase.height();
    int bestHeight    = menuDatabase.optimalHeight();

    std::cout << std::endl << "  MENU DATABASE STATISTICS" << std::endl;
    ConsoleUI::printLine('-');
    std::cout << "  Total items stored        : " << menuDatabase.size() << std::endl;
    std::cout << "  Available / Sold out      : " << availableCount << " / "
              << (menuDatabase.size() - availableCount) << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Price range               : RM " << cheapest
              << "  to  RM " << dearest << std::endl;
    std::cout << "  Average price             : RM "
              << (totalPrice / menuDatabase.size()) << std::endl;
    ConsoleUI::printLine('-');
    std::cout << "  Current tree height       : " << currentHeight << std::endl;
    std::cout << "  Best possible height      : " << bestHeight << std::endl;
    std::cout << "  Worst-case comparisons    : " << currentHeight
              << "  (a linear scan would need " << menuDatabase.size() << ")"
              << std::endl;
    std::cout << "  Shape                     : "
              << ((currentHeight <= bestHeight + 1) ? "well balanced"
                                                    : "SKEWED - rebalancing advised")
              << std::endl;
    ConsoleUI::printLine('-');

    if (currentHeight > bestHeight)
    {
        if (ConsoleUI::readYesNo("  Rebalance the tree now? (Y/N) : "))
        {
            menuDatabase.rebalance();
            std::cout << "  >> Tree rebuilt. Height reduced from " << currentHeight
                      << " to " << menuDatabase.height() << "." << std::endl;
        }
    }
    else
    {
        std::cout << "  The tree is already at its minimum height." << std::endl;
    }
}

// ============================================================================
//  MODULE MENU LOOP
// ============================================================================

void MenuModule::displayMenuOptions() const
{
    std::cout << std::endl;
    ConsoleUI::printLine('=');
    std::cout << "   TASK 4 : MENU ITEM SEARCH AND MANAGEMENT   "
              << "[ Binary Search Tree ]" << std::endl;
    std::cout << "   Items stored: " << menuDatabase.size()
              << "   |   Tree height: " << menuDatabase.height() << std::endl;
    ConsoleUI::printLine('=');
    std::cout << "   DISPLAY                          SEARCH" << std::endl;
    std::cout << "    1. All items (by Item ID)        3. By Item ID" << std::endl;
    std::cout << "    2. All items (by Name)           4. By exact name" << std::endl;
    std::cout << "                                     5. By name prefix" << std::endl;
    std::cout << "   MANAGE                            6. By category" << std::endl;
    std::cout << "    9. Add a new item                7. By stall" << std::endl;
    std::cout << "   10. Update an item                8. By price range" << std::endl;
    std::cout << "   11. Mark available / sold out" << std::endl;
    std::cout << "   12. Remove an item               SYSTEM" << std::endl;
    std::cout << "                                    13. Save menu to file" << std::endl;
    std::cout << "    0. Back to main menu            14. Tree statistics"
              << std::endl;
    ConsoleUI::printLine('=');
}

void MenuModule::run()
{
    int choice = -1;

    while (choice != 0)
    {
        displayMenuOptions();
        choice = ConsoleUI::readInteger("  Select an option (0-14) : ", 0, 14);

        switch (choice)
        {
            case 1:  actionDisplaySortedByID();   break;
            case 2:  actionDisplaySortedByName(); break;
            case 3:  actionSearchByID();          break;
            case 4:  actionSearchByName();        break;
            case 5:  actionSearchByPrefix();      break;
            case 6:  actionSearchByCategory();    break;
            case 7:  actionSearchByStall();       break;
            case 8:  actionSearchByPriceRange();  break;
            case 9:  actionAddItem();             break;
            case 10: actionUpdateItem();          break;
            case 11: actionChangeAvailability();  break;
            case 12: actionRemoveItem();          break;
            case 13: actionSaveToFile();          break;
            case 14: actionTreeStatistics();      break;
            case 0:
                std::cout << "  Returning to the main system menu..." << std::endl;
                break;
            default:
                std::cout << "  >> Invalid option." << std::endl;
                break;
        }

        if (choice != 0)
        {
            ConsoleUI::pause();
        }
    }
}
