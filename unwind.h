#include <execinfo.h>
#include <unistd.h>
#include <ucontext.h>
#include <backtrace.h>

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include <iostream>
#include <print>
#include <vector>

#include <cxxabi.h>
#include <dlfcn.h>

static void backtrace_error_cb(void*, const char* msg, int errnum) {
    std::println(stderr, "### Error {}: {}", errnum, msg);
}

static backtrace_state* backtrace_state_instance() {
    static backtrace_state* state = backtrace_create_state(
        nullptr,
        1,
        backtrace_error_cb,
        nullptr);

    return state;
}

static int print_frame_cb(
    void*,
    uintptr_t,
    const char* filename,
    int line,
    const char* function
) {
    int status = 0;
    char* demangled = abi::__cxa_demangle(
        function,
        nullptr,
        nullptr,
        &status);

    const char* demangled_name = status == 0 ? demangled : function;

    std::println("=== {}:{} {}",
        filename ? filename : "??",
        line,
        demangled_name ? demangled_name : "??");

    free(demangled);

    return 0;
}

static void segfault_handler(int sig, siginfo_t*, void*) {
    std::vector<void*> frames(1024);

    size_t count = 0;
    while (true) {
        int n = backtrace(frames.data(), frames.size());

        // all frames fit the vector, breaking out
        if (static_cast<size_t>(n) < frames.size()) {
            count += n;
            break;
        }

        // resize vector if not enough capacity to hold all frames
        frames.resize(frames.size() * 2);
    }

    for (size_t i{}; i < count; i++) {
        backtrace_pcinfo(
            backtrace_state_instance(),
            reinterpret_cast<uintptr_t>(frames[i]),
            print_frame_cb,
            backtrace_error_cb,
            nullptr);
    }

    std::_Exit(128 + sig);
}

struct SigHandlerInstaller {
    SigHandlerInstaller() {
        struct sigaction sa{};

        sa.sa_sigaction = segfault_handler;
        sigemptyset(&sa.sa_mask);

        sa.sa_flags = SA_SIGINFO;

        sigaction(SIGSEGV, &sa, nullptr);
    }
};

static SigHandlerInstaller sa_installer;
