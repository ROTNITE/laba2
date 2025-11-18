// HashTable.hpp
#pragma once

#include "DynamicArray.hpp"
#include <functional>
#include <stdexcept>
#include <cstddef>

template<typename TKey, typename TValue>
class HashTable {
private:
    struct Entry {
        TKey key;
        TValue value;

        Entry() = default;
        Entry(const TKey& k, const TValue& v)
            : key(k), value(v) {}
    };

    DynamicArray<DynamicArray<Entry>> table_;
    std::size_t count_;
    std::size_t capacity_;
    std::function<std::size_t(const TKey&)> hashFunc_;

    [[nodiscard]] std::size_t getIndex(const TKey& key) const {
        return hashFunc_(key) % capacity_;
    }

public:
    // Вариант для C++17: всегда явно передаём хеш-функцию.
    explicit HashTable(std::size_t initialCapacity,
                       std::function<std::size_t(const TKey&)> hashFunction)
        : table_(),
          count_(0),
          capacity_(initialCapacity),
          hashFunc_(std::move(hashFunction)) {

        table_.reserve(capacity_);
        for (std::size_t i = 0; i < capacity_; ++i) {
            table_.push_back(DynamicArray<Entry>());
        }
    }

    void Add(const TKey& key, const TValue& value) {
        std::size_t index = getIndex(key);
        DynamicArray<Entry>& chain = table_[index];

        for (std::size_t i = 0; i < chain.size(); ++i) {
            if (chain[i].key == key) {
                chain[i].value = value;
                return;
            }
        }

        chain.push_back(Entry(key, value));
        ++count_;
    }

    [[nodiscard]] TValue Get(const TKey& key) const {
        std::size_t index = getIndex(key);
        const DynamicArray<Entry>& chain = table_[index];

        for (std::size_t i = 0; i < chain.size(); ++i) {
            if (chain[i].key == key) {
                return chain[i].value;
            }
        }
        throw std::out_of_range("Key not found");
    }

    [[nodiscard]] bool ContainsKey(const TKey& key) const {
        std::size_t index = getIndex(key);
        const DynamicArray<Entry>& chain = table_[index];

        for (std::size_t i = 0; i < chain.size(); ++i) {
            if (chain[i].key == key) {
                return true;
            }
        }
        return false;
    }

    void Remove(const TKey& key) {
        std::size_t index = getIndex(key);
        DynamicArray<Entry>& chain = table_[index];

        for (std::size_t i = 0; i < chain.size(); ++i) {
            if (chain[i].key == key) {
                // «Сжимаем» цепочку, сдвигая оставшиеся элементы
                for (std::size_t j = i + 1; j < chain.size(); ++j) {
                    chain[j - 1] = chain[j];
                }
                chain.pop_back();
                --count_;
                return;
            }
        }
        throw std::out_of_range("Key not found");
    }

    [[nodiscard]] std::size_t GetCount() const noexcept {
        return count_;
    }

    [[nodiscard]] std::size_t GetCapacity() const noexcept {
        return capacity_;
    }

    [[nodiscard]] DynamicArray<TKey> GetKeys() const {
        DynamicArray<TKey> keys;
        keys.reserve(count_);

        for (std::size_t i = 0; i < capacity_; ++i) {
            const DynamicArray<Entry>& chain = table_[i];
            for (std::size_t j = 0; j < chain.size(); ++j) {
                keys.push_back(chain[j].key);
            }
        }
        return keys;
    }
};
