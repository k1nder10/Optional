#include <type_traits>
#include <utility>

namespace palkin {

#if __cplusplus < 201703L
#define constexr inline constexpr
#endif

namespace details {

template <typename T>
class optional_base {
public:
  void construct(const optional_base& other) noexcept(
      std::is_nothrow_copy_constructible<T>::value) {
    initialized_ = other.initialized_;
    if (initialized_) new (&storage_) T(other.get());
  }

  void construct(optional_base&& other) noexcept(
      std::is_nothrow_move_constructible<T>::value) {
    initialized_ = other.initialized_;
    if (initialized_) {
      new (&storage_) T(std::move(other.get()));

      other.initialized_ = false;
      other.get().~T();
    }
  }

  optional_base& assign(const optional_base& other) noexcept(
      std::is_nothrow_copy_assignable<T>::value) {
    if (initialized_) {
      get().~T();
      initialized_ = false;
    }

    if (other.initialized_) {
      initialized_ = other.initialized_;
      new (&storage_) T(other.get());
    }

    return *this;
  }

  optional_base& assign(optional_base&& other) noexcept(
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

    return *this;
  }

  // clang-format off
  constexpr bool initialized() const noexcept { return initialized_; }

  constexpr T& get() & { return reinterpret_cast<T&>(storage_); }
  constexpr const T& get() const& { return reinterpret_cast<const T&>(storage_); }
  constexpr T&& get() && { return std::move(get()); }
  constexpr const T&& get() const&& { return std::move(get()); }

  void set(const T& val) { 
    new (&storage_) T(val);
    initialized_ = true;
  }

  void set(T&& val) { 
    new (&storage_) T(std::move(val));
    initialized_ = true;  
  }
  // clang-format on

  bool initialized_{false};
  std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
};

template <typename Base>
struct deleted_copy_construct : Base {
  deleted_copy_construct() = default;
  deleted_copy_construct(const deleted_copy_construct&) = delete;
  deleted_copy_construct(deleted_copy_construct&&) = default;
  deleted_copy_construct& operator=(const deleted_copy_construct&) = default;
  deleted_copy_construct& operator=(deleted_copy_construct&&) = default;
};

template <typename Base>
struct copy_construct : Base {
  copy_construct(const copy_construct& other) noexcept(
      noexcept(this->construct(other))) {
    Base::construct(other);
  }

  copy_construct() = default;
  copy_construct(copy_construct&&) = default;
  copy_construct& operator=(const copy_construct&) = default;
  copy_construct& operator=(copy_construct&&) = default;
};

template <typename Base>
struct deleted_move_construct : Base {
  deleted_move_construct() = default;
  deleted_move_construct(deleted_move_construct&&) = delete;
  deleted_move_construct(const deleted_move_construct&) = default;
  deleted_move_construct& operator=(deleted_move_construct&&) = default;
  deleted_move_construct& operator=(const deleted_move_construct&) = default;
};

template <typename Base>
struct move_construct : Base {
  move_construct(move_construct&& other) noexcept(
      noexcept(this->construct(std::move(other)))) {
    Base::construct(std::move(other));
  }

  move_construct() = default;
  move_construct(const move_construct&) = default;
  move_construct& operator=(move_construct&&) = default;
  move_construct& operator=(const move_construct&) = default;
};

template <typename Base>
struct deleted_copy_assign : Base {
  deleted_copy_assign() = default;
  deleted_copy_assign(const deleted_copy_assign&) = default;
  deleted_copy_assign(deleted_copy_assign&&) = default;
  deleted_copy_assign& operator=(const deleted_copy_assign&) = delete;
  deleted_copy_assign& operator=(deleted_copy_assign&&) = default;
};

template <typename Base>
struct copy_assign : Base {
  copy_assign& operator=(const copy_assign& other) noexcept(
      noexcept(this->assign(other))) {
    Base::assign(other);
    return *this;
  }

  copy_assign() = default;
  copy_assign(const copy_assign&) = default;
  copy_assign(copy_assign&&) = default;
  copy_assign& operator=(copy_assign&&) = default;
};

template <typename Base>
struct deleted_move_assign : Base {
  deleted_move_assign() = default;
  deleted_move_assign(const deleted_move_assign&) = default;
  deleted_move_assign(deleted_move_assign&&) = default;
  deleted_move_assign& operator=(const deleted_move_assign&) = default;
  deleted_move_assign& operator=(deleted_move_assign&&) = delete;
};

template <typename Base>
struct move_assign : Base {
  move_assign& operator=(move_assign&& other) noexcept(
      noexcept(this->assign(std::move(other)))) {
    Base::assign(std::move(other));
    return *this;
  }

  move_assign() = default;
  move_assign(const move_assign&) = default;
  move_assign(move_assign&&) = default;
  move_assign& operator=(const move_assign&) = default;
};

// clang-format off
template <typename T, typename Base>
using select_copy_construct = std::conditional_t<
                              std::is_copy_constructible<T>::value,
                              std::conditional_t<
                              std::is_trivially_copy_constructible<T>::value,
                              Base,
                              copy_construct<Base>>,
                              deleted_copy_construct<Base>>;

// Each next alias delegates a work to the previous one
template <typename T, typename Base>
using select_move_construct = select_copy_construct<T,
                              std::conditional_t<
                              std::is_move_constructible<T>::value,
                              move_construct<Base>,
                              deleted_move_construct<Base>>>;

template <typename T, typename Base>
using select_copy_assign = select_move_construct<T,
                           std::conditional_t<std::is_copy_assignable<T>::value && 
                           std::is_copy_constructible<T>::value,
                           copy_assign<Base>,
                           deleted_copy_assign<Base>>>;

template <typename T, typename Base>
using select_move_assign = select_copy_assign<T, 
                           std::conditional_t<std::is_move_assignable<T>::value &&
                           std::is_move_constructible<T>::value,
                           move_assign<Base>,
                           deleted_move_assign<Base>>>;
// clang-format on

}  // namespace details

struct NullOpt {
  explicit constexpr NullOpt(int) {}
};

const static constexpr NullOpt nullopt{0};

template <typename T>
class Optional
    : protected details::select_move_assign<T, details::optional_base<T>> {
public:
  constexpr Optional() noexcept = default;
  Optional(const T& val) noexcept(std::is_nothrow_constructible<T>::value) {
    this->set(val);
  }

  Optional(T&& val) noexcept(std::is_nothrow_constructible<T>::value) {
    this->set(std::move(val));
  }

  ~Optional() noexcept(std::is_nothrow_destructible<T>::value) {
    if (this->initialized()) this->get().~T();
  }

  // clang-format off
  constexpr explicit operator bool() const noexcept { return this->initialized(); }
  constexpr bool has_value() const noexcept { return this->initialized(); }
  // clang-format on

  T& value() & { return details::optional_base<T>::get(); }
  const T& value() const& { return details::optional_base<T>::get(); }
  T&& value() && { return details::optional_base<T>::get(); }
  const T&& value() const&& { return details::optional_base<T>::get(); }
};

}  // namespace palkin