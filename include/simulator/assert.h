#ifndef SIM_ASSERT_H
#define SIM_ASSERT_H

#include <stdio.h>

#define SIM_assert(exp) (exp)? ((void)0) : SIM_handleAssertionFailure(__FILE__, __LINE__, #exp);
#define SIM_assert_always() SIM_handleAssertionFailure(__FILE__, __LINE__, "");
#define SIM_assert_msg(exp, ...) (exp)? ((void)0) : SIM_handleAssertionFailureMsg(__FILE__, __LINE__, __VA_ARGS__);
#define SIM_assert_always_msg(...) SIM_handleAssertionFailureMsg(__FILE__, __LINE__, __VA_ARGS__);

void SIM_handleAssertionFailure(const char* filename, unsigned int lineno, const char* exp);
void SIM_handleAssertionFailureMsg(const char* filename, unsigned int lineno, const char* fmt, ...);
void SIM_debugBreak();

#endif