#include <nlohmann/json.hpp>
#include <fstream>
#include "ThreadQueue.h"
#include "Logger.h"

namespace ffGraph {
namespace JSON {

using json = nlohmann::json;

ConstructedGeometry ThreadSafeQueue::pop()
{
    std::unique_lock<std::mutex> lock(Mutex);
    while (Queue.empty()) {
        Conditional.wait(lock);
    }
    auto item = Queue.front();
    Queue.pop();
    return item;
}

void ThreadSafeQueue::pop(ConstructedGeometry& item)
{
    std::unique_lock<std::mutex> lock(Mutex);
    while (Queue.empty()) {
        Conditional.wait(lock);
    }
    item = Queue.front();
    Queue.pop();
}

void ThreadSafeQueue::push(const ConstructedGeometry& item)
{
    std::unique_lock<std::mutex> lock(Mutex);
    Queue.push(item);
    lock.unlock();
    Conditional.notify_one();
}

void ThreadSafeQueue::push(ConstructedGeometry&& item)
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

size_t ThreadSafeQueue::size()
{
    std::unique_lock<std::mutex> lock(Mutex);
    size_t r = Queue.size();
    lock.unlock();
    return r;
}

}    // namespace JSON
}    // namespace ffGraph