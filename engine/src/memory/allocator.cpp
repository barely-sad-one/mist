#include "memory/allocator.h"
#include "core/application.h"
#include "assertions.h"
#include "utils/utils.h"
#include "platform/platform.h"

namespace mist
{

  ArenaTemp::ArenaTemp(Arena* arena)
    : mArena(arena), mPos(arena->pos())
  {}

  ArenaTemp::~ArenaTemp()
  {
    if (mArena != nullptr)
      mArena->popTo(mPos);
  }

  void ArenaTemp::end()
  {
    if (mArena != nullptr)
    {
      mArena->popTo(mPos);
      mArena = nullptr;
    }
  }

  Arena::Arena(usize capacity, ArenaFlags flags, usize commitSize)
    : mFlags(flags)
#ifdef MIST_ARENA_FREELIST
    , mFreeList(nullptr)
#endif
  {
    mArena = allocBlock(capacity);
    mArena->commitSize = commitSize;
  }

  Arena::~Arena()
  {
    while (mArena != nullptr)
    {
      ArenaBlock* prev = mArena->prev;
      freeBlock(mArena);
      mArena = prev;
    }
  }

  ArenaBlock* Arena::allocBlock(usize capacity)
  {
#ifdef MIST_ARENA_FREELIST
    ArenaBlock** node = &mFreeList;
    while (*node != nullptr)
    {
      if ((*node)->capacity >= capacity)
      {
        ArenaBlock* block = *node;
        *node = block->prev;
        block->used = 0;
        block->prev = nullptr;
        return block;
      }
      node = &(*node)->prev;
    }
#endif

    ArenaBlock* block = mist_memory.allocate<ArenaBlock>(MemoryType::arena, 1);
    assert_trap(block != nullptr);

    block->base = reinterpret_cast<u8*>(mist_memory.reserve(MemoryType::arena, capacity));
    assert_trap(block->base);

    block->capacity = capacity;
    block->commited = 0;
    block->used = 0;
    block->commitSize = defaultCommitSize;
    block->basePos = 0;
    block->prev = nullptr;
    return block;
  }

  void Arena::freeBlock(ArenaBlock* block)
  {
#ifdef MIST_ARENA_FREELIST
    block->used = 0;
    block->prev = mFreeList;
    mFreeList = block;
#else
    mist_memory.release(MemoryType::arena, block->base, block->capacity);
    block->base = nullptr;
    mist_memory.free<ArenaBlock>(MemoryType::arena, block, 1);
#endif
  }

  void* Arena::push(usize size, Alignment align)
  {
    usize alignedOffset = alignUp(mArena->used, align);
    usize newUsed = alignedOffset + size;

    if (newUsed > mArena->capacity)
    {
      assert_trap((static_cast<u8>(mFlags) & static_cast<u8>(ArenaFlags::chain)) != 0);

      usize newCapacity = (size > mArena->capacity) ? size : mArena->capacity;
      ArenaBlock* block = allocBlock(newCapacity);
      block->commitSize = mArena->commitSize;
      block->basePos = mArena->basePos + mArena->capacity;
      block->prev = mArena;
      mArena = block;

      alignedOffset = 0;
      newUsed       = size;
    }

    growCommit(newUsed);
    void* ptr = mArena->base + alignedOffset;
    mArena->used = newUsed;
    return ptr;
  }

  void Arena::growCommit(usize needed)
  {
    if (needed <= mArena->commited)
      return;

    usize toCommit = needed - mArena->commited;
    usize size = (toCommit > mArena->commitSize) ? toCommit : mArena->commitSize;

    Platform::commit(mArena->base + mArena->commited, size);
    mArena->commited += size;
  }

  usize Arena::pos()
  {
    return mArena->basePos + mArena->used;
  }

  void Arena::popTo(usize target)
  {
    while (mArena->basePos >= target)
    {
      ArenaBlock* prev = mArena->prev;
      assert_trap(prev != nullptr);
      freeBlock(mArena);
      mArena = prev;
    }

    usize newUsed = target - mArena->basePos;
    assert_trap(newUsed <= mArena->used);
    mArena->used = newUsed;

#ifndef MIST_ARENA_FREELIST
    usize commitMask   = mArena->commitSize - 1;
    usize newCommitted = (newUsed + commitMask) & ~commitMask;
    if (newCommitted < mArena->commited)
    {
      Platform::decommit(mArena->base + newCommitted, mArena->commited - newCommitted);
      mArena->commited = newCommitted;
    }
#endif
  }

  void Arena::pop(usize size)
  {
    usize current = pos();
    popTo(size < current ? current - size : 0);
  }

  ArenaTemp Arena::beginTemp()
  {
    return ArenaTemp(this);
  }

}
