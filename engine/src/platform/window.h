#pragma once

namespace mist
{

  struct InternalWindowState;

  class Window
  {
    public:
      Window();
      ~Window();

    private:
      InternalWindowState* mState;
  };

}
