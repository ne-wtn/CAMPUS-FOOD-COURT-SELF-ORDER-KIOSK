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
    // TODO (Task 3 owner): declare your own stack here, in its own
    //       SessionStack.hpp / .cpp files.
    //
    //   Functional requirements from the brief:
    //     - record every step of a kiosk session in the order it happened
    //     - store the whole history from login to the current state
    //     - step backwards one action at a time
    //     - allow a previous state to be revisited and changed
    //     - handle going back on an empty history and exceeding the limit
    std::string placeholderStudentID;
};

#endif // SESSION_MODULE_HPP
