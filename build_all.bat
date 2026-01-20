@ECHO OFF
REM "Build everything"
IF not exist "bin" mkdir "bin"
PUSHD engine
CALL build.bat
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit )
POPD

PUSHD testbed
CALL build.bat
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)
POPD

ECHO "all assemblies build successfully"
pause