#ifndef __TIMER_MANAGER_H__
#define __TIMER_MANAGER_H__

#include <stdint.h>
#include <list>
#include <vector>

#include "Timer.h"



typedef std::list<Timer*> TimerList;

void defaultTimerCallback()
{

}

class TimerManager
{
public:
    /**
     * @param slotInterval 时间槽的时间长度
     */
    TimerManager(int64_t slotInterval);
    ~TimerManager() = default;

    /** 添加定时器
     * @param repeatedCount 重复次数
     * @param 触发间隔
     * @
     * @return 返回创建成功的定时器id
     */
    int64_t addTimer(int32_t repeatedCount, int64_t interval, const TimerCallback& timerCallback);

    /** 移除指定id的定时器
     * @param timer 待移除的定时器指针
     * @return 成功移除定时器返回true，反之返回false
     */
    bool removeTimer(Timer* timer);

    /** 检测定时器是否到期，如果到期则触发定时器函数
     */
    void checkAndHandleTimers();


private:
    //时间轮对象
    std::vector<TimerList*>   m_timeWheel;
    //时间槽间隔
    int64_t                   m_slotInterval;
};

#endif //!__TIMER_MANAGER_H__