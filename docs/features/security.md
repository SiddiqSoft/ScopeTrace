# Security & Exception Model

## Exception Safety

`siddiqsoft::scopelog::~scopelog()` is declared `noexcept`.
Any exceptions thrown inside user-defined callback functions are caught safely inside the destructor to prevent `std::terminate()` from being invoked during stack unwinding.

## Thread Safety

- **Thread-Local Scope Depth**: Nesting depth relies on `thread_local` state, requiring zero lock contention between threads.
- **Global Callback Protection**: Setting and dispatching global callbacks is protected via `std::mutex`.
