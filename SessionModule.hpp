// ============================================================================
//  File        : SessionModule.hpp
//  Module      : TASK 3 - Kiosk Session History and Navigation Module
//  Owner       : <member 3 - put your name and TP number here>
//  Suggested   : STACK (LIFO) - browser-style back navigation and undo
//
//  >>> THIS IS A SKELETON. The owner of Task 3 fills it in. <<<
//  Nobody else edits this file.
//
//  The PUBLIC METHODS BELOW ARE A TEAM CONTRACT: KioskSystem.cpp already calls
//  them. Do not rename or change a signature without telling the team.
// ============================================================================

#ifndef SESSION_MODULE_HPP
#define SESSION_MODULE_HPP

#include "Common.hpp"

class SessionModule
{
public:
    SessionModule();

    // Fixed upper bound for the history stack.
    static const int MAX_SESSION_STEPS = 100;

    // --- Entry point -------------------------------------------------------
    void run();                                     // the Task 3 sub-menu

    // --- CONTRACT used by KioskSystem --------------------------------------
    void startSession(const std::string& studentID); // step 1: ID scanned
    void recordStep(const std::string& description); // step 5: log an action
    bool goBack(std::string& restoredStep);          // step 6: the back button
    void endSession();                               // student walks away
    bool hasActiveSession() const;
    int  stepCount() const;
    void displayHistory() const;
    std::string currentStudentID() const;

private:
    std::string placeholderStudentID;
    std::string stepHistory[MAX_SESSION_STEPS];
    int         historyCount;
};

#endif // SESSION_MODULE_HPP
