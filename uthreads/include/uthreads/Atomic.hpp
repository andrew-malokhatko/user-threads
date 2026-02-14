#pragma once

#include <type_traits>

namespace uthread
{

template <typename T>
requires (std::is_integral_v<T> || std::is_pointer_v<T>)
class Atomic
{
public:
    Atomic() noexcept = default;
    Atomic(T value) noexcept : m_value(value) {};

    Atomic(const Atomic&) = delete;
    Atomic& operator=(const Atomic&) = delete;

    T load() const noexcept
    {
        return __atomic_load_n(&m_value, __ATOMIC_SEQ_CST);
    }

    void store(T value) noexcept
    {
        __atomic_store_n(&m_value, value, __ATOMIC_SEQ_CST);
    }

    T exchange(T value) noexcept
    {
        return __atomic_exchange_n(&m_value, value, __ATOMIC_SEQ_CST);
    }

    T fetchAdd(T value) noexcept
    {
        return __atomic_fetch_add(&m_value, value, __ATOMIC_SEQ_CST);
    }

    bool compareAndExchange(T& expected, T desired) noexcept
    {
        return __atomic_compare_exchange_n(
            &m_value,
            &expected,
            desired,
            false,
            __ATOMIC_SEQ_CST,
            __ATOMIC_SEQ_CST
        );
    }

    void operator=(T value) noexcept
    {
        store(value);
    }

    operator T() const noexcept
    {
        return load();
    }

private:
    alignas(sizeof(T)) T m_value;
};

}