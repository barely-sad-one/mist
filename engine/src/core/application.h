#pragma once

#include "defines.h"
#include "core/logger.h"

namespace mist
{

  struct ApplicationState
  {
    Logger logger;
  };

  class Application
  {
    public:
      Application();
      ~Application();

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

#define application_global mist::Application::instance().mAppState
