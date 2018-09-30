#ifndef __PTHREAD_WRAPPER_TEST_H__
#define __PTHREAD_WRAPPER_TEST_H__
#include "../../toolbox/pthread_wrapper.h"
#include <iostream>

class pthread_wrapper_test
{
public:
    void start()
    {
        thread_obj.start_with(this, &pthread_wrapper_test::run);
    }

    int run()
    {
        while (true)
        {
            std::cout << "this is pthread_wrapper_test" << std::endl;
            thread_obj.yield(1000);
        }
        return 0;
    }

private:
    thread_wrapper<pthread_wrapper_test> thread_obj;
};

#endif
