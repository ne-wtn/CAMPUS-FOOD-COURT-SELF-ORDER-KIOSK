// ============================================================================
//  File        : SessionModule.cpp
//  Module      : TASK 3 - Kiosk Session History and Navigation Module
//  Owner       : <member 3 - put your name and TP number here>
//
//  >>> SKELETON <<<  Replace the bodies with your real stack implementation -
//  do not change the signatures.
// ============================================================================

#include "SessionModule.hpp"

#include <iostream>

namespace
{
    const int MENU_VIEW_HISTORY = 1;
    const int MENU_START_SESSION = 2;
    const int MENU_RECORD_STEP   = 3;
    const int MENU_GO_BACK       = 4;
    const int MENU_END_SESSION   = 5;
    const int MENU_EXIT          = 0;
}

// Creates an empty session module with no active student and no history.
// This is O(1) because it only initializes the fixed state owned by the object.
SessionModule::SessionModule()
    : placeholderStudentID(""), historyCount(0)
{
}

// Shows a small Task 3 menu so the stack can be demonstrated on its own.
// Each menu action is O(1) except history display, which is O(n) in the number
// of recorded steps because every entry must be printed.
void SessionModule::run()
{
    int choice = -1;

    while (choice != MENU_EXIT)
    {
        ConsoleUI::printTitle("TASK 3 : SESSION HISTORY AND NAVIGATION");

        if (hasActiveSession())
        {
            std::cout << "  Active student : " << placeholderStudentID << std::endl;
            std::cout << "  Recorded steps : " << historyCount << std::endl;
        }
        else
        {
            std::cout << "  Active student : none" << std::endl;
            std::cout << "  Recorded steps : 0" << std::endl;
        }

        ConsoleUI::printLine('-');
        std::cout << "  1. View session history" << std::endl;
        std::cout << "  2. Start a new session" << std::endl;
        std::cout << "  3. Record a step" << std::endl;
        std::cout << "  4. Go back one step" << std::endl;
        std::cout << "  5. End current session" << std::endl;
        std::cout << "  0. Return to the main menu" << std::endl;
        ConsoleUI::printLine('-');

        choice = ConsoleUI::readInteger("  Select an option (0-5) : ", 0, 5);

        if (choice == MENU_VIEW_HISTORY)
        {
            displayHistory();
            ConsoleUI::pause();
        }
        else if (choice == MENU_START_SESSION)
        {
            std::string studentID = ConsoleUI::readRequiredText("  Enter student ID : ");
            startSession(studentID);
            recordStep("Login: " + studentID);
            ConsoleUI::showMessage("Session started for " + studentID + ".");
            ConsoleUI::pause();
        }
        else if (choice == MENU_RECORD_STEP)
        {
            if (!hasActiveSession())
            {
                ConsoleUI::showMessage("Start a session first.");
            }
            else
            {
                std::string description = ConsoleUI::readRequiredText("  Step description : ");
                recordStep(description);
            }
            ConsoleUI::pause();
        }
        else if (choice == MENU_GO_BACK)
        {
            std::string restoredStep;
            if (goBack(restoredStep))
            {
                ConsoleUI::showMessage("Went back from: " + restoredStep);
            }
            else
            {
                ConsoleUI::showMessage("No step is available to undo.");
            }
            ConsoleUI::pause();
        }
        else if (choice == MENU_END_SESSION)
        {
            endSession();
            ConsoleUI::showMessage("Session ended.");
            ConsoleUI::pause();
        }
    }
}

// Starts a fresh session for one student and clears the previous history.
// This is O(1) because it only resets the active session state.
void SessionModule::startSession(const std::string& studentID)
{
    placeholderStudentID = studentID;
    historyCount = 0;
}

// Records one step in the session history unless the fixed stack is full.
// This is O(1) because it only appends one entry to the next free slot.
void SessionModule::recordStep(const std::string& description)
{
    if (description.empty())
    {
        ConsoleUI::showMessage("The step description cannot be blank.");
        return;
    }

    if (historyCount >= MAX_SESSION_STEPS)
    {
        ConsoleUI::showMessage("Session history is full.");
        return;
    }

    stepHistory[historyCount] = description;
    historyCount++;
}

// Removes the most recent step and returns it to the caller.
// This is O(1) because it only decrements the stack top and copies one string.
bool SessionModule::goBack(std::string& restoredStep)
{
    if (historyCount <= 0)
    {
        restoredStep = "";
        return false;
    }

    historyCount--;
    restoredStep = stepHistory[historyCount];
    stepHistory[historyCount] = "";

    if (historyCount == 0)
    {
        placeholderStudentID = "";
    }

    return true;
}

// Ends the current session and clears every recorded step.
// This is O(n) because each stored history entry is reset.
void SessionModule::endSession()
{
    placeholderStudentID = "";
    for (int i = 0; i < historyCount; i++)
    {
        stepHistory[i] = "";
    }
    historyCount = 0;
}

// Reports whether a student is currently signed in.
// This is O(1) because it only checks the stored student ID.
bool SessionModule::hasActiveSession() const
{
    return !placeholderStudentID.empty();
}

// Returns the number of recorded steps in the current session.
// This is O(1) because the count is stored directly.
int SessionModule::stepCount() const
{
    return historyCount;
}

// Prints the recorded session steps from oldest to newest.
// This is O(n) because every stored step must be visited once.
void SessionModule::displayHistory() const
{
    ConsoleUI::printLine('-');
    std::cout << "  SESSION HISTORY" << std::endl;
    ConsoleUI::printLine('-');

    if (!hasActiveSession())
    {
        ConsoleUI::showMessage("No active session.");
        return;
    }

    if (historyCount == 0)
    {
        ConsoleUI::showMessage("The session history is empty.");
        return;
    }

    for (int i = 0; i < historyCount; i++)
    {
        std::cout << "  " << (i + 1) << ". " << stepHistory[i] << std::endl;
    }
}

// Returns the scanned student ID for the active session.
// This is O(1) because the ID is stored directly in the module.
std::string SessionModule::currentStudentID() const
{
    return placeholderStudentID;
}
