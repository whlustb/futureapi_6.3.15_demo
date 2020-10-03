#pragma once
#pragma execution_character_set("utf-8")

#include <stdio.h>

extern FILE *logfile;
#define LOG(format, ...) fprintf(logfile, format, __VA_ARGS__); printf(format, __VA_ARGS__); fflush(logfile);