#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

// 异步写日志队列
template <typename T>
class LockQueue
{
public:
    // 多个worker线程都会入队列
    void Push(const T &data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        m_condvariable.notify_one();
    }


    // 一个专门的线程用来写入日志
    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty())
        {
            // 日志队列为空，线程进入wait
            m_condvariable.wait(lock);
        }

        T data = m_queue.front();
        m_queue.pop();
        return data;
    }

private:
    std::queue<T> m_queue; 
    std::mutex m_mutex;
    std::condition_variable m_condvariable;
};