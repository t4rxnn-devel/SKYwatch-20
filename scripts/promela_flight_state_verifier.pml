/* Formal Logic Verification Engine for Avionics State Machine */
mtype = { IDLE, TRACKING, LOCK_ACQUIRED, FAULT_EMERGENCY };

mtype currentState = IDLE;
bool hardwareOk = true;
bool targetAcquired = false;

inline transition(newState) {
    atomic {
        currentState = newState;
        printf("STATE TRANSITION: %d\n", currentState);
    }
}

proctype FlightSystem() {
    do
    :: (currentState == IDLE) ->
        if
        :: targetAcquired -> transition(TRACKING);
        :: !hardwareOk -> transition(FAULT_EMERGENCY);
        fi;
    :: (currentState == TRACKING) ->
        if
        :: targetAcquired -> transition(LOCK_ACQUIRED);
        :: !targetAcquired -> transition(IDLE);
        :: !hardwareOk -> transition(FAULT_EMERGENCY);
        fi;
    :: (currentState == LOCK_ACQUIRED) ->
        if
        :: !hardwareOk -> transition(FAULT_EMERGENCY);
        :: !targetAcquired -> transition(TRACKING);
        fi;
    :: (currentState == FAULT_EMERGENCY) ->
        /* Terminal Safe State */
        break;
    od
}

init {
    run FlightSystem();
}

/* LTL Formula: Prove that Hardware Fault ALWAYS eventually leads to FAULT_EMERGENCY */
ltl p1 { [] (!hardwareOk -> <> (currentState == FAULT_EMERGENCY)) }
