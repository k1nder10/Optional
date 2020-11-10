#include <type_traits>
#include <utility>

namespace palkin {

struct NullOpt {
  explicit constexpr NullOpt(int) {}
};

const static constexpr NullOpt nullopt{0};

template <typename T>
class OptionalStorage {
public:
};

template <typename T>
class Optional {
public:
  Optional() noexcept : initialized_(false) {}
  Optional(const T& value) noexcept(std::is_nothrow_constructible<T>::value) {
    initialized_ = true;
    new (&storage_) T(value);
  }

  Optional(const Optional& other) noexcept(
      std::is_nothrow_copy_constructible<T>::value) {
    initialized_ = other.initialized_;
    if (initialized_) new (&storage_) T(other.get());
  }

  Optional& operator=(const Optional& other) noexcept(
      std::is_nothrow_copy_assignable<T>::value) {
    if (initialized_) {
      get().~T();
      initialized_ = false;
    }

    if (other.initialized_) {
      initialized_ = other.initialized_;
      new (&storage_) T(other.get());
    }
  }

  Optional(Optional&& other) noexcept(
      std::is_nothrow_move_constructible<T>::value) {
    initialized_ = other.initialized_;
    if (initialized_) {
      new (&storage_) T(std::move(other.get()));

      other.initialized_ = false;
      other.get().~T();
    }
  }

  Optional& operator=(Optional&& other) noexcept(
      std::is_nothrow_move_assignable<T>::value) {
    if (initialized_) {
      get().~T();
      initialized_ = false;
    }

    if (other.initialized_) {
      initialized_ = other.initialized_;
      new (&storage_) T(std::move(other.get()));

      other.initialized_ = false;
      other.get().~T();
    }
  }

  ~Optional() noexcept(std::is_nothrow_destructible<T>::value) {
    if (initialized_) get().~T();
  }

  T& get() & { return reinterpret_cast<T&>(storage_); }
  const T& get() const& { return reinterpret_cast<const T&>(storage_); }
  T&& get() && { return std::move(get()); }

private:
  bool initialized_;
  std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
};

}  // namespace palkin