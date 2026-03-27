@ECHO OFF
REM clean everything

ECHO "cleaning everythine..."

REM engine
make -f "Makefile.engine.windows.mak" clean
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

REM testbed
make -f "Makefile.testbed.windows.mak" clean
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "all assembleis cleaned successfully."