#include "pthread_wrapper.h"
#include <sys/time.h>

static void* thread_func(void* pParameter)
{
    z_thread *thread = (z_thread *)pParameter;
    thread->execute();
    return NULL;
}

z_thread::z_thread()
{
    m_running = false;
}

bool z_thread::start()
{
    if (!m_running && !pthread_create(&m_thread_id, NULL, thread_func, this))
        return m_running = true;

    return false;
}

bool z_thread::stop()
{
    if (m_running) 
    {
        pthread_cancel(m_thread_id);
        return !(m_running = false);
    }
    return false;
}

void z_thread::yield(unsigned long interval)
{
    struct timeval sleep_interval;
    sleep_interval.tv_sec = 0;
    sleep_interval.tv_usec = interval * 1000;
    select(0, 0, 0, 0, &sleep_interval);
}
