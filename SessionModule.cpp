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

SessionModule::SessionModule()
    : placeholderStudentID("")
{
}

void SessionModule::run()
{
    ConsoleUI::printTitle("TASK 3 : SESSION HISTORY AND NAVIGATION   [ not implemented yet ]");
    ConsoleUI::showMessage("This module is still being written by its owner.");
    ConsoleUI::pause();
}

void SessionModule::startSession(const std::string& studentID)
{
    // TODO (Task 3 owner): clear the stack and push the "login" step.
    placeholderStudentID = studentID;
}

void SessionModule::recordStep(const std::string& description)
{
    // TODO (Task 3 owner): push the step onto the history stack.
    (void)description;
}

bool SessionModule::goBack(std::string& restoredStep)
{
    // TODO (Task 3 owner): pop the most recent step and report it back.
    (void)restoredStep;
    return false;
}

void SessionModule::endSession()
{
    placeholderStudentID = "";
}

bool SessionModule::hasActiveSession() const
{
    return !placeholderStudentID.empty();
}

int SessionModule::stepCount() const
{
    return 0;
}

void SessionModule::displayHistory() const
{
    ConsoleUI::showMessage("Task 3 (session history) is not implemented yet.");
}

std::string SessionModule::currentStudentID() const
{
    return placeholderStudentID;
}
