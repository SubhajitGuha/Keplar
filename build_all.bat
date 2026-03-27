@ECHO OFF
REM "Build everything"
@REM PUSHD engine
@REM CALL build.bat
@REM IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit )
@REM POPD

@REM PUSHD testbed
@REM CALL build.bat
@REM IF %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)
@REM POPD
REM Engine
make -f "Makefile.engine.windows.mak" all
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit )

REM Testbed
make -f "Makefile.testbed.windows.mak" all
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit )

ECHO "all assemblies build successfully"
pause