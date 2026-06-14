#pragma once

#include "core/application.h"

extern void testbed();

inline int AppMain(int argc, char* argv[])
{
  mist::Application application = mist::Application();

  testbed();

  return 0;
}

#if platform_linux || platform_apple

int main(int argc, char* argv[])
{
  return AppMain(argc, argv);
}

#endif

#if platform_windows

#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPWSTR cmdline, int cmdshow)
{
  return AppMain(0, nullptr);
}

#endif
