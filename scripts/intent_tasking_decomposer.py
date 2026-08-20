class IntentTaskingDecomposer:
    """
    Decomposes operator intent commands into execution directives for autonomous effectors.
    """
    @staticmethod
    def parse_intent(command_str: str) -> dict:
        parts = command_str.upper().split(":")
        action = parts[0]
        
        if action == "INTERCEPT":
            return {"action": "TASK_INTERCEPTOR", "target_id": int(parts[1]), "priority": "HIGH"}
        elif action == "JAM":
            return {"action": "TASK_EW_JAMMER", "frequency_ghz": float(parts[1]), "priority": "CRITICAL"}
        
        return {"action": "MONITOR_ONLY", "priority": "LOW"}

if __name__ == "__main__":
    task = IntentTaskingDecomposer.parse_intent("JAM:1.575")
    print(f"[Intent Tasking Engine] Operator Intent Transformed into Effector Directive: {task}")
