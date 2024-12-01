@echo off
set NUGET_URL=https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
set DEV_DIR=dev
set LIB_DIR=lib
set INCLUDE_DIR=lib\include\webview2

:: Check if Node.js is installed
node -v >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Node.js is not installed. Download from here:
    echo https://nodejs.org/
    pause
    exit /b
)

:: Check if Visual Studio is installed
if not exist "C:\Program Files\Microsoft Visual Studio" (
    echo Visual Studio installation directory not found.
    echo If this is an error, press any key to continue.
    echo Otherwise, download Visual Studio from here:
    echo https://visualstudio.microsoft.com/
    echo Be sure to install the "Desktop development with C++" workload.
    pause 
)

:: Check if CMake is installed
cmake --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo CMake is not installed. Download from here:
    echo https://cmake.org/download/
    pause
    exit /b
)

:: Create folders
mkdir %DEV_DIR% 

:: Download NuGet
if not exist "%DEV_DIR%\nuget.exe" (
    echo Downloading NuGet...
    powershell -Command "Invoke-WebRequest -Uri %NUGET_URL% -OutFile %DEV_DIR%\nuget.exe"
)

:: Install WebView2
echo Installing WebView2 with NuGet...
%DEV_DIR%\nuget.exe install Microsoft.Web.WebView2 -Version 1.0.2903.40 -ExcludeVersion -OutputDirectory %DEV_DIR%

:: Copy necessary files
echo Copying files...
copy "%DEV_DIR%\Microsoft.Web.WebView2\build\native\include\WebView2.h" %INCLUDE_DIR%\
copy "%DEV_DIR%\Microsoft.Web.WebView2\build\native\include\WebView2EnvironmentOptions.h" %INCLUDE_DIR%\
copy "%DEV_DIR%\Microsoft.Web.WebView2\build\native\x64\WebView2LoaderStatic.lib" %INCLUDE_DIR%\

echo Completed successfully!
pause
