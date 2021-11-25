#include <iostream>

// Элемент хеш-таблицы - список объектов с одним хешем
template <class KeyType, class ValueType>
struct Node {
    KeyType key_;
    ValueType value_;
    Node *next_;

    Node(KeyType key, ValueType value) {
        key_ = key;
        value_ = value;
        next_ = nullptr;
    }

    /// Рекурсивное удаление узлов.
    void deleting() {
        if (next_ != nullptr) {
            next_->deleting();
        }
        delete this;
    }
};

// Хеш-таблица
template <class KeyType, class ValueType, class Func = std::hash<KeyType>>
class HashTable {
public:
    HashTable() : HashTable(Func()) {
    }

    explicit HashTable(Func hasher) : HashTable(100, 0.5, hasher) {
    }

    HashTable(size_t capacity, double fullness_ratio, Func hasher = std::hash<KeyType>()) {
        size_ = 0;
        capacity_ = capacity;
        if (fullness_ratio <= 0 || fullness_ratio > 1) {
            fullness_ratio = 0.5;
        }
        fullness_ratio_ = fullness_ratio;
        hasher_ = hasher;
        hash_arr_ = new Node<KeyType, ValueType> *[capacity];
        for (int i = 0; i < capacity; ++i) {
            hash_arr_[i] = nullptr;
        }
    }

    ~HashTable() {
        deleteHashArr(hash_arr_, capacity_);
    }

    void insert(KeyType key, ValueType value) {
        size_t hash = (hasher_(key)) % capacity_;
        Node<KeyType, ValueType> *last_node = nullptr;
        Node<KeyType, ValueType> *current_node = hash_arr_[hash];
        while (current_node) {
            if (current_node->key_ == key) {
                current_node->value_ = value;
                return;
            }
            last_node = current_node;
            current_node = current_node->next_;
        }
        current_node = new Node<KeyType, ValueType>(key, value);
        if (last_node) {
            last_node->next_ = current_node;
        }
        if (!hash_arr_[hash]) {
            hash_arr_[hash] = current_node;
        }
        size_++;
        if (static_cast<double>(size_) / capacity_ > fullness_ratio_) {
            rebuild();
        }
    }

    ValueType *find(KeyType key) {
        size_t hash = (hasher_(key)) % capacity_;
        Node<KeyType, ValueType> *node = hash_arr_[hash];
        while (node && node->key_ != key) {
            node = node->next_;
        }
        if (!node) {
            return nullptr;
        }
        return &(node->value_);
    }

    void erase(KeyType key) {
        size_t hash = (hasher_(key)) % capacity_;
        Node<KeyType, ValueType> *last_node = nullptr;
        Node<KeyType, ValueType> *current_node = hash_arr_[hash];
        while (current_node) {
            if (current_node->key_ == key) {
                if (last_node) {
                    last_node->next_ = current_node->next_;
                }
                if (!last_node) {
                    hash_arr_[hash] = current_node->next_;
                }
                delete current_node;
                size_--;
                return;
            }
            last_node = current_node;
            current_node = current_node->next_;
        }
    }

    Node<KeyType, ValueType> &operator[](uint64_t hash) {
        if (hash >= capacity_ || hash < 0) {
            throw std::out_of_range("");
        }
        if (!hash_arr_[hash]) {
            throw std::runtime_error("");
        }
        return (*(hash_arr_[hash]));
    }

    Node<KeyType, ValueType> at(uint64_t hash) {
        if (hash >= capacity_ || hash < 0) {
            throw std::out_of_range("");
        }
        if (!hash_arr_[hash]) {
            throw std::runtime_error("");
        }
        return (*(hash_arr_[hash]));
    }

    size_t size() const {
        return size_;
    }

    size_t capacity() const {
        return capacity_;
    }

private:
    /// Размер.
    size_t size_;
    /// Вместимость.
    size_t capacity_;
    /// Хэш.
    Func hasher_;
    /// Коэффициент вместимости.
    double fullness_ratio_;
    /// Таблица хэшей.
    Node<KeyType, ValueType> **hash_arr_;

    /// Перестройка хэштаблицы при превышении fullness_ratio_
    void rebuild() {
        auto old_hash_arr = hash_arr_;
        capacity_ *= 2;
        hash_arr_ = new Node<KeyType, ValueType> *[capacity_ * 2];
        for (int i = 0; i < capacity_ * 2; ++i) {
            hash_arr_[i] = nullptr;
        }
        size_ = 0;
        for (int i = 0; i < capacity_ / 2; ++i) {
            auto node = old_hash_arr[i];
            while (node) {
                insert(node->key_, node->value_);
                node = node->next_;
            }
        }
        deleteHashArr(old_hash_arr, capacity_ / 2);
    }

    /// Удаление ассива хэшей.
    void deleteHashArr(Node<KeyType, ValueType> **hash_arr, size_t capacity) {
        if (hash_arr) {
            for (int i = 0; i < capacity; ++i) {
                if (hash_arr[i] != nullptr) {
                    hash_arr[i]->deleting();
                }
            }
            delete[] hash_arr;
        }
    }
};

/*int main() {
    HashTable<int, int> *table = new HashTable<int, int>();
    for (int i = 0; i < 51; i++) {
        table->insert(i, 2);
    }
    for(int i=0; i<51; i++){
        //std::cout<<*(table->find(3))<<std::endl;
        table->erase(i);
    }
    std::cout<<table->size();

    delete table;
}*/
