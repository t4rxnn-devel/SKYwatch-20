use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::Arc;

pub struct RingBuffer<T, const CAP: usize> {
    buffer: [Option<T>; CAP],
    head: AtomicUsize,
    tail: AtomicUsize,
}

impl<T: Copy, const CAP: usize> RingBuffer<T, CAP> {
    pub fn new() -> Self {
        const UNINIT: Option<f32> = None;
        Self {
            buffer: [None; CAP],
            head: AtomicUsize::new(0),
            tail: AtomicUsize::new(0),
        }
    }

    pub fn push(&mut self, item: T) -> Result<(), &'static str> {
        let head = self.head.load(Ordering::Relaxed);
        let tail = self.tail.load(Ordering::Acquire);

        if (head + 1) % CAP == tail {
            return Err("Buffer Full");
        }

        self.buffer[head] = Some(item);
        self.head.store((head + 1) % CAP, Ordering::Release);
        Ok(())
    }

    pub fn pop(&mut self) -> Option<T> {
        let tail = self.tail.load(Ordering::Relaxed);
        let head = self.head.load(Ordering::Acquire);

        if head == tail {
            return None; // Buffer Empty
        }

        let item = self.buffer[tail].take();
        self.tail.store((tail + 1) % CAP, Ordering::Release);
        item
    }
}

fn main() {
    println!("[Rust Subsystem] Distributed Ring Buffer Module Compiled Successfully.");
}
