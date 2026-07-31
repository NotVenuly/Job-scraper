@echo off

REM Create build folder if it doesn't exist
if not exist build mkdir build

REM Compile
C:\msys64\ucrt64\bin\g++.exe ^
src\*.cpp ^
-Isrc ^
-IC:\msys64\ucrt64\include ^
-LC:\msys64\ucrt64\lib ^
-lcpr -llexbor -lcurl ^
-o build\JobScraper.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ==========================
    echo Build Successful!
    echo ==========================
) else (
    echo.
    echo ==========================
    echo Build Failed!
    echo ==========================
)

pause