#ifndef __PTHREAD_WRAPPER_H__
#define __PTHREAD_WRAPPER_H__
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

/*
功能：pthread的包装类
参数：宿主类的类名
说明：使用线程时，定义一个与宿主类相关的线程对象，线程启动时可以调用宿主类的成员函数
*/

template<class host>
class pthread_wrapper : public z_thread
{
    typedef int (host::*func_type)();
public:
    pthread_wrapper() : m_host(NULL), m_thread_func(NULL) {}
    ~pthread_wrapper() {}

    bool start_with(host* p_host, func_type p_thread_func)
    {
        m_host = p_host;
        m_thread_func = p_thread_func;
        return start();
    }

    void execute()
    {
        (m_host != NULL && m_thread_func != NULL) ? (m_host->*m_thread_func)() : 0;
    }
private:
    host*       m_host;
    func_type   m_thread_func;
};

#endif //__PTHREAD_WRAPPER_H__