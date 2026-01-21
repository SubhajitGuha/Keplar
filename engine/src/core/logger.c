#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "kassert.h"

b8 initilize_logging()
{
    //TODO 
    return TRUE;
}
void shutdown_logging()
{
    //TODO
}

void log_output(LogLevel logLevel,const char* message, ...)
{
    const char* log_levels[] = {"[LOG_FETAL]: ", "[LOG_ERROR]: ", "[LOG_WARN]: ", "[LOG_INFO]: ",
    "[LOG_DEBUG]: ", "[LOG_TRACE]: "};
    char outputBuffer[MAX_LOG_CHARACTERS];
    memset(outputBuffer,0,MAX_LOG_CHARACTERS);

    // Format original message.
    // NOTE: Oddly enough, MS's headers override the GCC/Clang va_list type with a "typedef char* va_list" in some
    // cases, and as a result throws a strange error here. The workaround for now is to just use __builtin_va_list,
    // which is the type GCC/Clang's va_start expects.
    __builtin_va_list args;
    va_start(args, message);
    vsnprintf(outputBuffer,MAX_LOG_CHARACTERS,message, args);
    va_end(args);

    char outputBuffer2[MAX_LOG_CHARACTERS];
    memset(outputBuffer2,0,MAX_LOG_CHARACTERS);
    sprintf(outputBuffer2,"%s%s\n", log_levels[logLevel],outputBuffer);

    printf("%s", outputBuffer2);
}

void report_assertion_faliure(const char* expression, const char* message, const char* file, int lineNumber)
{
    log_output(LOG_FETAL,"Assertion Failure: %s, message: %s, file: %s, line number: %i",expression,message,file,lineNumber);
}
