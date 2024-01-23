#include <iostream>

#include "nvm/nvm_core.h"

int main() {
  uint8_t arr_u8[] = {0, 1, 2, 3};
  char arr_ch[] = {0, 1, 2, 3};

  nvm::bytes::ByteOpResult result = nvm::bytes::ByteOpResult::None;
  auto res = nvm::bytes::ToUint32(&arr_u8[0], 4, result,
                                  nvm::bytes::EndianessType::BigEndian);

  std::cout << "Val: " << res << " [" << result << "]" << std::endl;

  res = nvm::bytes::ToUint32(&arr_ch[0], 4, result,
                             nvm::bytes::EndianessType::BigEndian);

  std::cout << "Val: " << res << " [" << result << "]" << std::endl;
  return 0;
}