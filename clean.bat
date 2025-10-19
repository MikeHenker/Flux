@echo off
echo Cleaning Flux build artifacts...

REM Remove build directory and contents
if exist "build" (
    echo Removing build directory...
    rmdir /s /q "build"
)

REM Remove executable
if exist "flux.exe" (
    echo Removing flux.exe...
    del "flux.exe"
)

REM Remove any stray object files
if exist "*.obj" (
    echo Removing stray object files...
    del "*.obj"
)

REM Remove any stray .o files (Unix)
if exist "*.o" (
    echo Removing .o files...
    del "*.o"
)

REM Remove generated ZIP files
if exist "*.zip" (
    echo Removing ZIP files...
    del "*.zip"
)

echo Clean complete!
pause