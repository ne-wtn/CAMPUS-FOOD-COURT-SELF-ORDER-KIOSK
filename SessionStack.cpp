// ============================================================================
//  File        : SessionStack.cpp
//  Module      : TASK 3 - Kiosk Session History and Navigation Module
//  Owner       : <Member 3 - Dorjee Dhaktsel Lama / TP086416>
//  Description : Implementation of the SessionStack data structure.
// ============================================================================

#include "SessionStack.hpp"

SessionStack::SessionStack(int maximumSize)
    : entries(0), entryCount(0), maxEntries(maximumSize)
{
    if (maxEntries < 1)
    {
        maxEntries = 1;
    }
    entries = new std::string[maxEntries];
}

SessionStack::SessionStack(const SessionStack& other)
    : entries(0), entryCount(0), maxEntries(1)
{
    copyFrom(other);
}

SessionStack& SessionStack::operator=(const SessionStack& other)
{
    if (this == &other)
    {
        return *this;
    }

    delete[] entries;
    entries = 0;
    entryCount = 0;
    maxEntries = 1;

    copyFrom(other);
    return *this;
}

SessionStack::~SessionStack()
{
    delete[] entries;
    entries = 0;
}

bool SessionStack::push(const std::string& stepText)
{
    if (isFull())
    {
        return false;
    }

    entries[entryCount] = stepText;
    entryCount++;
    return true;
}

bool SessionStack::pop(std::string& stepText)
{
    if (isEmpty())
    {
        stepText = "";
        return false;
    }

    entryCount--;
    stepText = entries[entryCount];
    entries[entryCount] = "";
    return true;
}

bool SessionStack::peek(std::string& stepText) const
{
    if (isEmpty())
    {
        stepText = "";
        return false;
    }

    stepText = entries[entryCount - 1];
    return true;
}

void SessionStack::clear()
{
    for (int i = 0; i < entryCount; i++)
    {
        entries[i] = "";
    }
    entryCount = 0;
}

int SessionStack::size() const
{
    return entryCount;
}

int SessionStack::capacity() const
{
    return maxEntries;
}

bool SessionStack::isEmpty() const
{
    return entryCount == 0;
}

bool SessionStack::isFull() const
{
    return entryCount >= maxEntries;
}

std::string SessionStack::get(int index) const
{
    if (index < 0 || index >= entryCount)
    {
        return "";
    }
    return entries[index];
}

void SessionStack::copyFrom(const SessionStack& other)
{
    maxEntries = other.maxEntries;
    entryCount = other.entryCount;
    entries    = new std::string[maxEntries];

    for (int i = 0; i < entryCount; i++)
    {
        entries[i] = other.entries[i];
    }
}
