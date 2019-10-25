#include <nlohmann/json.hpp>
#include <fstream>
#include "ThreadQueue.h"
#include "Logger.h"

namespace ffGraph {
namespace JSON {

using json = nlohmann::json;

Geometry ThreadSafeQueue::pop()
{
    std::unique_lock<std::mutex> lock(Mutex);
    while (Queue.empty()) {
        Conditional.wait(lock);
    }
    auto item = Queue.front();
    Queue.pop();
    return item;
}

void ThreadSafeQueue::pop(Geometry& item)
{
    std::unique_lock<std::mutex> lock(Mutex);
    while (Queue.empty()) {
        Conditional.wait(lock);
    }
    item = Queue.front();
    Queue.pop();
}

void ThreadSafeQueue::push(const Geometry& item)
{
    std::unique_lock<std::mutex> lock(Mutex);
    Queue.push(item);
    lock.unlock();
    Conditional.notify_one();
}

void ThreadSafeQueue::push(Geometry&& item)
{
    std::unique_lock<std::mutex> lock(Mutex);
    Queue.push(item);
    lock.unlock();
    Conditional.notify_one();
}

bool ThreadSafeQueue::empty()
{
    std::unique_lock<std::mutex> lock(Mutex);
    bool r = Queue.empty();
    lock.unlock();
    return r;
}

}    // namespace JSON
}    // namespace ffGraph