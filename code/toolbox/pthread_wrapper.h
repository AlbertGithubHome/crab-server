#pragma once

#include <pthread.h>
class z_thread
{
public:
    z_thread();
    virtual ~z_thread() {}

    bool start();
    bool stop();
    virtual void execute() = 0;
    virtual void yield(unsigned long interval = 1);
private:
    pthread_t   m_thread_id;
    bool        m_running;
};

template<class host>
class thread_wrapper : z_thread
{
    typedef int (host::*func_type)();
public:
    thread_wrapper() : m_host(NULL), m_thread_func(NULL) {}
    ~thread_wrapper(){}

    bool start_with(host* p_host, func_type p_thread_func)
    {
        m_host = p_host;
        m_thread_func = p_thread_func;
        return start();
    }

    void execute()
    {
        if (m_host != NULL && m_thread_func != NULL)
            (m_host->*m_thread_func)();
    }
private:
    host*	    m_host;
    func_type   m_thread_func;
};

