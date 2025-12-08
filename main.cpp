#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <stdint.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

template <typename K, typename V> struct LruNode {
  using LruType = LruNode<K, V>;
  friend std::ostream &operator<<(std::ostream &os, const LruType &dt);
  K key;
  V val;
  int &size;

  // Fix: Need to use std::weak_ptr for the prev pointer
   std::weak_ptr<LruType> prev;
  std::shared_ptr<LruType> next = nullptr;
  LruNode(K key, V val, int &size) : key(key), val(val), size(size) {
    ++size;
#ifndef NOPRINT
    std::cout << "size:" << size << " [new] " << key << ":" << val << std::endl;
#endif
  }
  ~LruNode() {
    --size;
#ifndef NOPRINT
    std::cout << "size:" << size << " [deleted] " << key << ":" << val
              << std::endl;
#endif
  }
};

template <typename K, typename V>
std::ostream &operator<<(std::ostream &in, LruNode<K, V> *elem) {
  if(elem){
    in << "[" << elem->key << ":" << elem->val << "]";
  }
  return in;
}

template <typename K, typename V, int SIZE> class LruList {
  using KeyType =
      std::conditional_t<std::is_same_v<K, std::string>, std::string_view, K>;
  using LruType = LruNode<K, V>;

public:
  void Print() {
    std::cout << "size:" << size << " ";
    auto elem = first;
    if (elem) {
      while (elem->next) {
        std::cout << elem;
        elem = elem->next;
      }
      std::cout << elem;
    }
    std::cout << std::endl;
  }

  void Set(const K &key, const V &val) {
    auto node = Find(key);
    if (node != nullptr) {
      node->val = val;
#ifndef NOPRINT
      std::cout << "size:" << size << " [update] " << key << ":" << val
                << std::endl;
#endif
    } else {
      node = std::make_shared<LruType>(key, val, size);
      AddNodeToDB(node);
    }
    MoveToStart(node);
    if (size > max_size) {
      Delete(last);
    }
  }

  std::optional<V> Get(const KeyType &key) {
    auto result = Find(key);
    if (result) {
      MoveToStart(result);
      return result->val;
    } else {
      return std::nullopt;
    }
  }

  void Delete(std::shared_ptr<LruType> node) {
    if (node) {
      DeleteNodeFromDB(node);
      DetachNode(node);
    }
  }

  void Delete(const KeyType &key) {
    auto node = Find(key);
    Delete(node);
  }

  void Clear() {
    first = nullptr;
    key_db.clear();
    last = nullptr;
#ifndef NOPRINT
    std::cout << "size:" << size << " [clear] " << std::endl;
#endif
  }

private:
  void MoveToStart(std::shared_ptr<LruType> node) {
    if (node) {
      if (first == node)
        return;
      DetachNode(node);
      if (first != nullptr) {
        first->prev = node;
      }
      if (last == nullptr) {
        last = node;
      }

      node->next = first;
      node->prev.reset();
      first = node;
    }
  }

  void DetachNode(std::shared_ptr<LruType> node) {
    if (node) {
      auto next = node->next;
      auto prev = node->prev.lock();       //reuturn sared_ptr / nullptr (if obext not exist)
      if (node == last)
        last = prev;
      if (node == first)
        first = next;
      if (next != nullptr)
        next->prev = prev;
      if (prev != nullptr)
        prev->next = next;
    }
  }

  void AddNodeToDB(std::shared_ptr<LruType> node) {
    if (node) {
      if constexpr (std::is_same_v<KeyType, std::string_view>) {
        key_db.insert({std::string_view(node->key), node});
      } else {
        key_db.insert({node->key, node});
      }
    }
  }

  void DeleteNodeFromDB(std::shared_ptr<LruType> node) {
    if (node) {
      key_db.erase(node->key);
    }
  }

  std::shared_ptr<LruType> Find(const KeyType &key) {
    auto item = key_db.find(key);
    if (item != key_db.end()) {
      return item->second;
    } else {
      return nullptr;
    }
  }

  std::shared_ptr<LruType> first = nullptr;
  std::shared_ptr<LruType> last = nullptr;
  int max_size = SIZE;
  int size = 0;
  std::unordered_map<KeyType, std::shared_ptr<LruType>> key_db;
};

#ifdef FUZZING
// clang++ -O0 -Wall -g -fsanitize=fuzzer,address,undefined,leak -DFUZZING
// -DNOPRINT main.cpp  -o lru_cache && ./lru_cache
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Size < 6) {
    return -1;
  }
  static LruList<std::string, int, 1000> cache;
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

  LruList<std::string, int, 10> cache;
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
  return 1;
}
#endif