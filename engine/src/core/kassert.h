#pragma once

#include "defines.h"

#define KASSERT_ENABLE

#ifdef KASSERT_ENABLE
#ifdef _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

KAPI void report_assertion_faliure(const char* expression, const char* message, const char* file, int lineNumber);


#define KASSERT(cond) \
if(cond){}\
else{report_assertion_faliure(#cond, "", __FILE__, __LINE__); debugBreak();}

#define KASSERT_MSG(cond, message) \
if(cond){}\
else{report_assertion_faliure(#cond, message, __FILE__, __LINE__); debugBreak();}

#else
#define debugBreak()

#define KASSERT(cond)
#define KASSERT_MSG(cond, message)
#endif