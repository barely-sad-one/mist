#pragma once

#include "defines.h"
#include "memory/memory.h"
#include "assertions.h"

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

  struct MAPI ArenaTemp
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

  class MAPI Arena
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


  class MAPI RtAllocator
  {
    public:
      RtAllocator(usize capacity);
      ~RtAllocator();

      void* allocate(usize size, Alignment align = Alignment::default_);
      void free(void* ptr);

      template <typename T>
      T* allocate(usize count, Alignment align = Alignment::default_)
      {
        return reinterpret_cast<T*>(allocate(sizeof(T) * count, align));
      }

      template <typename T>
      void free(T* ptr)
      {
        free(reinterpret_cast<void*>(ptr));
      }

      void reset();

    private:
      struct BlockHeader
      {
        usize size;
        BlockHeader* next;
      };
      static_assert(sizeof(BlockHeader) == 16);

      u8* mBase;
      BlockHeader* mFreeHead;
      usize mCapacity;
      usize mUsed;
  };


  template <typename T>
  class MAPI Pool
  {
    static_assert(std::is_trivially_destructible_v<T>, "Pool<T>: T must be trivially destructible. Use DynPool for types with destructors.");

    static constexpr usize kSlotSize = sizeof(T) >= sizeof(void*) ? sizeof(T) : sizeof(void*);
    static_assert(kSlotSize % alignof(T) == 0, "Pool<T>: slot size does satisfy alignment of T");

    public:
      Pool(Arena& arena, usize capacity)
        : mCapacity(capacity)
      {
        mBase = arena.push<u8>(kSlotSize * capacity);
        assert_trap(mBase != nullptr);

        mFreeHead = nullptr;
        for (isize i = static_cast<isize>(capacity) - 1; i >= 0; --i)
        {
          u8* slot = mBase + i * kSlotSize;
          *(u8**)slot = mFreeHead;
          mFreeHead = slot;
        }
      }

    delete_copy_constructor(Pool);
    delete_move_constructor(Pool);

    T* allocate()
    {
      assert_trap(mFreeHead != nullptr);
      u8* slot = mFreeHead;
      mFreeHead = *(u8**)slot;
      return reinterpret_cast<T*>(slot);
    }

    void deallocate(T* ptr)
    {
      assert_trap(ptr != nullptr);
      u8* slot = reinterpret_cast<u8*>(ptr);
      *(u8**) slot = mFreeHead;
      mFreeHead = slot;
    }

    private:
      u8* mBase;
      u8* mFreeHead;
      usize mCapacity;
  };

}
