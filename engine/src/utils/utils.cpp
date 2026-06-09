#include "utils/utils.h"

namespace mist
{

  usize alignUp(usize value, Alignment align)
  {
    usize a = static_cast<usize>(align);
    return (value + a - 1) & ~(a - 1);
  }

}
