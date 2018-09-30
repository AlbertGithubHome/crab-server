#include "../toolbox/pthread_wrapper.h"
#include "testcase/pthread_wrapper_test.h"
#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    pthread_wrapper_test thread_test1;
    thread_test1.start();

    cout << "test ok\n";
    getchar();
    return 0;
}
