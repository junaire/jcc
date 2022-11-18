#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>
#include <vector>

namespace jcc {

class Arena {
  static constexpr std::size_t min_chunk_size = 4096;
  static constexpr std::size_t max_chunk_size =
      std::numeric_limits<uint16_t>::max();
  static constexpr std::size_t alignment =
      std::alignment_of<std::max_align_t>::value;

  struct Chunk {
    explicit Chunk(Chunk *ptr) : prev_(ptr) {}

    union {
      Chunk *prev_ = nullptr;
      std::byte aligned_[alignment];
    };
  };

  Chunk *current_chunk_ = nullptr;
  // total size which available to allocate
  std::size_t available_size_ = 0;
  // The total chunks size
  // = allocatedSize_ + availableSize_ + Memory Deprecated (Size can't fit
  // allocation) Current pointer to available memory address
  std::byte *current_ptr_ = nullptr;

 public:
  Arena() = default;
  Arena(const Arena &) = delete;
  Arena(Arena &&) = delete;
  ~Arena() {
    while (current_chunk_ != nullptr) {
      Chunk *prev = current_chunk_->prev_;
      delete[] current_chunk_;
      current_chunk_ = prev;
    }
    available_size_ = 0;
    current_ptr_ = nullptr;
  }

  // The CPU access memory with the alignment,
  // So construct object from alignment address will reduce the CPU access count
  // then speed up read/write
  void *AllocateAligned(std::size_t alloc) {
    const std::size_t pad =
        alignment -
        (reinterpret_cast<uintptr_t>(current_ptr_) & (alignment - 1));
    const std::size_t consumption = alloc + pad;
    if (consumption > max_chunk_size) {
      return nullptr;
    }
    if (consumption <= available_size_) {
      void *ptr = current_ptr_ + pad;
      current_ptr_ += consumption;
      available_size_ -= consumption;
      return ptr;
    }
    NewChunk(std::max(alloc, min_chunk_size));
    void *ptr = current_ptr_;
    current_ptr_ += alloc;
    available_size_ -= alloc;
    return ptr;
  }

  [[nodiscard]] std::size_t AvailableSize() const { return available_size_; }

 private:
  // allocate new chunk
  // The current pointer will keep alignment
  void NewChunk(std::size_t size) {
    assert(size != 0 && "Can't allocate 0 size chunk!");
    auto *ptr = new std::byte[size + sizeof(Chunk)];
    current_chunk_ = new (ptr) Chunk(current_chunk_);
    available_size_ = size;
    current_ptr_ = (ptr + sizeof(Chunk));
  }
};

template <typename T>
class Allocator {
  std::vector<void *> slabs_;
  Arena arena_;

 public:
  template <typename U>
  requires std::is_base_of_v<T, U>
  void *Allocate() {
    void *mem = arena_.AllocateAligned(sizeof(U));
    slabs_.push_back(mem);
    return mem;
  }

  static void Deallocate(void *mem) { reinterpret_cast<T *>(mem)->~T(); }

  ~Allocator() {
    for (auto *slab : slabs_) {
      Deallocate(slab);
    }
  }
};
}  // namespace jcc
