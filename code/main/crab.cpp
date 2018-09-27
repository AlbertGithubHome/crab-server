#include "../toolbox/pthread_wrapper.h"
#include <iostream>
using namespace std;


class a
{
public:
    void start()
    {
        m.start_with(this, &a::run);
    }

    int run()
    {
        cout << "this is thread" << endl;
        m.yeild(1);
    }

private:
    thread_wrapper<a> m;
};



int main(int argc, char **argv)
{
    a a1;
    a1.start();

    cout << "test ok" << endl;
    getchar();
    return 0;
}
