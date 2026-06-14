#pragma once

#include "defines.h"
#include "platform/platform.h"
#include "core/logger.h"
#include "memory/memory.h"

namespace mist
{

  struct ApplicationState
  {
    Platform plat;
    Logger logger;
    Memory memory;
  };

  class Application
  {
    public:
      MAPI Application();
      MAPI ~Application();

      MAPI static Application& instance();

      Application(const Application&) = delete;
      Application& operator=(const Application&) = delete;
      Application(Application&&) = delete;
      Application& operator=(Application&&) = delete;

      ApplicationState mAppState;

    private:
      inline static Application* gInstance = nullptr;
  };

}

#define mist_application  mist::Application::instance().mAppState
#define mist_platform     mist::Application::instance().mAppState.plat
#define mist_memory       mist::Application::instance().mAppState.memory
