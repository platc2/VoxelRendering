#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>


/*
 * An unordered_map like datastructure with a limited capacity.
 * If capacity is reached, the first inserted item is removed from the map.
 */
template <typename Key, typename Value> class LimitedUnorderedMap {
private:
    std::unordered_map<Key, std::shared_ptr<Value>> map;
    std::list<Key> list;
    std::size_t limit;

public:
    LimitedUnorderedMap(const std::size_t limit) : limit(limit) {}

    std::shared_ptr<Value> get(const Key& key) const {
        auto entry = map.find(key);
        if (entry != map.end()) {
            return entry->second;
        }
        return std::shared_ptr<Value>();
    }

    void set(const Key& key, std::shared_ptr<Value> value) {
        map[key] = value;
        list.push_front(key);
        if (list.size() > limit) {
            Key keyToRemove = list.back();
            map.erase(keyToRemove);
            list.pop_back();
        }
    }
};
