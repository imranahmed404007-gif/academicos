@echo off
setlocal
echo ===================================================
echo   Compiling AcademicOS - KUET CSE Terminal App
echo ===================================================

where gcc >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    if exist "C:\msys64\ucrt64\bin\gcc.exe" (
        set "PATH=C:\msys64\ucrt64\bin;C:\msys64\usr\bin;%PATH%"
    )
)

gcc -Wall -Wextra -std=c99 -Iinclude -O2 src\*.c -o academic_os.exe
if %ERRORLEVEL% EQU 0 (
    echo [OK] Compilation successful!
    echo Launching AcademicOS...
    academic_os.exe
) else (
    echo [ERROR] Compilation failed. Please ensure GCC is installed and on your PATH.
    pause
)
