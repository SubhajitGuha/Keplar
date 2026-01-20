@ECHO OFF
REM "Build everything"
PUSHD engine
CALL build.bat
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL && pause )
POPD

PUSHD testbed
CALL build.bat
IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && pause)
POPD

ECHO "all assemblies build successfully"
pause