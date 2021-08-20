#pragma once
#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

namespace mini
{
template <typename T>
class BlockingQueue
{
 public:
  BlockingQueue() = delete;
  BlockingQueue(const BlockingQueue&) = delete;
  BlockingQueue(BlockingQueue&&) = delete;
  BlockingQueue& operator=(const BlockingQueue&) = delete;
  BlockingQueue& operator=(BlockingQueue&&) = delete;

  BlockingQueue(size_t max_size, size_t nb_producers)
      : m_max_size(max_size), m_nb_producers(nb_producers)
  {
    m_fp.resize(m_nb_producers);
  }

  void push(T&& e)
  {
    {
      std::unique_lock<std::mutex> lock(m_queue_mutex);
      m_full_condition.wait(lock, [this] { return !(this->m_queue.size() == this->m_max_size); });
      m_queue.push(std::move(e));
    }
    m_empty_condition.notify_all();
  }

  bool pop(T& e)
  {
    {
      std::unique_lock<std::mutex> lock(m_queue_mutex);
      m_empty_condition.wait(lock, [this] {
        return !this->m_queue.empty() ||
               std::all_of(m_fp.begin(), m_fp.end(), [](bool b) { return b; });
      });
      if (std::all_of(m_fp.begin(), m_fp.end(), [](bool b) { return b; }) && m_queue.empty())
        return false;
      e = std::move(m_queue.front());
      m_queue.pop();
    }
    m_full_condition.notify_one();
    return true;
  }

  void end_signal()
  {
    {
      std::unique_lock<std::mutex> lock(m_queue_mutex);
      for (size_t i=0; i<m_fp.size(); i++) m_fp[i] = true;
      m_finish = true;
    }
    m_empty_condition.notify_all();
    m_full_condition.notify_all();
  }

  void end_signal(int i)
  {
    m_fp[i] = true;
    {
      if (std::all_of(m_fp.begin(), m_fp.end(), [](bool b) {return b;}))
      {
        end_signal();
      }
    }
  }

 private:
  size_t m_max_size{0};
  size_t m_nb_producers{0};
  bool m_finish{false};

  std::queue<T> m_queue;
  std::mutex m_queue_mutex;
  std::condition_variable m_empty_condition;
  std::condition_variable m_full_condition;
  std::vector<bool> m_fp;
};

};  // namespace kmdiff