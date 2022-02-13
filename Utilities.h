// Copyright (C) 2020 Ilijatech

#pragma once
#ifndef __cplusplus
	#error C++ is required for this application!
#endif

#include <Windows.h>
#include <CommCtrl.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <windowsx.h>
#include <exception>
#include <ctime>

#pragma warning(disable: 4996)
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define LP_CLASS_NAME		"Ilijatech - Random Item Picker"

#define I_ERROR_CANNOT_PROCESS_MESSAGE			-0x01
#define I_ERROR_CANNOT_REGISTER_WCLASS			-0x02
#define I_ERROR_CANNOT_CREATE_WINDOW			-0x03

typedef struct
{
	int X, Y, Width, Height;
} WTransform, *LPWTransform;