#include <iostream>
#include <vector>
#include <cstdint>
#include <cstddef>

template<typename T, size_t BlockCount = 4096>
class BoundedMemoryPool {
public:
    BoundedMemoryPool() : free_index_(0) {
        // Allocate contiguous 64-byte cache-line aligned memory block
        raw_buffer_ = reinterpret_cast<uint8_t*>(
            ::operator new[](sizeof(T) * BlockCount, std::align_val_t{64})
        );
        for (size_t i = 0; i < BlockCount; ++i) {
            free_list_[i] = reinterpret_cast<T*>(raw_buffer_ + i * sizeof(T));
        }
    }

    ~BoundedMemoryPool() {
        ::operator delete[](raw_buffer_, std::align_val_t{64});
    }

    T* allocate() {
        if (free_index_ >= BlockCount) {
            std::cerr << "[Memory Pool Error] Out of pre-allocated pool slots!\n";
            return nullptr;
        }
        return free_list_[free_index_++];
    }

    void deallocate(T* ptr) {
        if (free_index_ > 0) {
            free_list_[--free_index_] = ptr;
        }
    }

private:
    uint8_t* raw_buffer_;
    T* free_list_[BlockCount];
    size_t free_index_;
};

int main() {
    BoundedMemoryPool<double, 1024> pool;
    double* val = pool.allocate();
    if (val) *val = 9.80665;
    pool.deallocate(val);
    std::cout << "[Structural Engine] Cache-Aligned Fixed Memory Pool Ready.\n";
    return 0;
}
