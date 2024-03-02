#pragma once

namespace nvm {
template<typename T, typename E>
class Result {
private:
    union {
        T value;
        E error;
    };
    bool isOk;

public:
    Result(const T& value) : value(value), isOk(true) {}
    Result(const E& error, bool) : error(error), isOk(false) {} // Use bool to differentiate constructors
    ~Result() {
        if (isOk) {
            value.~T();
        } else {
            error.~E();
        }
    }

    bool ok() const { return isOk; }

    T unwrap() const {
        if (!isOk) {
            throw std::runtime_error("Attempted to unwrap a Result containing an error.");
        }
        return value;
    }

    E unwrapError() const {
        if (isOk) {
            throw std::runtime_error("Attempted to unwrap an error from a Result containing a value.");
        }
        return error;
    }
};
}
