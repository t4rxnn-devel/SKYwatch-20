import time

class CircuitBreakerOpenException(Exception):
    pass

class SubsystemCircuitBreaker:
    """
    Guards execution mechanisms against cascading failures across asynchronous threads.
    """
    def __init__(self, failure_threshold=3, recovery_timeout_sec=5.0):
        self.failure_threshold = failure_threshold
        self.recovery_timeout_sec = recovery_timeout_sec
        self.failure_count = 0
        self.state = "CLOSED"  # CLOSED, OPEN, HALF-OPEN
        self.last_state_change = time.time()

    def __call__(self, func, *args, **kwargs):
        now = time.time()

        # Check if OPEN timeout has elapsed to attempt recovery
        if self.state == "OPEN":
            if now - self.last_state_change > self.recovery_timeout_sec:
                self.state = "HALF-OPEN"
                self.last_state_change = now
            else:
                raise CircuitBreakerOpenException(f"[Circuit Breaker] Subsystem isolation active. Function '{func.__name__}' blocked.")

        try:
            result = func(*args, **kwargs)
            
            # Successful call in HALF-OPEN state resets the circuit
            if self.state == "HALF-OPEN":
                self.state = "CLOSED"
                self.failure_count = 0
                self.last_state_change = now
            return result

        except Exception as e:
            self.failure_count += 1
            if self.failure_count >= self.failure_threshold:
                self.state = "OPEN"
                self.last_state_change = now
                print(f"[Fault Recovery Engine] Failure threshold exceeded ({self.failure_count}). Circuit OPENED for '{func.__name__}'.")
            raise e

if __name__ == "__main__":
    breaker = SubsystemCircuitBreaker(failure_threshold=2, recovery_timeout_sec=1.0)

    def faulty_ipc_call():
        raise ConnectionResetError("Simulated IPC Network Drop")

    for i in range(3):
        try:
            breaker(faulty_ipc_call)
        except Exception as err:
            print(f"Handled Execution Guard Notice: {err}")
