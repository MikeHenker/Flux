@echo off
echo Building Flux Programming Language...

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

REM Try different compilers
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
    echo Using Visual Studio 2019...
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    cl.exe /EHsc /std:c++17 /Fo:build\ /Fe:flux.exe main.cpp lexer.cpp parser.cpp ast.cpp interpreter.cpp
    goto :build_done
)

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    echo Using Visual Studio 2022...
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    cl.exe /EHsc /std:c++17 /Fo:build\ /Fe:flux.exe main.cpp lexer.cpp parser.cpp ast.cpp interpreter.cpp
    goto :build_done
)

REM Try MinGW if available
where g++ >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo Using MinGW g++...
    if not exist "build" mkdir build
    g++ -std=c++17 -O2 -o flux.exe main.cpp lexer.cpp parser.cpp ast.cpp interpreter.cpp
    goto :build_done
)

REM Try clang if available
where clang++ >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo Using Clang++...
    if not exist "build" mkdir build
    clang++ -std=c++17 -O2 -o flux.exe main.cpp lexer.cpp parser.cpp ast.cpp interpreter.cpp
    goto :build_done
)

echo Error: No suitable C++ compiler found!
echo Please install one of the following:
echo - Microsoft Visual Studio 2019/2022 with C++ support
echo - MinGW-w64
echo - Clang
pause
exit /b 1

:build_done
if exist "flux.exe" (
    echo.
    echo Build successful! flux.exe created.
    echo.
    echo Usage:
    echo   flux.exe                    - Start interactive REPL
    echo   flux.exe examples\hello.flux - Run a script file
    echo.
) else (
    echo Build failed!
    pause
    exit /b 1
)

pause