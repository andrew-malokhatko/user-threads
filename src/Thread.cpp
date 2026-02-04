#include "../include/Thread.hpp"

#include <iostream>

Thread Thread::s_mainThread= Thread(nullptr);
Thread Thread::s_schedulerThread= Thread(nullptr);
Thread* Thread::s_currentThread = &Thread::s_mainThread;

Thread::Thread(void (*func)())
    : m_state(ThreadState::Ready),
    m_stack(std::make_unique<uint8_t[]>(1024 * 4)),
    m_sp{m_stack.get() + STACK_SIZE}
{
    // align stack to 16 bytes (System V ABI)
    m_sp = reinterpret_cast<uint8_t*>((reinterpret_cast<uintptr_t>(m_sp) & ~0xF));

    // TODO: if func is not nullptr change the instruction pointer
}

void Thread::Init()
{
    // TODO: what if Init is accidentally called for the second time?

    s_schedulerThread = Thread(SchedulerFunc);

    // create first thread and set its starting point to the main function
    // Switch();


    // sets up the interrupts
    // runs the scheduler
}

// Callee-saved registers:
// https://math.hws.edu/eck/cs220/f22/registers.html
// For more information:
// https://wiki.osdev.org/CPU_Registers_x86
// Save only Callee-saved registers as caller saved registers are managed by the compiler
extern "C" void thread_switch(Context* from, Context* to)
{
    // rdi = from
    // rsi = to

    // save current threads context
    asm("mov [rdi + 0x00], rbx\n"
        "mov [rdi + 0x08], rbp\n"
        "mov [rdi + 0x10], r12\n"
        "mov [rdi + 0x18], r13\n"
        "mov [rdi + 0x20], r14\n"
        "mov [rdi + 0x28], r15\n"
        "mov [rdi + 0x30], rsp\n"
        "lea rax, [rdi + 0x38]\n" // load ip to rax
        "mov [rdi + 0x38], rax\n"

        // load new context
        "mov rbx, [rsi + 0x00]\n"
        "mov rbp, [rsi + 0x08]\n"
        "mov r12, [rsi + 0x10]\n"
        "mov r13, [rsi + 0x18]\n"
        "mov r14, [rsi + 0x20]\n"
        "mov r15, [rsi + 0x28]\n"
        "mov rsp, [rsi + 0x30]\n"
        "jmp [rsi + 0x38]\n"
        );
}

void Thread::Switch(Context* from, Context* to)
{
    thread_switch(from, to);
}

void Thread::SchedulerFunc()
{
    std::cout << "Running scheduler func\n";
}
