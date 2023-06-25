#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>
#include "common.h"

void throwe(const char *msg, bool errno_msg) __noreturn;

#endif
