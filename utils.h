#pragma once
#include <windows.h>
#include <stdio.h>

#define IS_SET(flag, bit) (((flag) & (bit)) ? TRUE : FALSE)

PCHAR*
CommandLineToArgvA(
	PCHAR CmdLine,
	int* _argc
);

BOOL is_blk_zero(void *ptr, size_t s);
