/** 
 * SharedMutex.h C++11 没有std::shared_mutex, 自己实现一个
 * zhangyl 20191108
 */

#ifndef __SHARED_MUTEX_H__
#define __SHARED_MUTEX_H__

#ifdef WIN32
#include <Windows.h>
#else
#include 
#include <pthread.h>
#endif

class SharedMutex final
{
public:
    SharedMutex();
    ~SharedMutex();

    void acquireReadLock();
    void acquireWriteLock();
    void unlockReadLock();
    void unlockWriteLock();

private:
    SharedMutex(const SharedMutex& rhs) = delete;
    SharedMutex& operator =(const SharedMutex& rhs) = delete;

private:
#ifdef WIN32
    SRWLOCK             m_SRWLock;
#else
    pthread_rwlock_t    m_SRWLock;
#endif   
};

class SharedLockGuard final
{
public:
    SharedLockGuard(SharedMutex& sharedMutex);
    ~SharedLockGuard();

private:
    SharedLockGuard(const SharedLockGuard& rhs) = delete;
    SharedLockGuard operator=(const SharedLockGuard& rhs) = delete;

private:
    SharedMutex&        m_SharedMutex;
};

class UniqueLockGuard final
{
public:
    UniqueLockGuard(SharedMutex& sharedMutex);
    ~UniqueLockGuard();

private:
    UniqueLockGuard(const UniqueLockGuard& rhs) = delete;
    UniqueLockGuard operator=(const UniqueLockGuard& rhs) = delete;

private:
    SharedMutex& m_SharedMutex;
};


#endif //!__SHARED_MUTEX_H__
