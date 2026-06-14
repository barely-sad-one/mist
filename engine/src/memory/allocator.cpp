#include "memory/allocator.h"
#include "core/application.h"
#include "assertions.h"
#include "utils/utils.h"
#include "platform/platform.h"

namespace mist
{

  // Arena

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
    while (mArena->prev != nullptr && mArena->basePos >= target)
    {
      ArenaBlock* prev = mArena->prev;
      freeBlock(mArena);
      mArena = prev;
    }

    usize newUsed = target > mArena->basePos ? target - mArena->basePos : 0;
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

  // RtAllocator

  RtAllocator::RtAllocator(usize capacity)
    : mCapacity(capacity),
      mUsed(0)
  {
    assert_trap_msg(capacity > sizeof(BlockHeader), "RtAllocator: capacity is to small to initialize RtAllocator");

    mBase = mist_memory.allocate<u8>(MemoryType::freelist, capacity);
    assert_trap(mBase != nullptr);

    mFreeHead = reinterpret_cast<BlockHeader*>(mBase);
    mFreeHead->size = capacity - sizeof(BlockHeader);
    mFreeHead->next = nullptr;
  }

  void* RtAllocator::allocate(usize size, Alignment align)
  {
    if (size == 0) return nullptr;

    BlockHeader* prev = nullptr;
    BlockHeader* curr = mFreeHead;

    while (curr != nullptr)
    {
      const usize headerEnd = reinterpret_cast<usize>(curr) + sizeof(BlockHeader);
      const usize rawStart  = headerEnd + sizeof(void*);
      const usize userStart = alignUp(rawStart, align);
      const usize padding   = userStart - headerEnd;
      const usize totalNeed = alignUp(padding + size, static_cast<Alignment>(alignof(BlockHeader)));

      if (curr->size >= totalNeed)
      {
        const usize remainder = curr->size - totalNeed;
        const usize minSplit  = sizeof(BlockHeader) + sizeof(void*);

        if (remainder >= minSplit)
        {
          BlockHeader* split = reinterpret_cast<BlockHeader*>(
              reinterpret_cast<u8*>(curr) + sizeof(BlockHeader) + totalNeed);
          split->size = remainder - sizeof(BlockHeader);
          split->next = curr->next;

          if (prev) prev->next = split;
          else      mFreeHead  = split;
        }
        else
        {
          if (prev) prev->next = curr->next;
          else      mFreeHead  = curr->next;
        }

        curr->size = totalNeed;
        curr->next = nullptr;
        mUsed     += sizeof(BlockHeader) + curr->size;

        *reinterpret_cast<BlockHeader**>(userStart - sizeof(void*)) = curr;
        return reinterpret_cast<void*>(userStart);
      }

      prev = curr;
      curr = curr->next;
    }

    assert_trap_msg(false, "RtAllocator::allocate - out of memory");
    return nullptr;
  }

  void RtAllocator::free(void* ptr)
  {
    if (!ptr) return;

    BlockHeader* toFree = *reinterpret_cast<BlockHeader**>(static_cast<u8*>(ptr) - sizeof(void*));
    mUsed -= sizeof(BlockHeader) + toFree->size;

    BlockHeader* prev = nullptr;
    BlockHeader* curr = mFreeHead;
    while (curr != nullptr && curr < toFree)
    {
      prev = curr;
      curr = curr->next;
    }

    toFree->next = curr;
    if (prev) prev->next = toFree;
    else      mFreeHead  = toFree;

    if (curr != nullptr &&
        reinterpret_cast<u8*>(toFree) + sizeof(BlockHeader) + toFree->size == reinterpret_cast<u8*>(curr))
    {
      toFree->size += sizeof(BlockHeader) + curr->size;
      toFree->next  = curr->next;
    }

    if (prev != nullptr &&
        reinterpret_cast<u8*>(prev) + sizeof(BlockHeader) + prev->size == reinterpret_cast<u8*>(toFree))
    {
      prev->size += sizeof(BlockHeader) + toFree->size;
      prev->next  = toFree->next;
    }
  }

  void RtAllocator::reset()
  {
    mFreeHead       = reinterpret_cast<BlockHeader*>(mBase);
    mFreeHead->size = mCapacity - sizeof(BlockHeader);
    mFreeHead->next = nullptr;
    mUsed           = 0;
  }

}
