#include <systemapi.h>
#include <translate.h>
#include <yjson.h>
#include <iostream>

using namespace std::literals;

int main() {
  YJson setting = YJson::O{
    {u8"PairBaidu", YJson::A {0, 0}},
    {u8"PairYoudao", YJson::A {0, 0}},
  };
  Translate tran(setting);
  SetConsoleOutputCP(65001);
  auto const& res = tran.GetResult("翻译结果"s);
  if (!res.empty()) {
    std::cout << res;
  } else {
    std::cout << res << "\nPOST Failed.\n";
  }
  return 0;
}
