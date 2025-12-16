@echo off
REM Multi-Color Comp - Windows Build Script
REM This script builds VST3 and optionally AAX formats on Windows

setlocal enabledelayedexpansion

echo.
echo ================================================
echo   Multi-Color Comp - Windows Build Script
echo ================================================
echo.

REM Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found. Please install CMake from https://cmake.org/download/
    exit /b 1
)

REM Configuration
set BUILD_DIR=build_windows
set BUILD_TYPE=Release
if not "%CMAKE_BUILD_TYPE%"=="" set BUILD_TYPE=%CMAKE_BUILD_TYPE%

REM Detect number of processors for parallel build
if not defined NUMBER_OF_PROCESSORS set NUMBER_OF_PROCESSORS=4

echo Build Configuration:
echo   Build Type: %BUILD_TYPE%
echo   Parallel Jobs: %NUMBER_OF_PROCESSORS%

REM Check for AAX SDK
if defined AAX_SDK_PATH (
    if exist "%AAX_SDK_PATH%" (
        echo   AAX SDK: Found at %AAX_SDK_PATH%
        set AAX_CMAKE_FLAG=-DAAX_SDK_PATH="%AAX_SDK_PATH%"
    ) else (
        echo   AAX SDK: Path set but directory not found
        set AAX_CMAKE_FLAG=
    )
) else (
    echo   AAX SDK: Not set (set AAX_SDK_PATH to enable)
    set AAX_CMAKE_FLAG=
)
echo.

REM Create and enter build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

REM Detect Visual Studio version
echo Detecting Visual Studio...
where cl.exe >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo   Using existing Visual Studio environment
    set GENERATOR=
) else (
    REM Try to find Visual Studio 2022, 2019, or 2017
    if exist "C:\Program Files\Microsoft Visual Studio\2022" (
        echo   Found Visual Studio 2022
        set GENERATOR=-G "Visual Studio 17 2022" -A x64
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019" (
        echo   Found Visual Studio 2019
        set GENERATOR=-G "Visual Studio 16 2019" -A x64
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017" (
        echo   Found Visual Studio 2017
        set GENERATOR=-G "Visual Studio 15 2017" -A x64
    ) else (
        echo   WARNING: No Visual Studio found, using default generator
        set GENERATOR=
    )
)
echo.

REM Configure
echo Configuring CMake...
cmake .. %GENERATOR% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% %AAX_CMAKE_FLAG%
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed
    cd ..
    exit /b 1
)

echo.
echo Building plugins...
cmake --build . --config %BUILD_TYPE% --parallel %NUMBER_OF_PROCESSORS%
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed
    cd ..
    exit /b 1
)

echo.
echo ================================================
echo   Build Complete!
echo ================================================
echo.
echo Built plugins are located at:
echo   VST3: %BUILD_DIR%\MultiColorComp_artefacts\%BUILD_TYPE%\VST3\Multi-Color Comp.vst3
echo   Standalone: %BUILD_DIR%\MultiColorComp_artefacts\%BUILD_TYPE%\Standalone\Multi-Color Comp.exe

if defined AAX_CMAKE_FLAG (
    echo   AAX: %BUILD_DIR%\MultiColorComp_artefacts\%BUILD_TYPE%\AAX\Multi-Color Comp.aaxplugin
)

echo.
echo To install:
echo   VST3: Copy to %%COMMONPROGRAMFILES%%\VST3\
echo   xcopy /E /I "%BUILD_DIR%\MultiColorComp_artefacts\%BUILD_TYPE%\VST3\Multi-Color Comp.vst3" "%%COMMONPROGRAMFILES%%\VST3\Multi-Color Comp.vst3"

if defined AAX_CMAKE_FLAG (
    echo.
    echo   AAX: Copy to %%COMMONPROGRAMFILES%%\Avid\Audio\Plug-Ins\
    echo   xcopy /E /I "%BUILD_DIR%\MultiColorComp_artefacts\%BUILD_TYPE%\AAX\Multi-Color Comp.aaxplugin" "%%COMMONPROGRAMFILES%%\Avid\Audio\Plug-Ins\Multi-Color Comp.aaxplugin"
    echo.
    echo   NOTE: AAX plugins must be signed with PACE wraptool to run in Pro Tools
)

echo.
echo Done!
cd ..
