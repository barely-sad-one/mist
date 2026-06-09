#pragma once

#include "defines.h"
#include "memory/memory.h"

namespace mist
{

  enum class ArenaFlags : u8
  {
    nochain = 0,
    chain   = 1 << 0,
  };


  struct ArenaBlock
  {
    u8*          base;
    usize        capacity;
    usize        commited;
    usize        used;
    usize        commitSize;
    usize        basePos;
    ArenaBlock*  prev;
  };

  class Arena;

  struct ArenaTemp
  {
    ArenaTemp(Arena* arena);
    ~ArenaTemp();

    delete_copy_constructor(ArenaTemp);

    ArenaTemp(ArenaTemp&& o) noexcept : mArena(o.mArena), mPos(o.mPos) { o.mArena = nullptr; }
    ArenaTemp& operator=(ArenaTemp&&) = delete;

    void end();

    private:
      Arena* mArena;
      usize  mPos;
  };

  class Arena
  {
    public:
      static constexpr usize defaultCommitSize = megabytes(1);

      Arena(usize capacity, ArenaFlags flags = ArenaFlags::nochain, usize commitSize = defaultCommitSize);
      ~Arena();

      void* push(usize size, Alignment align = Alignment::default_);

      template <typename T>
      T* push(usize count, Alignment align = Alignment::default_)
      {
        return reinterpret_cast<T*>(push(sizeof(T) * count, align));
      }

      usize pos();
      void popTo(usize pos);
      void pop(usize size);
      ArenaTemp beginTemp();

    private:
      ArenaBlock* allocBlock(usize capacity);
      void freeBlock(ArenaBlock* block);
      void growCommit(usize needed);

      ArenaBlock* mArena;
      ArenaFlags  mFlags;
#ifdef MIST_ARENA_FREELIST
      ArenaBlock* mFreeList;
#endif
  };

}
