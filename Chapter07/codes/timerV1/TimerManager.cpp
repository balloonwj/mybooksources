#include "TimerManager.h"
    
int64_t TimerManager::addTimer(int32_t repeatedCount, int64_t interval, const TimerCallback& timerCallback)
{
    Timer* pTimer = new Timer(repeatedCount, interval, timerCallback);

    m_listTimers.push_back(pTimer);

    //对定时器对象按过期时间从小到大排序
    m_listTimers.sort(TimerCompare());

    return pTimer->getId();
}

bool TimerManager::removeTimer(int64_t timerId)
{
    for (auto iter = m_listTimers.begin(); iter != m_listTimers.end(); ++iter)
    {
        if ((*iter)->getId() == timerId)
        {
                m_listTimers.erase(iter);
                return true;
        }
    }

    return false;
}

void TimerManager::checkAndHandleTimers()
{
    //遍历过程中是否调整了部分定时器的过期时间
    bool adjusted = false;
    Timer* deletedTimer;   
    for (auto iter = m_listTimers.begin(); iter != m_listTimers.end(); )
    {
        if ((*iter)->isExpired())
        {
            (*iter)->run();
            
            if ((*iter)->getRepeatedTimes() == 0)
            {
                //定时器不需要再触发时从集合中移除该对象
                deletedTimer = *iter;
                iter = m_listTimers.erase(iter);
                delete deletedTimer;
                continue;
            }
            else 
            {
                ++iter;
                //标记下集合中有定时器调整了过期时间
                adjusted = true;
            }
        }
        else
        {
            //找到大于当前系统时间的定时器对象就不需要继续往下检查了，退出循环
            break;
        }// end if      
    }// end for-loop

    //由于调整了部分定时器的过期时间，需要重新排序一下
    if (adjusted)
    {
        m_listTimers.sort(TimerCompare());
    }
}