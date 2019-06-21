#ifndef __RS_FAULT_H__
#define __RS_FAULT_H__

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_backtrace()
{
    dprintf(STDERR_FILENO, "=====> backtrace:\n");
    void* array[30];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 30);

    // print out all the frames to stderr
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    dprintf(STDERR_FILENO, "\n");
}

void fault_handler(int sig, siginfo_t *info, void *ucontext)
{
    dprintf(STDERR_FILENO, "\nError: signal %d si_code 0x%x si_addr %p :\n", sig, info->si_code, info->si_addr);
    print_backtrace();
    exit(1);
}

int register_fault_signals()
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = fault_handler;
    if (sigaction(SIGSEGV, &sa, NULL) < 0) {
        perror("sigaction SIGSEGV");
        return -1;
    }

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = fault_handler;
    if (sigaction(SIGABRT, &sa, NULL) < 0) {
        perror("sigaction SIGABRT");
        return -1;
    }
}

#endif // __RS_FAULT_H__