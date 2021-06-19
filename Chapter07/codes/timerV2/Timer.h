#ifndef __TIMER_H__
#define __TIMER_H__

#include <functional>

typedef std::function<void()> TimerCallback;

class Timer
{
public:
    /**
     * @param repeatedTimes 定时器重复次数，设置为-1表示一直重复下去
     * @param interval      下一次触发的时间间隔
     * @param timerCallback 定时器触发后的回调函数
     */
    Timer(int32_t repeatedTimes, int64_t interval, const TimerCallback& timerCallback);
    ~Timer();

    int64_t getId() const
    {
        return m_id;
    }

    int64_t getExpiredTime() const
    {
        return m_expiredTime;
    }

    bool isExpired();

    int32_t getRepeatedTimes() const
    {
        return m_repeatedTimes;
    }

    void run();

    //其他实现暂且省略

private:
    //定时器的id，唯一标识一个定时器
    int64_t                     m_id;
    //定时器的到期时间
    time_t                      m_expiredTime;
    //定时器重复触发的次数
    int32_t                     m_repeatedTimes;
    //定时器触发后的回调函数
    TimerCallback               m_callback;
    //触发时间间隔                
    int64_t                     m_interval;

    //定时器id基准值
    static std::atomic<int>     s_initialId;
};


#endif //!__TIMER_H__