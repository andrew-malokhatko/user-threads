#include "TCB.hpp"

#include <iostream>
#include <ucontext.h>
#include <list>

namespace uthread
{

// state globals accessible with extern
namespace state
{
    extern std::list<TCB> tcbs;

    extern TCB* current;
    extern TCB* prev;

}

// TCB static variables
id_t TCB::s_idCounter = 0;

TCB::TCB(void (*func)(), ThreadState state, TCB* cleanup)
    : state(state),
    stack(std::make_unique<uint8_t[]>(STACK_SIZE)),
    m_id(s_idCounter++)
{
    // create new context for this thread
    getcontext(&context);

    context.uc_stack.ss_sp = stack.get();
    context.uc_stack.ss_size = STACK_SIZE;
    context.uc_stack.ss_flags = 0;

    context.uc_link = &cleanup->context;

    makecontext(&context, reinterpret_cast<void(*)()>(func), 0);
}

id_t TCB::getId() const
{
    return m_id;
}

void TCB::Switch(TCB* dest)
{
    std::cout << "switching from tid: " << state::current->m_id << " to " << dest->m_id << "\n";

    // update current and prev threads to be valid after switch
    state::prev = state::current;
    state::current = dest;

    // swap this thread saved in prev and dest
    swapcontext(&(state::prev->context), &dest->context);
}

bool TCB::operator==(const TCB& rhs) const
{
    return m_id == rhs.m_id;
}


}