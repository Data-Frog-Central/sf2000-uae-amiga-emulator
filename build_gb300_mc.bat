@echo off
setlocal enabledelayedexpansion

REM ========================================
REM UAE4ALL GB300 Build Script (Multicore Official)
REM Uses: gb300_multicore (madcock)
REM ========================================

REM === CONFIGURATION - EDIT THESE PATHS ===
set MULTICORE_DIR=C:\Temp\gb300_multicore
REM =========================================

for %%I in (.) do set FOLDER_NAME=%%~nxI

echo ========================================
echo Building %FOLDER_NAME% - Amiga Emulator for GB300
echo Using gb300_multicore
echo ========================================

set UAE_DIR=%~dp0
set CORE_NAME=%FOLDER_NAME%

REM Convert Windows path to WSL path
set DRIVE_LETTER=%UAE_DIR:~0,1%
for %%a in (a b c d e f g h i j k l m n o p q r s t u v w x y z) do call set DRIVE_LETTER=%%DRIVE_LETTER:%%a=%%a%%
set UAE_PATH_TAIL=%UAE_DIR:~2,-1%
set UAE_PATH_TAIL=%UAE_PATH_TAIL:\=/%
set WSL_UAE_PATH=/mnt/%DRIVE_LETTER%%UAE_PATH_TAIL%

REM Convert MULTICORE_DIR to WSL path
set MC_DRIVE=%MULTICORE_DIR:~0,1%
for %%a in (a b c d e f g h i j k l m n o p q r s t u v w x y z) do call set MC_DRIVE=%%MC_DRIVE:%%a=%%a%%
set MC_PATH_TAIL=%MULTICORE_DIR:~2%
set MC_PATH_TAIL=%MC_PATH_TAIL:\=/%
set WSL_MC_PATH=/mnt/%MC_DRIVE%%MC_PATH_TAIL%

echo WSL Path: %WSL_UAE_PATH%
echo Core name: %CORE_NAME%
echo Multicore: %WSL_MC_PATH%

echo.
echo [1/5] Cleaning previous build...
wsl -e bash -c "cd '%WSL_UAE_PATH%' && rm -f src/*.o src/**/*.o libretro/**/*.o uae4all_libretro_sf2000.a 2>/dev/null; true"

echo.
echo [2/5] Compiling %CORE_NAME% library...
REM Note: We compile with platform=sf2000 because the .a file is identical for both platforms
REM The difference is only in the linking step (different linker script)
wsl -e bash -c "cd '%WSL_UAE_PATH%' && export PATH=/tmp/mips32-mti-elf/2019.09-03-2/bin:$PATH && make -f Makefile.libretro clean platform=sf2000 && make -f Makefile.libretro platform=sf2000 -j4"

if %errorlevel% neq 0 (
    echo.
    echo *** COMPILATION FAILED ***
    pause
    exit /b 1
)

if not exist "%UAE_DIR%uae4all_libretro_sf2000.a" (
    echo.
    echo *** ERROR: uae4all_libretro_sf2000.a not created! ***
    pause
    exit /b 1
)

echo.
echo [3/5] Setting up GB300 multicore framework...
wsl -e bash -c "mkdir -p '%WSL_MC_PATH%/cores/%CORE_NAME%'"

REM Copy the .a file and Makefile
copy /Y "%UAE_DIR%uae4all_libretro_sf2000.a" "%MULTICORE_DIR%\cores\%CORE_NAME%\"
copy /Y "%MULTICORE_DIR%\cores\uae4all-libretro\Makefile" "%MULTICORE_DIR%\cores\%CORE_NAME%\" 2>nul

echo.
echo [4/5] Linking core_87000000 for GB300...
wsl -e bash -c "cd '%WSL_MC_PATH%' && rm -f core_87000000 core.elf libretro_core.a 2>/dev/null; export PATH=/tmp/mips32-mti-elf/2019.09-03-2/bin:$PATH && make CORE=cores/%CORE_NAME% CONSOLE=amiga core_87000000"

if %errorlevel% neq 0 (
    echo.
    echo *** LINKING FAILED ***
    pause
    exit /b 1
)

if not exist "%MULTICORE_DIR%\core_87000000" (
    echo.
    echo *** ERROR: core_87000000 not created! ***
    pause
    exit /b 1
)

echo.
echo [5/5] Copying result...
copy /Y "%MULTICORE_DIR%\core_87000000" "%UAE_DIR%core_87000000_gb300"

echo.
echo ========================================
echo BUILD SUCCESSFUL! (GB300 Multicore Official)
echo ========================================
echo.
echo Output: %UAE_DIR%core_87000000_gb300
for %%A in ("%UAE_DIR%core_87000000_gb300") do echo Size: %%~zA bytes
echo.
echo To deploy: copy core_87000000_gb300 to SD:\cores\amiga\core_87000000 on GB300
echo.
