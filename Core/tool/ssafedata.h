#ifndef SSAFEDATA_H
#define SSAFEDATA_H

#include "core_global.h"

template<class T>
/**
 * @brief   SSafeData can be used to protects any object.
 *
 */
class SSafeData: public QReadWriteLock{
public:
    T m_Data; /**< Object */
    /**
     * @brief   Override the object's pointer operation.
     *
     * @return T *operator ->
     */
    T* operator ->(void)
    {
        return &m_Data;
    }
};

template<class T>
/**
 * @brief SSafePtrData can be used to protects any pointer.
 *
 */
class SSafePtrData: public QReadWriteLock{
public:
    // 数据
    T m_Data = NULL; /**< Pointer */
    /**
     * @brief   Override the pointer's pointer operation.
     *
     * @return T &operator ->
     */
    T& operator ->(void)
    {
        return m_Data;
    }
};

#endif // SSAFEDATA_H
