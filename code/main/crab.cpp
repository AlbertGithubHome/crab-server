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
        while (true)
        {
            cout << "this is thread" << endl;
            m.yield(1000);
        }
        return 0;
    }

private:
    thread_wrapper<a> m;
};



int main(int argc, char **argv)
{
    a a1;
    a1.start();

    cout << "test ok\n";
    getchar();
    return 0;
}
