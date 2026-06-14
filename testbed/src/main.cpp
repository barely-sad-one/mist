#include <entry.h>
#include <memory/allocator.h>
#include <assertions.h>
#include <collections/array.h>
#include <collections/strings.h>

void testbed()
{
  mist::Arena arena = mist::Arena(megabytes(64));
  f32* ptr = arena.push<f32>(4);
  assert_trap(ptr != nullptr);
  ptr[0] = 32.f;
  ptr[1] = 32.f;
  ptr[2] = 32.f;
  ptr[3] = 32.f;

  for (int i = 0; i < 4; ++i)
  {
    log_debug("%.2f", ptr[i]);
  }

  mist::StringView strview = mist::StringView("This is a Game Engine.");
  log_debug("string view: %s", strview.data());

  mist::String str = mist::String(arena, strview);
  log_debug("string : %s", str.data());

  mist::StringBuilder builder;
  builder.append(arena, strview);
  mist::String sb = builder.build(arena);
  log_debug("string builder: %s", sb.data());

  log_fatal("hello");
}
