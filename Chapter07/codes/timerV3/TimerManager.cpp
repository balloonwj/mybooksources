#include "TimerManager.h"

TimerManager::TimerManager(int64_t slotInterval)
{
    m_slotInterval = slotInterval;
}
    
int64_t TimerManager::addTimer(int32_t repeatedCount, int64_t interval, const TimerCallback& timerCallback)
{
    int64_t now = (int64_t)time(nullptr);
    
    //计算位于哪个槽中
    int slotIndex = interval % m_slotInterval;

    Timer* pTimer = new Timer(repeatedCount, interval, now + interval, timerCallback);
    int64_t timerId = pTimer->getId();
    
    TimerList* pTimerList;
    if (m_timeWheel.empty())
    {      
        //时间轮是空的
        pTimerList = new TimerList();
        if (slotIndex == 0)
        {
            //第一个元素         
            m_timeWheel.push_back(pTimerList);
        }
        else
        {
            //不是第一个元素
             m_timeWheel.resize(slotIndex + 1);
             //在第slotIndex个槽中填充
             m_timeWheel[slotIndex] = pTimerList;
        }       
    } 
    else if (m_timeWheel.size() > slotIndex + 1)
    {
        //时间轮不是空的，槽已经存在，但是链表不存在
        pTimerList = m_timeWheel[slotIndex];
        if (pTimerList == nullptr)
        {
            pTimerList = new TimerList();
            m_timeWheel[slotIndex] = pTimerList;
        }
    }
    else 
    {
        //m_timeWheel.size() <= slotIndex + 1
        //时间轮不是空的，但槽不存在
        //扩展时间轮
        std::vector<TimerList*> tmpTimeWheel = m_timeWheel;
        m_timeWheel.clear();
        //在尾部多扩展8个槽，减少下次扩展槽内存复制的几率
        m_timeWheel.resize(slotIndex + 1 + 8);
        int32_t tmpTimeWheelSize = tmpTimeWheel.size();
        for (int32_t i = 0; i < tmpTimeWheelSize; ++i)
        {
            m_timeWheel[i] = tmpTimeWheel[i];
        }

        //补上扩展的槽
        pTimerList = new TimerList();
        m_timeWheel[slotIndex] = pTimerList;
    }

    pTimerList->push_back(pTimer);

    return timerId;
}

bool TimerManager::removeTimer(Timer* timer)
{
    int32_t slotIndex = timer->getInterval() / m_slotInterval;
    if (m_timeWheel.size() < slotIndex + 1)
        return false;

    TimerList* pTimerList = m_timeWheel[slotIndex];
    if (pTimerList == nullptr)
        return false;

    for (auto iter = pTimerList->begin(); iter != pTimerList->end(); ++iter)
    {
        pTimerList->erase(iter);
        return true;
    }        

    return false;
}

void TimerManager::checkAndHandleTimers()
{
    //先计算当前时间位于哪个槽中
    int64_t now = (int64_t)time(nullptr);
    
    //取第一个槽的起始时间
    if (m_timeWheel.empty())
        return;

    TimerList* pTimeList = m_timeWheel[0];
    if (pTimeList == nullptr)
        return;

        

}