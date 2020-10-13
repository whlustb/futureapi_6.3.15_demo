#pragma once
#include <stdio.h>

extern FILE *logfile;
#define LOG(format, ...) fprintf(logfile, format, __VA_ARGS__); printf(format, __VA_ARGS__); fflush(logfile);

#define QStr(str) QString::fromLocal8Bit(str)

