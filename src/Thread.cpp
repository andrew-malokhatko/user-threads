#include "../include/Thread.hpp"

#include <cassert>
#include <iostream>

// TODO: remove default constructor
Timer Thread::s_timer = Timer();

// Optionally cleanup
tid_t Thread::s_idCounter = 0;
std::vector<Thread*> Thread::s_threads{};
Thread* Thread::s_currentThread = nullptr;
Thread Thread::s_mainThread = Thread(nullptr);


Thread::Thread(void (*func)())
    : m_tid(s_idCounter++),
    m_state(ThreadState::Ready),
    m_stack(std::make_unique<uint8_t[]>(STACK_SIZE)),
    m_sp{m_stack.get() + STACK_SIZE},
    m_ip{reinterpret_cast<void*>(func)}
{
    // align stack to 16 bytes (System V ABI)
    m_sp = reinterpret_cast<uint8_t*>((reinterpret_cast<uintptr_t>(m_sp) & ~0xF));

    s_threads.push_back(this);

    m_context.rip = m_ip;
    m_context.rsp = m_sp;
}


tid_t Thread::gettid() const
{
    return m_tid;
}

void Thread::Init()
{
    // TODO: what if Init is accidentally called for the second time?

    std::cout << "Tread init. \n";

    // s_mainThread = Thread(nullptr); // Double initialization (if uncomment)!!!!
    s_currentThread = &s_mainThread;

    // create first thread and set its starting point to the main function
    // Switch();

    // set up the interrupts
    s_timer = Timer(SchedulerFunc, 150, 150);

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
        "lea rax, [rip + 0f]\n" // load ip to rax
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

        "0:\n"
        "ret\n"
        );
}

void Thread::Switch(Thread* from, Thread* to)
{
    thread_switch(&(from->m_context), &(to->m_context));
}

void Thread::SchedulerFunc(int signal)
{
    std::cout << "Running scheduler func\n";
    std::cout << "Received signal: " << signal << "\n";

    size_t candidate = -1;
    for (size_t i = 0; i < s_threads.size(); ++i)
    {
        const auto& thread = s_threads[i];

        // find first thread that is not this thread
        if (s_currentThread->m_tid != thread->m_tid)
        {
            candidate = i;
            break;
        }
    }

    // switch only if the other thread exists
    if (candidate != -1)
    {
        std::cout << "context switching from tid: " << s_currentThread->m_tid << " to " << s_threads[candidate]->m_tid << "\n";
        Switch(s_currentThread, s_threads[candidate]);
    }
}
