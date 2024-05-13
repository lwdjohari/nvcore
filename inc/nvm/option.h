#pragma once

namespace nvm {

template <typename T>
class Option {
 private:
  T* value;

 public:
  Option() : value(nullptr) {}
  explicit Option(const T& value) : value(new T(value)) {}
  explicit Option(const Option<T>& other)
      : value(other.value ? new T(*other.value) : nullptr) {}
  ~Option() { delete value; }

  Option& operator=(const Option<T>& other) {
    if (this != &other) {
      delete value;
      value = other.value ? new T(*other.value) : nullptr;
    }
    return *this;
  }

  bool isSome() const { return value != nullptr; }
  bool isNone() const { return value == nullptr; }

  T unwrap() const {
    if (isNone()) {
      throw std::runtime_error(
          "Attempted to unwrap an Option containing no value.");
    }
    return *value;
  }
};

}  // namespace nvm
