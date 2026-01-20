@echo off
setlocal enabledelayedexpansion

REM ========================================
REM UAE4ALL GB300 FrogUI Build Script
REM For use with sf2000_multicore (Trademarked69)
REM Platform: GB300V2 (FROGGY_TYPE=GB300V2)
REM ========================================

REM === CONFIGURATION - EDIT THESE PATHS ===
set MULTICORE_DIR=C:\Temp_FrogUI\sf2000_multicore
REM =========================================

for %%I in (.) do set FOLDER_NAME=%%~nxI

echo ========================================
echo Building %FOLDER_NAME% - Amiga Emulator for GB300 FrogUI
echo Using Trademarked69/sf2000_multicore
echo ========================================

set UAE_DIR=%~dp0

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
set WSL_MULTICORE=/mnt/%MC_DRIVE%%MC_PATH_TAIL%

echo WSL Path: %WSL_UAE_PATH%
echo Multicore: %MULTICORE_DIR%

echo.
echo [1/6] Cleaning previous build...
wsl -e bash -c "cd '%WSL_UAE_PATH%' && rm -f src/*.o src/**/*.o libretro/**/*.o uae4all_libretro_sf2000.a core_87000000 2>/dev/null; true"

echo.
echo [2/6] Compiling UAE4ALL library...
wsl -e bash -c "cd '%WSL_UAE_PATH%' && export PATH=/opt/mips32-mti-elf/2019.09-03-2/bin:/tmp/mips32-mti-elf/2019.09-03-2/bin:$PATH && make -f Makefile.libretro clean platform=sf2000 2>/dev/null; make -f Makefile.libretro platform=sf2000 -j4"

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
echo [3/6] Building multicore support files...
wsl -e bash -c "cd '%WSL_MULTICORE%' && export PATH=/opt/mips32-mti-elf/2019.09-03-2/bin:/tmp/mips32-mti-elf/2019.09-03-2/bin:$PATH && make -C libs/libretro-common 2>/dev/null || true"

echo.
echo [4/6] Copying multicore linker files (GB300V2)...
copy /Y "%MULTICORE_DIR%\linker_scripts\core.ld" "%UAE_DIR%" >nul 2>nul
copy /Y "%MULTICORE_DIR%\linker_scripts\bisrv_GB300_V2-core.ld" "%UAE_DIR%" >nul 2>nul
copy /Y "%MULTICORE_DIR%\libs\libretro-common\libretro-common.a" "%UAE_DIR%" >nul 2>nul

echo.
echo [5/6] Setting up FrogUI core directory...
wsl -e bash -c "mkdir -p '%WSL_MULTICORE%/cores/uae4all152'"
copy /Y "%UAE_DIR%uae4all_libretro_sf2000.a" "%MULTICORE_DIR%\cores\uae4all152\" >nul

echo.
echo [6/7] Linking core_87000000 (GB300V2 FrogUI)...
wsl -e bash -c "cd '%WSL_MULTICORE%' && export PATH=/opt/mips32-mti-elf/2019.09-03-2/bin:/tmp/mips32-mti-elf/2019.09-03-2/bin:$PATH && rm -f core_87000000 core.elf libretro_core.a 2>/dev/null; make CORE=cores/uae4all152 CONSOLE=amiga FROGGY_TYPE=GB300V2 core_87000000"

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
echo [7/7] Copying result...
copy /Y "%MULTICORE_DIR%\core_87000000" "%UAE_DIR%core_87000000_gb300_frogui"

echo.
echo Done!

echo.
echo ========================================
echo BUILD SUCCESSFUL!
echo ========================================
echo.
echo Platform: GB300 V2 FrogUI (Trademarked69 multicore)
echo FROGGY_TYPE: GB300V2
echo.
echo Output: %UAE_DIR%core_87000000_gb300_frogui
for %%A in ("%UAE_DIR%core_87000000_gb300_frogui") do echo Size: %%~zA bytes
echo.
echo To deploy: copy core_87000000 to SD:\cores\amiga\ on GB300
echo.
