/**
 * SharedMutex.cpp
 * zhangyl 20191108
 */

#include "SharedMutex.h"

SharedMutex::SharedMutex()
{
#ifdef WIN32
    ::InitializeSRWLock(&m_SRWLock);
#else
    ::pthread_rwlock_init(&m_SRWLock, nullptr);
#endif   
}

SharedMutex::~SharedMutex()
{
#ifdef WIN32
    //Windows上读写锁不需要显式销毁
#else
    ::pthread_rwlock_destroy(&m_SRWLock);
#endif
}

void SharedMutex::acquireReadLock()
{
#ifdef WIN32
    ::AcquireSRWLockShared(&m_SRWLock);
#else
    ::pthread_rwlock_rdlock(&m_SRWLock);
#endif   
}

void SharedMutex::acquireWriteLock()
{
#ifdef WIN32
    ::AcquireSRWLockExclusive(&m_SRWLock);
#else
    ::pthread_rwlock_wrlock(&m_SRWLock);
#endif   
}

void SharedMutex::unlockReadLock()
{
#ifdef WIN32   
    ::ReleaseSRWLockShared(&m_SRWLock);   
#else
    ::pthread_rwlock_unlock(&m_SRWLock);
#endif
}

void SharedMutex::unlockWriteLock()
{
#ifdef WIN32  
    ::ReleaseSRWLockExclusive(&m_SRWLock);
#else
    ::pthread_rwlock_unlock(&m_SRWLock);
#endif
}

SharedLockGuard::SharedLockGuard(SharedMutex& sharedMutex) :
    m_SharedMutex(sharedMutex)
{
    m_SharedMutex.acquireReadLock();
}

SharedLockGuard::~SharedLockGuard()
{
    m_SharedMutex.unlockReadLock();
}

UniqueLockGuard::UniqueLockGuard(SharedMutex& sharedMutex) :
    m_SharedMutex(sharedMutex)
{
    m_SharedMutex.acquireWriteLock();
}

UniqueLockGuard::~UniqueLockGuard()
{
    m_SharedMutex.unlockWriteLock();
}
