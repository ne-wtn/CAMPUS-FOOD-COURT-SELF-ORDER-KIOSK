// ============================================================================
//  File        : Common.cpp
//  Project     : APU Campus Food Court Self-Order Kiosk System (CT077-3-2-DSTR)
//  Owner       : Team leader  -  SHARED FILE
//  Description : Implementation of the shared Order record and of the TextUtil,
//                CsvUtil and ConsoleUI helper namespaces.
// ============================================================================

#include "Common.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <cstdlib>

// ----------------------------------------------------------------------------
//  Data file names
// ----------------------------------------------------------------------------
const char* FILE_MENU_ITEMS = "menu_items.csv";
const char* FILE_STALLS     = "stalls.csv";
const char* FILE_ORDERS     = "orders.csv";
const char* FILE_LAYOUT     = "layout.csv";

// ============================================================================
//  ORDER
// ============================================================================

std::string orderStatusToText(OrderStatus status)
{
    switch (status)
    {
        case ORDER_PENDING:   return "Pending";
        case ORDER_ASSIGNED:  return "Assigned";
        case ORDER_READY:     return "Ready";
        case ORDER_COMPLETED: return "Completed";
        case ORDER_CANCELLED: return "Cancelled";
        default:              return "Unknown";
    }
}

OrderLine::OrderLine()
    : itemID(""), itemName(""), unitPrice(0.0), quantity(0)
{
}

OrderLine::OrderLine(const std::string& id, const std::string& name,
                     double price, int qty)
    : itemID(id), itemName(name), unitPrice(price), quantity(qty)
{
}

double OrderLine::lineTotal() const
{
    return unitPrice * quantity;
}

Order::Order()
    : orderID(0), studentID(""), lineCount(0), status(ORDER_PENDING),
      assignedStall(""), arrivalSequence(0)
{
}

Order::Order(int id, const std::string& student, int sequence)
    : orderID(id), studentID(student), lineCount(0), status(ORDER_PENDING),
      assignedStall(""), arrivalSequence(sequence)
{
}

// Adds one item line. Returns false when the order has reached its limit,
// which is the "exceptional case" the kiosk must report rather than crash on.
bool Order::addLine(const OrderLine& line)
{
    if (lineCount >= MAX_ITEMS_PER_ORDER)
    {
        return false;
    }
    lines[lineCount] = line;
    lineCount++;
    return true;
}

double Order::totalAmount() const
{
    double total = 0.0;
    for (int i = 0; i < lineCount; i++)
    {
        total += lines[i].lineTotal();
    }
    return total;
}

int Order::totalQuantity() const
{
    int total = 0;
    for (int i = 0; i < lineCount; i++)
    {
        total += lines[i].quantity;
    }
    return total;
}

std::string Order::statusText() const
{
    return orderStatusToText(status);
}

// ============================================================================
//  TextUtil
// ============================================================================

std::string TextUtil::toLower(const std::string& text)
{
    std::string result = text;
    for (std::string::size_type i = 0; i < result.length(); i++)
    {
        result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
    }
    return result;
}

// Removes leading and trailing whitespace. CSV files written on Windows end
// each line with "\r\n", so the '\r' must be stripped or it becomes part of
// the last field and silently breaks every comparison against it.
std::string TextUtil::trim(const std::string& text)
{
    int start = 0;
    int end   = static_cast<int>(text.length()) - 1;

    while (start <= end && std::isspace(static_cast<unsigned char>(text[start])))
    {
        start++;
    }
    while (end >= start && std::isspace(static_cast<unsigned char>(text[end])))
    {
        end--;
    }

    if (start > end)
    {
        return "";
    }
    return text.substr(start, end - start + 1);
}

bool TextUtil::equalsIgnoreCase(const std::string& first, const std::string& second)
{
    if (first.length() != second.length())
    {
        return false;
    }
    return toLower(first) == toLower(second);
}

int TextUtil::comparePrefix(const std::string& prefix, const std::string& text)
{
    std::string::size_type length = prefix.length();

    for (std::string::size_type i = 0; i < length; i++)
    {
        if (i >= text.length())
        {
            return 1;               // text is shorter, so the prefix is larger
        }
        if (prefix[i] < text[i])
        {
            return -1;
        }
        if (prefix[i] > text[i])
        {
            return 1;
        }
    }
    return 0;                       // every character of the prefix matched
}

// Converts text to a whole number. 'valid' is false unless the entire string
// was digits, so a typing mistake can never be read as a number by accident.
int TextUtil::toInteger(const std::string& text, bool& valid)
{
    std::string cleaned = trim(text);

    valid = !cleaned.empty();
    for (std::string::size_type i = 0; i < cleaned.length() && valid; i++)
    {
        if (cleaned[i] < '0' || cleaned[i] > '9')
        {
            valid = false;
        }
    }

    if (!valid)
    {
        return 0;
    }
    return std::atoi(cleaned.c_str());
}

