#pragma once
#include <exception>
#include <string>

namespace nvm {
namespace exceptions {}  // namespace exceptions

class Exception : public std::exception {
 public:
  explicit Exception(const std::string& message) : message_(message) {}

  virtual const char* what() const noexcept override {
    return message_.c_str();
  }

 private:
  std::string message_;
};

class RuntimeException : public Exception {
 public:
  explicit RuntimeException(const std::string& message) : Exception(message) {}
};

class IoException : public Exception {
 public:
  explicit IoException(const std::string& message) : Exception(message) {}
};

class InvalidArgException : public Exception {
 public:
  explicit InvalidArgException(const std::string& message)
                  : Exception(message) {}
};

class NullReferenceException : public Exception {
 public:
  explicit NullReferenceException(const std::string& message)
                  : Exception(message) {}
};

class BadAllocationException : public Exception {
 public:
  explicit BadAllocationException(const std::string& message)
                  : Exception(message) {}
};

class CastException : public Exception {
 public:
  explicit CastException(const std::string& message)
                  : Exception(message) {}
};

class TypeException : public Exception {
 public:
  explicit TypeException(const std::string& message)
                  : Exception(message) {}
};

class OutOfBoundException : public Exception {
 public:
  explicit OutOfBoundException(const std::string& message)
                  : Exception(message) {}
};

class ThreadException : public Exception {
 public:
  explicit ThreadException(const std::string& message)
                  : Exception(message) {}
};


}  // namespace nvm

