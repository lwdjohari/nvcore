#ifndef NVM_CORE_IO_V2_BYTE_STREAM_H
#define NVM_CORE_IO_V2_BYTE_STREAM_H

#include <fstream>
#include <functional>
#include <memory>

namespace nvm {
namespace io {

template <typename T>
class IStream {
 public:
};

class StreamCursor {
 private:
  size_t final_position_;
  size_t move_size_;
  bool is_success_;
  bool can_chain_;

 public:
  StreamCursor()
      : final_position_(0),
        move_size_(0),
        is_success_(false),
        can_chain_(false) {}

  const size_t &FinalPosition() const { return final_position_; }

  const size_t &MoveSize() const { return move_size_; }

  const bool &IsSuccess() const { return is_success_; }

  const bool &CanChain() const { return can_chain_; }

  friend void SetCanCursorChain(StreamCursor &lhs, bool can_chain, bool success,
                                size_t final_position, size_t move_size);
};

template <typename TStream, typename TByte>
class ByteStream {
 private:
 protected:
  TStream stream_;

 public:
  explicit ByteStream(TStream &&stream) : stream_(std::move(stream)){};
  virtual ~ByteStream(){};

  StreamCursor ReadChainStart(size_t position, size_t n) {
    // check for out of bound
    auto s = StreamCursor();
    SetCanCursorChain(s, true, true, position, 0);
    return std::move(s);
  }

  friend void SetCanCursorChain(StreamCursor &lhs, bool can_chain, bool success,
                                size_t final_position, size_t move_size) {
    lhs.can_chain_ = can_chain;
    lhs.is_success_ = success;
    lhs.move_size_ = move_size;
    lhs.final_position_ = final_position;
  }

  template <typename TVal>
  ByteStream<TStream, TByte> &ReadChain(TVal &val, StreamCursor &cursor) {
    if (!cursor.IsSuccess()) {
      return *this;
    }

    size_t moving = 10;  // simulate cursor move
    SetCanCursorChain(cursor, cursor.CanChain(), true,
                      cursor.FinalPosition() + moving, moving);

    return *this;
  }

  template <typename TVal>
  ByteStream<TStream, TByte> &ReadChain(
      StreamCursor &cursor, std::function<void(const TVal &val)> l) {
    if (!cursor.IsSuccess()) {
      return *this;
    }

    size_t moving = 10;  // simulate cursor move
    SetCanCursorChain(cursor, cursor.CanChain(), true,
                      cursor.FinalPosition() + moving, moving);

    TVal val = 100.0f;
    l(val);
    return *this;
  }
};

}  // namespace io
}  // namespace nvm

#endif  // NVM_CORE_IO_V2_BYTE_STREAM_H
