#pragma once
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>

class DeterministicScheduler {
public:
    explicit DeterministicScheduler(double target_hz = 60.0) 
        : tick_interval_(std::chrono::nanoseconds(static_cast<uint64_t>(1e9 / target_hz))),
          running_(false) {}

    template<typename Func>
    void execute_tick(Func&& task) {
        auto t_start = std::chrono::steady_clock::now();
        
        // Memory barrier to prevent compiler/CPU out-of-order instruction reordering
        std::atomic_thread_fence(std::memory_order_seq_cst);
        
        task(); // Execute frame workload
        
        std::atomic_thread_fence(std::memory_order_seq_cst);

        // High-precision spin-wait sleep compensation
        auto t_end = std::chrono::steady_clock::now();
        auto elapsed = t_end - t_start;
        if (elapsed < tick_interval_) {
            auto sleep_duration = tick_interval_ - elapsed;
            auto spin_target = std::chrono::steady_clock::now() + sleep_duration;
            while (std::chrono::steady_clock::now() < spin_target) {
                #if defined(__x86_64__) || defined(_M_X64)
                _mm_pause(); // Reduce CPU pipeline power consumption during spin
                #endif
            }
        }
    }

private:
    std::chrono::nanoseconds tick_interval_;
    std::atomic<bool> running_;
};
