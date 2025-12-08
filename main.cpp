#include "lru_cache.h"

#ifdef FUZZING
// clang++ -O0 -Wall -g -fsanitize=fuzzer,address,undefined,leak -DFUZZING
// -DNOPRINT main.cpp  -o lru_cache && ./lru_cache
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Size < 6) {
    return -1;
  }
  static lru_cache::LruList<std::string, int, 1000> cache;
  std::string key = std::string((char *)Data + 5, Size - 5);
  int val = Data[1] << 24 | Data[2] << 16 | Data[3] << 8 | Data[4];
  volatile int tmp = 0;

  switch (Data[0]) {
  case 1:
    cache.Set(key, val);
    break;
  case 2:
    tmp = cache.Get(key).value_or(0);
    break;
  case 3:
    cache.Delete(key);
    break;
  case 4:
    cache.Clear();
    break;
  default:
    return -1;
  }

  return 0;
}
#else
// clang++ -O3 main.cpp -o lru_cache && ./lru_cache
void start_test() {
  lru_cache::LruList<std::string, int, 10> cache;
  cache.Set("key1", 999);
  cache.Set("key2", 888);
  cache.Set("key3", 777);
  cache.Set("key4", 666);
  cache.Set("key5", 555);
  cache.Set("key6", 444);
  cache.Print();
  cache.Delete("key2");
  cache.Set("key5", 1000);
  cache.Print();
}

int main() {
  start_test();
  return 0;
}
#endif
