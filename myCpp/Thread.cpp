#include <iostream>
#ifdef _WIN32
//    #include "local/platform_specific/platform_specific/platform_specific.h"
#else
    #include <unistd.h>
#endif
#include "Thread.h"

#ifdef __GNUC__

namespace thread
{

} /* end of namespace */

#define NUM_THREADS 5

extern "C"
{

void  *PrintHello(void *threadID)
{
    long tid;
    tid = (long)threadID;
    std::cout << "Hello! thred[" << tid << "]" << std::endl;
    pthread_exit(NULL);
    return NULL;
}

}

int thread_test(void)
{
    pthread_t threads[NUM_THREADS];
    int rc;
    long t;
    for(t=0; t < NUM_THREADS; t++)
    {
        std::cout << "Main: creating thread [" << t << "]" << std::endl;
        rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
        if(rc)
        {
            std::cerr << "Error: return code from pthread_create() is " << rc << std::endl;
            return -1;
        }

    }
    pthread_exit(NULL);
    return 0;
}

#endif /* __GNUC__ */

#ifdef _MSC_VER

int thread_test(void)
{
    //unsigned res = thread::thread::hardware_concurrency();
    //std::cout << "thread::harware_concurrency: " << res << std::endl;

    return 0;
}
    
#endif
