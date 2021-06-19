#include "TimerManager.h"
    
int64_t TimerManager::addTimer(int32_t repeatedCount, int64_t interval, const TimerCallback& timerCallback)
{
    Timer* pTimer = new Timer(repeatedCount, interval, timerCallback);
    int64_t timerId = pTimer->getId();

    //插入时会自动按TimerCompare对象进行排序
    m_mapTimers[timerId] = pTimer;

    return timerId;
}

bool TimerManager::removeTimer(int64_t timerId)
{
    auto iter = m_mapTimers.find(timerId);
    if (iter != m_mapTimers.end())
    {
        m_mapTimers.erase(iter);
        return true;
    }

    return false;
}

void TimerManager::checkAndHandleTimers()
{
    //遍历过程中是否调整了部分定时器的过期时间
    bool adjusted = false;
    Timer* deletedTimer;
    for (auto iter = m_mapTimers.begin(); iter != m_mapTimers.end(); )
    {
        if (iter->second->isExpired())
        {
            iter->second->run();
            
            if (iter->second->getRepeatedTimes() == 0)
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
        }   
    }

    //由于调整了部分定时器的过期时间，需要重新排序一下
    if (adjusted)
    {
        std::map<int64_t, Timer*, TimerCompare> localMapTimers;    
        for (const auto& iter : m_mapTimers)
        {
            localMapTimers[iter.first] = iter.second;
        }

        m_mapTimers.clear();
        m_mapTimers.swap(localMapTimers);
    }
}