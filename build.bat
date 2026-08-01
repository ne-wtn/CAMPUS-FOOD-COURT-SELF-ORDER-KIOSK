@echo off
REM ===========================================================================
REM  build.bat  -  one-click build for the whole team (Windows)
REM  Usage:  build          -> compile only
REM          build run      -> compile, then start the system
REM          build run 4    -> compile, then jump straight into Task 4
REM ===========================================================================

echo.
echo  Building the Campus Food Court Kiosk System...
echo.

g++ -std=c++11 -Wall -Wextra -o kiosk.exe *.cpp

if errorlevel 1 (
    echo.
    echo  ***  BUILD FAILED  -  fix the errors listed above.  ***
    echo.
    exit /b 1
)

echo.
echo  BUILD OK.  Created kiosk.exe
echo.

if "%1"=="run" (
    kiosk.exe %2
)
