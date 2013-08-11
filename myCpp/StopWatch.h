#ifndef  __STOP_WATCH_H__
#define  __STOP_WATCH_H__

#include <string>
#include <ctime>

//******************************
// Stop watch
//******************************

class StopWatch
{
public:
    struct Time
    {
        unsigned msec;
        unsigned sec;
        unsigned min;
        unsigned hour;
        unsigned day;
    };

    void start(void) { _start = clock();}

    void stop(void) { _stop = clock();}

    double elapsed(void) {return (_stop-_start)*1.0/CLOCKS_PER_SEC; }

    Time getTime(void);

    std::string str(void);

private:
    clock_t _start;
    clock_t _stop;

};

#endif /* __STOP_WATCH_H__ */

