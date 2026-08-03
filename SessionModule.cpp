// ============================================================================
//  File        : SessionModule.cpp
//  Module      : TASK 3 - Kiosk Session History and Navigation Module
//  Owner       : <Member 3 - Dorjee Dhaktsel Lama / TP086416>
//  Description : Implementation of the Task 3 workflow screens, stack-based
//                history navigation and CSV snapshot persistence.
// ============================================================================

#include "SessionModule.hpp"

#include <iostream>
#include <fstream>

namespace
{
    const int MENU_VIEW_HISTORY      = 1;
    const int MENU_START_SESSION     = 2;
    const int MENU_RECORD_STEP       = 3;
    const int MENU_GO_BACK           = 4;
    const int MENU_PEEK_CURRENT_STEP = 5;
    const int MENU_END_SESSION       = 6;
    const int MENU_SAVE_SNAPSHOT     = 7;
    const int MENU_LOAD_SNAPSHOT     = 8;
    const int MENU_EXIT              = 0;

    const char* SNAPSHOT_HEADER = "SessionID,StudentID,StepNumber,Description";

    std::string sanitizeCsvField(const std::string& text)
    {
        std::string cleaned = text;
        for (std::string::size_type i = 0; i < cleaned.length(); i++)
        {
            if (cleaned[i] == ',')
            {
                cleaned[i] = ';';
            }
        }
        return cleaned;
    }
}

SessionModule::SessionModule()
    : activeStudentID(""),
      stepStack(MAX_SESSION_STEPS),
      activeSessionID(0),
      totalSessionsStarted(0),
      totalStepsRecorded(0),
      totalBackActions(0),
      defaultSnapshotFile("session_history.csv")
{
}

bool SessionModule::loadFromCSV(const std::string& fileName)
{
    std::ifstream inputFile(fileName.c_str());

    if (!inputFile.is_open())
    {
        return false;
    }

    activeStudentID = "";
    stepStack.clear();

    int loadedSteps  = 0;
    int rejectedRows = 0;
    int expectedStep = 1;
    int loadedID     = 0;
    bool hasLoadedID = false;

    std::string line;
    while (std::getline(inputFile, line))
    {
        if (CsvUtil::isHeaderOrBlank(line, "SessionID"))
        {
            continue;
        }

        std::string sessionIDText;
        std::string studentID;
        std::string stepNumberText;
        std::string description;

        int position = CsvUtil::nextField(line, 0, sessionIDText);
        position = CsvUtil::nextField(line, position, studentID);
        position = CsvUtil::nextField(line, position, stepNumberText);
        CsvUtil::nextField(line, position, description);

        bool idIsValid = false;
        bool stepIsValid = false;
        int  sessionID = TextUtil::toInteger(sessionIDText, idIsValid);
        int  stepNumber = TextUtil::toInteger(stepNumberText, stepIsValid);

        if (!idIsValid || !stepIsValid || sessionID <= 0 ||
            studentID.empty() || description.empty())
        {
            rejectedRows++;
            continue;
        }

        if (!hasLoadedID)
        {
            loadedID     = sessionID;
            hasLoadedID  = true;
            activeStudentID = studentID;
        }

        if (sessionID != loadedID || studentID != activeStudentID ||
            stepNumber != expectedStep)
        {
            rejectedRows++;
            continue;
        }

        if (!stepStack.push(description))
        {
            rejectedRows++;
            continue;
        }

        loadedSteps++;
        expectedStep++;
    }

    inputFile.close();

    if (loadedSteps > 0)
    {
        activeSessionID = loadedID;
    }
    else
    {
        activeSessionID = 0;
        activeStudentID = "";
    }

    std::cout << "  " << loadedSteps << " session step(s) loaded from \""
              << fileName << "\"." << std::endl;
    if (rejectedRows > 0)
    {
        std::cout << "  " << rejectedRows
                  << " row(s) rejected (invalid values or non-sequential steps)."
                  << std::endl;
    }

    defaultSnapshotFile = fileName;
    return true;
}

bool SessionModule::saveToCSV(const std::string& fileName) const
{
    std::ofstream outputFile(fileName.c_str());

    if (!outputFile.is_open())
    {
        return false;
    }

    outputFile << SNAPSHOT_HEADER << std::endl;

    if (hasActiveSession())
    {
        for (int i = 0; i < stepStack.size(); i++)
        {
            outputFile << activeSessionID << ","
                       << sanitizeCsvField(activeStudentID) << ","
                       << (i + 1) << ","
                       << sanitizeCsvField(stepStack.get(i))
                       << std::endl;
        }
    }

    outputFile.close();
    return true;
}

void SessionModule::displaySessionStatus() const
{
    if (!hasActiveSession())
    {
        std::cout << "  Active session : none" << std::endl;
        std::cout << "  Steps recorded : 0 / " << stepStack.capacity() << std::endl;
    }
    else
    {
        std::cout << "  Active session : #" << activeSessionID
                  << "  (" << activeStudentID << ")" << std::endl;
        std::cout << "  Steps recorded : " << stepStack.size()
                  << " / " << stepStack.capacity() << std::endl;
    }

    std::cout << "  Lifetime stats : "
              << totalSessionsStarted << " session(s), "
              << totalStepsRecorded  << " step(s), "
              << totalBackActions    << " back action(s)" << std::endl;
}

