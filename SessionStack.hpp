// ============================================================================
//  File        : SessionStack.hpp
//  Module      : TASK 3 - Kiosk Session History and Navigation Module
//  Owner       : <Member 3 - Dorjee Dhaktsel Lama / TP086416>
//  Structure   : STACK (LIFO) - fixed-capacity stack for session steps
//
//  This class is the pure data-structure layer of Task 3 
// ============================================================================

#ifndef SESSION_STACK_HPP
#define SESSION_STACK_HPP

#include <string>

class SessionStack
{
public:
    explicit SessionStack(int maximumSize = 100);
    SessionStack(const SessionStack& other);
    SessionStack& operator=(const SessionStack& other);
    ~SessionStack();

    bool push(const std::string& stepText);
    bool pop(std::string& stepText);
    bool peek(std::string& stepText) const;
    void clear();

    int  size() const;
    int  capacity() const;
    bool isEmpty() const;
    bool isFull() const;

    // Reads a step by position, where index 0 is the oldest step and
    // index size()-1 is the current top of the stack.
    std::string get(int index) const;

private:
    std::string* entries;
    int          entryCount;
    int          maxEntries;

    void copyFrom(const SessionStack& other);
};

#endif // SESSION_STACK_HPP
