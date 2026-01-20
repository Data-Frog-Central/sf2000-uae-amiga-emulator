@echo off
setlocal enabledelayedexpansion

REM Auto-detect current folder
set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
for %%F in ("%SCRIPT_DIR%") do set "FOLDER_NAME=%%~nxF"

REM Convert to WSL path
set "WSL_PATH=/mnt/c/Temp/%FOLDER_NAME%"

echo ========================================
echo Building UAE4ALL - %FOLDER_NAME%
echo ========================================
echo.
echo v152: HYBRID approach (1st load=v145, 2nd+=v140)
echo.

echo [1/4] Cleaning and rebuilding from scratch...
wsl -e bash -c "cd '%WSL_PATH%' && rm -f src/*.o src/**/*.o libretro/**/*.o && make -f Makefile.libretro clean platform=sf2000 && make -f Makefile.libretro platform=sf2000 -j4"

if %errorlevel% neq 0 (
    echo.
    echo *** COMPILATION FAILED ***
    pause
    exit /b 1
)

echo.
echo [2/4] Copying .a to multicore framework...
wsl -e bash -c "mkdir -p /mnt/c/Temp/sf2000_multicore_official/cores/%FOLDER_NAME% && rm -f /mnt/c/Temp/sf2000_multicore_official/cores/%FOLDER_NAME%/*.a && cp '%WSL_PATH%'/uae4all_libretro_sf2000.a /mnt/c/Temp/sf2000_multicore_official/cores/%FOLDER_NAME%/ && cp /mnt/c/Temp/sf2000_multicore_official/cores/uae4all092/Makefile /mnt/c/Temp/sf2000_multicore_official/cores/%FOLDER_NAME%/"

echo.
echo [3/4] Linking core_87000000...
wsl -e bash -c "cd /mnt/c/Temp/sf2000_multicore_official && rm -f core_87000000 core.elf libretro_core.a && make CORE=cores/%FOLDER_NAME% CONSOLE=amiga core_87000000"

if %errorlevel% neq 0 (
    echo.
    echo *** LINKING FAILED ***
    pause
    exit /b 1
)

echo.
echo [4/4] Copying core_87000000 to source folder...
wsl -e bash -c "cp /mnt/c/Temp/sf2000_multicore_official/core_87000000 '%WSL_PATH%'/core_87000000"

echo.
echo ========================================
echo BUILD SUCCESSFUL! %FOLDER_NAME%
echo ========================================
echo.
echo core_87000000 is in: %SCRIPT_DIR%\core_87000000
echo.