void SessionModule::displayMenuOptions() const
{
    std::cout << std::endl;
    ConsoleUI::printLine('=');
    std::cout << "   TASK 3 : SESSION HISTORY AND NAVIGATION   [ Stack ]" << std::endl;
    ConsoleUI::printLine('=');
    std::cout << "   1. View current session history" << std::endl;
    std::cout << "   2. Start a new session" << std::endl;
    std::cout << "   3. Record a step" << std::endl;
    std::cout << "   4. Go back one step (pop)" << std::endl;
    std::cout << "   5. View current step (peek)" << std::endl;
    std::cout << "   6. End current session" << std::endl;
    std::cout << "   7. Save active session snapshot to CSV" << std::endl;
    std::cout << "   8. Load session snapshot from CSV" << std::endl;
    std::cout << "   0. Back to main menu" << std::endl;
    ConsoleUI::printLine('=');
}

void SessionModule::run()
{
    int choice = -1;

    while (choice != MENU_EXIT)
    {
        displayMenuOptions();
        displaySessionStatus();

        choice = ConsoleUI::readInteger("  Select an option (0-8) : ", 0, 8);

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
            std::string description = ConsoleUI::readRequiredText("  Step description : ");
            recordStep(description);
            ConsoleUI::pause();
        }
        else if (choice == MENU_GO_BACK)
        {
            std::string restoredStep;
            if (goBack(restoredStep))
            {
                ConsoleUI::showMessage("Back step removed: " + restoredStep);
            }
            else
            {
                ConsoleUI::showMessage("No removable step is available.");
            }
            ConsoleUI::pause();
        }
        else if (choice == MENU_PEEK_CURRENT_STEP)
        {
            std::string currentStep;
            if (peekCurrentStep(currentStep))
            {
                ConsoleUI::showMessage("Current step: " + currentStep);
            }
            else
            {
                ConsoleUI::showMessage("No step is currently recorded.");
            }
            ConsoleUI::pause();
        }
        else if (choice == MENU_END_SESSION)
        {
            if (hasActiveSession())
            {
                ConsoleUI::showMessage("Session ended for " + activeStudentID + ".");
            }
            else
            {
                ConsoleUI::showMessage("No active session to end.");
            }
            endSession();
            ConsoleUI::pause();
        }
        else if (choice == MENU_SAVE_SNAPSHOT)
        {
            std::string fileName =
                ConsoleUI::readLine("  File name (blank = " + defaultSnapshotFile + ") : ");
            if (fileName.empty())
            {
                fileName = defaultSnapshotFile;
            }

            if (saveToCSV(fileName))
            {
                defaultSnapshotFile = fileName;
                ConsoleUI::showMessage("Session snapshot saved to \"" + fileName + "\".");
            }
            else
            {
                ConsoleUI::showMessage("Could not open \"" + fileName + "\" for writing.");
            }
            ConsoleUI::pause();
        }
        else if (choice == MENU_LOAD_SNAPSHOT)
        {
            std::string fileName =
                ConsoleUI::readLine("  File name (blank = " + defaultSnapshotFile + ") : ");
            if (fileName.empty())
            {
                fileName = defaultSnapshotFile;
            }

            if (loadFromCSV(fileName))
            {
                ConsoleUI::showMessage("Session snapshot loaded from \"" + fileName + "\".");
            }
            else
            {
                ConsoleUI::showMessage("Could not open \"" + fileName + "\".");
            }
            ConsoleUI::pause();
        }
    }
}

void SessionModule::startSession(const std::string& studentID)
{
    std::string cleanedID = TextUtil::trim(studentID);

    if (cleanedID.empty())
    {
        ConsoleUI::showMessage("Student ID cannot be blank.");
        return;
    }

    activeStudentID = cleanedID;
    stepStack.clear();
    activeSessionID++;
    totalSessionsStarted++;
}

void SessionModule::recordStep(const std::string& description)
{
    if (!hasActiveSession())
    {
        ConsoleUI::showMessage("Start a session first.");
        return;
    }

    std::string cleaned = TextUtil::trim(description);
    if (cleaned.empty())
    {
        ConsoleUI::showMessage("Step description cannot be blank.");
        return;
    }
    if (stepStack.isFull())
    {
        ConsoleUI::showMessage("Session history stack is full.");
        return;
    }

    if (stepStack.push(cleaned))
    {
        totalStepsRecorded++;
    }
}

bool SessionModule::goBack(std::string& restoredStep)
{
    if (!hasActiveSession() || stepStack.size() <= 1)
    {
        restoredStep = "";
        return false;
    }

    if (!stepStack.pop(restoredStep))
    {
        return false;
    }

    totalBackActions++;
    return true;
}

void SessionModule::endSession()
{
    activeStudentID = "";
    stepStack.clear();
}

bool SessionModule::hasActiveSession() const
{
    return !activeStudentID.empty();
}

int SessionModule::stepCount() const
{
    return stepStack.size();
}

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

    if (stepStack.isEmpty())
    {
        ConsoleUI::showMessage("No recorded steps yet.");
        return;
    }

    for (int i = 0; i < stepStack.size(); i++)
    {
        std::cout << "  " << (i + 1) << ". " << stepStack.get(i) << std::endl;
    }
}

std::string SessionModule::currentStudentID() const
{
    return activeStudentID;
}

bool SessionModule::peekCurrentStep(std::string& stepText) const
{
    return stepStack.peek(stepText);
}
