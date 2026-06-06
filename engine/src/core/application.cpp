#include "core/application.h"
#include "assertions.h"

namespace mist
{

  Application::Application() {
    assert_trap(gInstance == nullptr);
    gInstance = this;
  }

  Application::~Application() {}

  Application& Application::instance()
  {
    return *gInstance;
  }

}
