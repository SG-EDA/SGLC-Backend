#pragma once

template<typename T>
class optional
{
public:
    optional() : m_hasInit(false) {}
    optional(const T& v) : m_hasInit(true), m_data(v) {}

    optional(const optional& other) : m_hasInit(other.has_value())
    {
        if (other.has_value())
            this->m_data=other.m_data;
    }

    bool has_value() const { return m_hasInit; }
    T value() { return m_data; }

private:
    bool m_hasInit;
    T m_data;
};
