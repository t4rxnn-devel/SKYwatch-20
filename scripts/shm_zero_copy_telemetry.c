#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdatomic.h>
#include <string.h>

#define SHM_NAME "/skywatch_shm_ring"
#define RING_SIZE 1024

typedef struct {
    double timestamp;
    uint32_t target_id;
    float position[3];
    float velocity[3];
} TelemetryPacket;

typedef struct {
    _Atomic uint32_t head;
    _Atomic uint32_t tail;
    TelemetryPacket ring[RING_SIZE];
} SharedRingBuffer;

int main() {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(SharedRingBuffer));
    
    SharedRingBuffer* buffer = (SharedRingBuffer*)mmap(
        NULL, sizeof(SharedRingBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0
    );

    // Atomic produce packet without mutex locks
    uint32_t current_head = atomic_load(&buffer->head);
    TelemetryPacket packet = {
        .timestamp = 1700000000.0,
        .target_id = 999,
        .position = {1200.5f, -450.2f, 8900.0f},
        .velocity = {340.0f, 12.0f, -1.5f}
    };

    buffer->ring[current_head % RING_SIZE] = packet;
    atomic_store(&buffer->head, current_head + 1);

    printf("[Zero-Copy IPC] Telemetry Packet Pushed via Memory-Mapped Ring Buffer.\n");
    munmap(buffer, sizeof(SharedRingBuffer));
    return 0;
}
