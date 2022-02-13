// Copyright (C) 2020 Ilijatech

#include "Application.h"

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application App(nullptr, "Random Picker", { 100, 100, 500, 500 }, 0);
	App.RunMessageLoop();
	return 0;
}