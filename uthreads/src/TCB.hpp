#pragma once

#include <memory>
#include <sys/ucontext.h>

namespace uthread
{

enum class ThreadState
{
    Running,
    Ready,
    Finished,

    Daemon,
};

struct TCB
{
    TCB(void (*func)(), ThreadState state, TCB* cleanup);

    static void Switch(TCB* dest);

    bool operator==(const TCB& rhs) const;

    [[nodiscard]] id_t getId() const;

    ThreadState state;
    bool joinable = true;

private:
    // id
    id_t m_id;

    // context (contains ip and sp)
    ucontext_t context {};

    // stack data
    std::unique_ptr<uint8_t[]> stack;

    static constexpr size_t STACK_SIZE = 1 << 20; // 1 MB
    static id_t s_idCounter;
};

static_assert(std::is_move_constructible<TCB>());
static_assert(std::is_move_assignable<TCB>());
static_assert(std::is_copy_constructible<TCB>() == false);
static_assert(std::is_copy_assignable<TCB>() == false);

}
