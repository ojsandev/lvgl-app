# Asynchronous task architecture

## Goals

- Never wait for network, file I/O, or image processing on the LVGL thread.
- Keep callbacks out of MainViewModel and use-case public APIs.
- Keep the common task contract usable by desktop and ESP-IDF builds.
- Do not modify the external aptumfr/lv wrapper.

## Task contract

common/async/Task.h provides Task<T> and TaskCompletionSource<T>. A task exposes non-blocking readiness checks and a result/error that may only be read once the task is ready. The completion source is used by callback-driven infrastructure adapters; those callbacks are an implementation detail and are not exposed by the ViewModel.

When the compiler enables C++20 coroutine support, Task<T> also provides operator co_await and a coroutine promise type. The continuation is resumed by the thread that completes the awaited task. Desktop HTTP completions occur on the HTTP worker pool, so the character use case resumes off the UI thread.

A task currently supports one awaiting coroutine and does not implement cancellation. Callers must keep the objects used by an in-flight coroutine alive until the task completes.

## Desktop flow

1. MainViewModel::update/nextPage/previousPage starts a Task<CharacterView> and returns immediately.
2. GetCharactersUseCase::execute uses co_await to await the character response and each required image download.
3. Image storage and thumbnail creation run after an asynchronous HTTP completion, on a worker thread.
4. The LVGL timer polls MainViewModel::poll() and calls render() only when a task has completed. No LVGL widgets are accessed from worker threads.

The desktop build already selects C++26 on GCC/Clang and C++20 mode on MSVC, so coroutine syntax is available in the application target without changing the wrapper.

## ESP-IDF flow

Task<T> is deliberately usable without coroutine support: the readiness/result/completion-source API is standard C++17. The firmware's current DemoUseCases implementation returns already-completed tasks and does not compile the desktop GetCharactersUseCase.cpp implementation, as documented in domain/CMakeLists.txt.

This keeps the UI contract compatible with the separate ESP-IDF toolchain. It does not add live HTTP requests to the firmware. When the ESP32 data path is implemented, use a persistent FreeRTOS worker and esp_http_client behind the same repository/task contract; complete tasks from that worker and keep LVGL work in the LVGL thread. Do not add Boost.Beast, desktop worker-pool code, or coroutine syntax to the ESP-IDF-only path unless its exact toolchain configuration explicitly enables it.

## Validation

- TaskCxx17CompatibilityTest checks the non-coroutine task contract under C++17.
- AsyncRepositoryFutureTest.cpp now checks deferred repository completion and coroutine resumption in the desktop test dialect.
- The full desktop build and an ESP-IDF firmware build still need to run in their respective configured environments; remote repository editing does not itself prove either build passes.
