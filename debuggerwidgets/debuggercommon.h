#ifndef DEBUGGERCOMMON_H
#define DEBUGGERCOMMON_H

#include "m64p_types.h"
#include <QString>

const double SCROLL_FRACTION_DEGREES = 15.;

void GetInstructionStrings(QString &qs_args, QString &qs_addr, char* args);

#endif
