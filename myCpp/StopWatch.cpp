#include <sstream>
#include "StopWatch.h"

StopWatch::Time StopWatch::getTime(void)
{
    unsigned Tmillisec = (unsigned)(1000L*elapsed());
    div_t res;
        
    res = div(Tmillisec, 1000);
    unsigned msec = res.rem;
    unsigned sec = res.quot;

    res = div(sec, 60);
    sec = res.rem;
    unsigned min = res.quot;

    res = div(min, 60);
    min = res.rem;
    unsigned hour = res.quot;

    res = div(hour, 24);
    hour = res.rem;
    unsigned day = res.quot;

    StopWatch::Time tt;
    tt.day=day;
    tt.hour=hour;
    tt.min=min;
    tt.sec=sec;
    tt.msec=msec;
        
    return tt;
}

std::string StopWatch::str(void)
{
    std::stringstream ss;
    StopWatch::Time tt = getTime();
    bool printFlag = false;

    if(tt.day > 0){
        printFlag = true;
        ss << tt.day << " day ";
    }
    if(tt.hour > 0 || printFlag)
    {
        printFlag = true;
        ss << tt.hour << " hr ";
    }
    if(tt.min > 0 || printFlag)
    {
        printFlag = true;
        ss << tt.min << " min ";
    }
    {
        ss << tt.sec+tt.msec*0.001 << " sec";
    }
    return ss.str();
}

