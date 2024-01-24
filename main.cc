#include <iostream>

#include "nvm/io/byte_stream.h"
#include "nvm/nvm_core.h"

using namespace nvm;
void Test() {
  std::fstream s;
  double d = 0;
  int32_t i = 0;

  auto bs = io::ByteStream<std::fstream, char>(std::move(s));
  auto c = bs.ReadChainStart(10, 16);
  if (c.CanChain()) {
    bs.ReadChain(d, c).ReadChain(d, c).ReadChain(i, c)
    .ReadChain<double>(c, [&d](const double &e) {
      d = double(e);
      std::cout << e << std::endl;
    }).ReadChain<int32_t>(c,[&i](const int32_t &e){
      i = int32_t(e);
    });
  }
};

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