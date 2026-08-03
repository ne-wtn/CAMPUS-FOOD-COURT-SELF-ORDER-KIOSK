// ============================================================================
//  File        : SessionModule.hpp
//  Module      : TASK 3 - Kiosk Session History and Navigation Module
//  Owner       : <Member 3 - Dorjee Dhaktsel Lama / TP086416>
//  Description : User-facing Task 3 module that owns the SessionStack,
//                handles session workflow actions and session CSV snapshots.
// ============================================================================

#ifndef SESSION_MODULE_HPP
#define SESSION_MODULE_HPP

#include "Common.hpp"
#include "SessionStack.hpp"

class SessionModule
{
public:
    SessionModule();

    // Fixed upper bound for the history stack.
    static const int MAX_SESSION_STEPS = 100;

    // --- Data --------------------------------------------------------------
    bool loadFromCSV(const std::string& fileName);
    bool saveToCSV(const std::string& fileName) const;

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
    std::string activeStudentID;
    SessionStack stepStack;
    int          activeSessionID;
    int          totalSessionsStarted;
    int          totalStepsRecorded;
    int          totalBackActions;
    std::string  defaultSnapshotFile;

    void displayMenuOptions() const;
    void displaySessionStatus() const;
    bool peekCurrentStep(std::string& stepText) const;
};

#endif // SESSION_MODULE_HPP
