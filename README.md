# ExecutionManager

a C++11 threading library

## Supports

#### Creation
* creating a thread in a running state    `createThread`
* creating a thread in a suspended state      `createThreadSuspended`

#### Signals
* sending signals to a thread               `sendSignal`

#### States
* pausing a thread                          `stopThread`
* resuming a thread                         `continueThread`

#### Waiting
* joining a thread                          `joinThread`
* waiting for thread pause                  `waitForStop`
* waiting for thread resume                 `waitForContinue`
* waiting for thread exit                   `waitForExit`

## Testing

```bash
chmod +x ./untilOutputMatches.sh
make debug && ./untilOutputMatches.sh "internal_error" ./debug_EXECUTABLE/ExecutionManager_tests
```# MemorySocket
