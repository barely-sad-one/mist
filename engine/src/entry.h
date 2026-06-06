#pragma once

#include "core/application.h"

extern void testbed();

int main()
{
  mist::Application application = mist::Application();

  testbed();

  return 0;
}
