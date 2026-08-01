// ============================================================================
//  File        : Common.hpp
//  Project     : APU Campus Food Court Self-Order Kiosk System (CT077-3-2-DSTR)
//  Owner       : Team leader  -  SHARED FILE
//  Description : The shared foundation every module is built on. It holds the
//                data that travels BETWEEN modules (an Order), plus the text,
//                CSV and console helpers that all five modules need.
//
//  >>> RULES FOR THIS FILE <<<
//  1. Nobody edits this file alone. Any change here affects all five modules,
//     so it must be agreed by the team first.
//  2. Nothing that belongs to a single module goes in here. A queue, a stack
//     or a tree lives in its owner's own files.
//  3. No STL container is used anywhere in this project (no <vector>, <list>,
//     <map>, <set>, <queue>, <stack>, ...). Every container is self-created.
// ============================================================================

#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>

// ----------------------------------------------------------------------------
//  SYSTEM-WIDE CONSTANTS
// ----------------------------------------------------------------------------
const int    MAX_ITEMS_PER_ORDER = 10;   // items a student may put in one order
const int    CONSOLE_WIDTH       = 92;   // width of every table and separator
const double MAX_ITEM_PRICE      = 999.99;

// Data file names. Declared once here so that no module invents its own.
extern const char* FILE_MENU_ITEMS;   // "menu_items.csv"  - Task 4
extern const char* FILE_STALLS;       // "stalls.csv"      - Task 2
extern const char* FILE_ORDERS;       // "orders.csv"      - Task 1
extern const char* FILE_LAYOUT;       // "layout.csv"      - Task 5

// ============================================================================
//  THE ORDER - the "currency" that travels between modules
// ----------------------------------------------------------------------------
//  Task 1 creates an Order and queues it.
//  Task 2 assigns a stall to it.
//  Task 3 records the steps that built it.
//  Task 4 supplies the item details inside it.
//
//  Because it is shared, its definition must NOT be duplicated in any module.
//  Include this header instead.
// ============================================================================

enum OrderStatus
{
    ORDER_PENDING,      // waiting in the order queue
    ORDER_ASSIGNED,     // a stall has been given the order
    ORDER_READY,        // the stall has finished preparing it
    ORDER_COMPLETED,    // collected by the student
    ORDER_CANCELLED     // withdrawn before preparation
};

std::string orderStatusToText(OrderStatus status);

// One line of an order: "2 x Nasi Lemak Ayam @ RM 8.50".
struct OrderLine
{
    std::string itemID;
    std::string itemName;
    double      unitPrice;
    int         quantity;

    OrderLine();
    OrderLine(const std::string& id, const std::string& name,
              double price, int qty);

    double lineTotal() const;
};

// A complete student order.
// The lines are held in a FIXED-SIZE array, not a dynamic container, so that
// this shared header stays independent of any one member's data structure.
struct Order
{
    int         orderID;                        // unique running number
    std::string studentID;                      // scanned at the kiosk
    OrderLine   lines[MAX_ITEMS_PER_ORDER];
    int         lineCount;
    OrderStatus status;
    std::string assignedStall;                  // filled in by Task 2
    int         arrivalSequence;                // used for fair FIFO ordering

    Order();
    Order(int id, const std::string& student, int sequence);

    bool        addLine(const OrderLine& line); // false when the order is full
    double      totalAmount() const;
    int         totalQuantity() const;
    std::string statusText() const;
};

// ============================================================================
//  TextUtil - string helpers used by every module
// ============================================================================
namespace TextUtil
{
    std::string toLower(const std::string& text);
    std::string trim(const std::string& text);
    bool        equalsIgnoreCase(const std::string& first, const std::string& second);

    // Compares 'prefix' against the first prefix.length() characters of 'text'.
    // Returns <0, 0 or >0. The three-way answer is what allows a search over
    // sorted data to prune whole sections instead of checking everything.
    int comparePrefix(const std::string& prefix, const std::string& text);

    // Converts text to a number safely. 'valid' reports whether the WHOLE
    // string was a number, so "12abc" is rejected instead of becoming 12.
    int    toInteger(const std::string& text, bool& valid);
    double toDecimal(const std::string& text, bool& valid);

    // Turns 8.5 into "8.50" - used whenever a price is printed or saved.
    std::string formatPrice(double value);

    // Turns 12 into "12", so numbers can be joined onto message strings.
    std::string toText(int value);
}

// ============================================================================
//  CsvUtil - shared CSV reading helper
//  Every module loads its data from a CSV file, so the field splitter is
//  written once here rather than four times.
// ============================================================================
namespace CsvUtil
{
    // Reads one comma separated field starting at 'start'.
    // Returns the index where the NEXT field begins, or -1 after the last one.
    //
    //   int position = CsvUtil::nextField(line, 0, id);
    //   position     = CsvUtil::nextField(line, position, name);
    int nextField(const std::string& line, int start, std::string& field);

    // True when the line is blank or is the column-header row of the file.
    bool isHeaderOrBlank(const std::string& line, const std::string& firstColumn);
}

// ============================================================================
//  ConsoleUI - shared screen and keyboard helpers
//  Using these keeps all five modules looking like ONE system, and gives every
//  member the same validated input for free.
// ============================================================================
namespace ConsoleUI
{
    void printLine(char symbol = '-');                    // horizontal rule
    void printTitle(const std::string& title);            // boxed heading
    void showMessage(const std::string& message);         // "  >> message"

    std::string readLine(const std::string& prompt);          // may be blank
    std::string readRequiredText(const std::string& prompt);  // never blank
    int         readInteger(const std::string& prompt, int minimum, int maximum);
    double      readDecimal(const std::string& prompt, double minimum, double maximum);
    bool        readYesNo(const std::string& prompt);
    void        pause();                                   // "Press ENTER..."
}

#endif // COMMON_HPP
