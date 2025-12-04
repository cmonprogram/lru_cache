#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#define LruType LruNode<K, V>

template <typename K, typename V> struct LruNode {
  K key;
  V val;
  int &size;
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
public:
  LruList() {}
  ~LruList() {
    while (first != nullptr) {
      first->prev = nullptr;
      first = first->next;
    }
  }

  void Set(const K &key, const V &val) {
    if (first == nullptr) {
      auto node = std::make_shared<LruType>(key, val, size);
      InitNode(node);
      return;
    }

    auto node = std::make_shared<LruType>(key, val, size);
    MoveToStart(node);

    if (size > max_size)
      Delete(last);
  }

  std::optional<V> Get(const K &key) {
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
    key_db[node->key] = first;
  }
  void MoveToStart(std::shared_ptr<LruType> node) {
    Delete(Find(node->key));
    if (first)
      first->prev = node;
    node->next = first;
    first = node;
    key_db[node->key] = node;
  }
  void Delete(std::shared_ptr<LruType> node) {
    if (node != nullptr) {
      key_db.erase(node->key);
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
    }
  }
  
  std::shared_ptr<LruType> Find(const K &key) {
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
  std::unordered_map<K, std::shared_ptr<LruType>> key_db;
};

void start_test() {
  LruList<std::string, int, 10> cache;
  cache.Set("key1", 999);
  cache.Set("key2", 888);
  cache.Set("key3", 777);
  cache.Set("key4", 666);
  cache.Set("key5", 555);
  cache.Set("key6", 444);

  std::cout << "before replace " << cache.Get("key5").value_or(0) << std::endl;
  cache.Set("key5", 1000);
  std::cout << "after replace " << cache.Get("key5").value_or(0) << std::endl;
}
int main() {
  start_test();
  return 1;
}
