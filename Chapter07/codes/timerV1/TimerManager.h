#ifndef __TIMER_MANAGER_H__
#define __TIMER_MANAGER_H__

#include <stdint.h>
#include <list>

#include "Timer.h"

//std::list<Timer*> m_listTimers;

// void EventLoop::check_and_handle_timers()
// {
//     for (auto& timer : m_listTimers)
//     {
//         if (timer->isExpired())
//         {
//             timer->run();
//         }
//     }
// }


struct TimerCompare  
{  
    bool operator () (const Timer* lhs, const Timer* rhs)  
    {  
        return lhs->getExpiredTime() <  rhs->getExpiredTime();
    }
}; 

void defaultTimerCallback()
{

}

class TimerManager
{
public:
    TimerManager() = default;
    ~TimerManager() = default;

    /** 添加定时器
     * @param repeatedCount 重复次数
     * @param 触发间隔
     * @
     * @return 返回创建成功的定时器id
     */
    int64_t addTimer(int32_t repeatedCount, int64_t interval, const TimerCallback& timerCallback);

    /** 移除指定id的定时器
     * @param timerId 待移除的定时器id
     * @return 成功移除定时器返回true，反之返回false
     */
    bool removeTimer(int64_t timerId);

    /** 检测定时器是否到期，如果到期则触发定时器函数
     */
    void checkAndHandleTimers();


private:
    std::list<Timer*> m_listTimers;

};

#endif //!__TIMER_MANAGER_H__