// Converts text to a decimal number. strtod stops at the first character it
// cannot read; if that is the end of the string then the whole input was valid.
double TextUtil::toDecimal(const std::string& text, bool& valid)
{
    std::string cleaned = trim(text);

    if (cleaned.empty())
    {
        valid = false;
        return 0.0;
    }

    char*  endPointer = 0;
    double value      = std::strtod(cleaned.c_str(), &endPointer);

    valid = (*endPointer == '\0');
    return valid ? value : 0.0;
}

std::string TextUtil::formatPrice(double value)
{
    std::ostringstream builder;
    builder << std::fixed << std::setprecision(2) << value;
    return builder.str();
}

std::string TextUtil::toText(int value)
{
    std::ostringstream builder;
    builder << value;
    return builder.str();
}

// ============================================================================
//  CsvUtil
// ============================================================================

int CsvUtil::nextField(const std::string& line, int start, std::string& field)
{
    field = "";

    if (start < 0 || start > static_cast<int>(line.length()))
    {
        return -1;
    }

    std::string::size_type comma =
        line.find(',', static_cast<std::string::size_type>(start));

    if (comma == std::string::npos)
    {
        field = TextUtil::trim(line.substr(start));
        return -1;                              // that was the last field
    }

    field = TextUtil::trim(line.substr(start, comma - start));
    return static_cast<int>(comma) + 1;
}

// Lets a loader skip blank lines and the column-header row in one test.
bool CsvUtil::isHeaderOrBlank(const std::string& line, const std::string& firstColumn)
{
    std::string cleaned = TextUtil::trim(line);

    if (cleaned.empty())
    {
        return true;
    }
    if (cleaned.length() >= firstColumn.length() &&
        TextUtil::equalsIgnoreCase(cleaned.substr(0, firstColumn.length()), firstColumn))
    {
        return true;
    }
    return false;
}

// ============================================================================
//  ConsoleUI
// ============================================================================

void ConsoleUI::printLine(char symbol)
{
    for (int i = 0; i < CONSOLE_WIDTH; i++)
    {
        std::cout << symbol;
    }
    std::cout << std::endl;
}

void ConsoleUI::printTitle(const std::string& title)
{
    std::cout << std::endl;
    printLine('=');
    std::cout << "  " << title << std::endl;
    printLine('=');
}

void ConsoleUI::showMessage(const std::string& message)
{
    std::cout << "  >> " << message << std::endl;
}

std::string ConsoleUI::readLine(const std::string& prompt)
{
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return TextUtil::trim(input);
}

std::string ConsoleUI::readRequiredText(const std::string& prompt)
{
    while (true)
    {
        std::string input = readLine(prompt);
        if (!input.empty())
        {
            return input;
        }
        showMessage("This field cannot be left blank. Please try again.");
    }
}

// A whole line is read and then validated by hand. Reading with "cin >>"
// instead would leave the stream in a failed state after a typing mistake and
// send the menu into an infinite loop - a classic kiosk bug.
int ConsoleUI::readInteger(const std::string& prompt, int minimum, int maximum)
{
    while (true)
    {
        bool valid = false;
        int  value = TextUtil::toInteger(readLine(prompt), valid);

        if (valid && value >= minimum && value <= maximum)
        {
            return value;
        }

        std::ostringstream complaint;
        complaint << "Please enter a whole number between "
                  << minimum << " and " << maximum << ".";
        showMessage(complaint.str());
    }
}

double ConsoleUI::readDecimal(const std::string& prompt, double minimum, double maximum)
{
    while (true)
    {
        bool   valid = false;
        double value = TextUtil::toDecimal(readLine(prompt), valid);

        if (valid && value >= minimum && value <= maximum)
        {
            return value;
        }

        std::ostringstream complaint;
        complaint << "Please enter a number between "
                  << TextUtil::formatPrice(minimum) << " and "
                  << TextUtil::formatPrice(maximum) << ".";
        showMessage(complaint.str());
    }
}

bool ConsoleUI::readYesNo(const std::string& prompt)
{
    while (true)
    {
        std::string input = TextUtil::toLower(readLine(prompt));

        if (input == "y" || input == "yes")
        {
            return true;
        }
        if (input == "n" || input == "no")
        {
            return false;
        }
        showMessage("Please answer Y or N.");
    }
}

void ConsoleUI::pause()
{
    std::cout << std::endl << "  Press ENTER to continue...";
    std::string ignored;
    std::getline(std::cin, ignored);
}
