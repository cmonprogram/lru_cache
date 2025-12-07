#include <iostream>
#include <memory>
#include <optional>
#include <stdint.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

template <typename K, typename V> struct LruNode {
  using LruType = LruNode<K, V>;
  K key;
  V val;
  int &size;
  // Fix: Need to use std::weak_ptr for the prev pointer (which doesn't count
  // towards ownership)
  //  and keep std::shared_ptr only for the next pointer and the map values.
  std::shared_ptr<LruType> prev = nullptr;
  std::shared_ptr<LruType> next = nullptr;
  LruNode(K key, V val, int &size) : key(key), val(val), size(size) {
    ++size;
    std::cout << "size:" << size << " [new] " << key << ":" << val << std::endl;
  }
  ~LruNode() {
    --size;
    std::cout << "size:" << size << " [deleted] " << key << ":" << val
              << std::endl;
  }
};

template <typename K, typename V, int SIZE> class LruList {
  using KeyType =
      std::conditional_t<std::is_same_v<K, std::string>, std::string, K>;
  using LruType = LruNode<K, V>;

public:
  LruList() {}
  ~LruList() {
    while (first != nullptr) {
      first->prev = nullptr;
      first = first->next;
    }
  }

  void Set(const KeyType &key, const V &val) {
    auto node = std::make_shared<LruType>(key, val, size);
    if (first == nullptr) {
      InitNode(node);
      return;
    }
    MoveToStart(node);
    AddNodeToDB(node);
    if (size > max_size)
      DeleteNode(last);
  }

  std::optional<V> Get(const KeyType &key) {
    auto result = Find(key);
    if (result == nullptr) {
      return std::nullopt;
    } else {
      return result->val;
    }
  }

private:
  void InitNode(std::shared_ptr<LruType> node) {
    first = node;
    last = first;
    AddNodeToDB(node);
  }

  void MoveToStart(std::shared_ptr<LruType> node) {
    DeleteNode(Find(node->key));
    if (first != nullptr) {
      first->prev = node;
    }
    node->next = first;
    first = node;
  }

  void DeleteNode(std::shared_ptr<LruType> node) {
    if (node != nullptr) {
      auto next = node->next;
      auto prev = node->prev;
      if (next == nullptr)
        last = prev;
      if (node == first)
        first = next;
      if (next != nullptr)
        next->prev = prev;
      if (prev != nullptr)
        prev->next = next;

      DelteNodeFromDB(node);
    }
  }

  void AddNodeToDB(std::shared_ptr<LruType> node) {
    if constexpr (std::is_same_v<KeyType, std::string_view>) {
      key_db[std::string_view(node->key)] = node;
    } else {
      key_db[node->key] = node;
    }
  }

  void DelteNodeFromDB(std::shared_ptr<LruType> node) {
    key_db.erase(node->key);
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
//clang++ -g -fsanitize=address,fuzzer,undefined main.cpp
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Size < 6) {
    return 0;
  }
  static LruList<std::string, int, 1000> cache;
  std::string key = std::string((char *)Data + 5, Size - 5);
  int val = Data[1] << 24 | Data[2] << 16 | Data[3] << 8 | Data[4];
  if (Data[0] > 128) {
    cache.Set(key,val);
  } else {
    std::cout << "GET: " << cache.Get(key).value_or(0) << std::endl;
  }
  return 0;
}
#else
void start_test() {
  LruList<std::string, int, 10> cache;
  cache.Set("key1", 999);
  cache.Set("key2", 888);
  cache.Set("key3", 777);
  cache.Set("key4", 666);
  cache.Set("key5", 555);
  cache.Set("key6", 444);

  std::cout << "before replace " << cache.Get("key1").value_or(0) << std::endl;
  cache.Set("key1", 1000);
  std::cout << "after replace " << cache.Get("key1").value_or(0) << std::endl;
}
int main() {
  start_test();
  return 1;
}
#endif