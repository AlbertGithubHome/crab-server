#ifndef __THREAD_WRAPPER_H__
#define __THREAD_WRAPPER_H__
#include <functional>
using namespace std;

/*
功能：std::thread包装类
参数：宿主类的类名
说明：使用线程时，定义一个与宿主类相关的线程对象，线程启动时可以调用宿主类的成员函数
*/

template<class host>
class thread_wrapper
{
    

};

#endif //__THREAD_WRAPPER_H__