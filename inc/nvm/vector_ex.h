#ifndef NVM_CORE_V2_VECTOR_EX_H
#define NVM_CORE_V2_VECTOR_EX_H

#include <algorithm>
#include <cstddef>

namespace nvm {
namespace vectorex {

template <typename T>
inline bool CompareArrays(const T *seq1, const T *seq2, size_t size) {
  if (!seq1 && !seq2 && size == 0) return true;
  if (!seq1 || !seq2 || size == 0) return false;

  return std::equal(seq1, seq1 + size, seq2);
}

}  // namespace vectorex
}  // namespace nvm

#endif  // NVM_CORE_V2_VECTOR_EX_H
