#ifndef THREAD_QUEUE_H_
#define THREAD_QUEUE_H_

#include <queue>
#include <condition_variable>
#include <mutex>
#include "Geometry.h"
#include "LabelTable.h"

namespace ffGraph {
namespace JSON {

class ThreadSafeQueue {
    public:
        ConstructedGeometry pop();
        void pop(ConstructedGeometry& item);
        void push(const ConstructedGeometry& item);
        void push(ConstructedGeometry&& item);

        bool empty();
        size_t size();

        // delete copy constructor
        ThreadSafeQueue(const ThreadSafeQueue&) = delete;
        ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

        ThreadSafeQueue() {};

    private:

        std::queue<ConstructedGeometry> Queue;
        std::mutex Mutex;
        std::condition_variable Conditional;
};

}    // namespace JSON
}    // namespace ffGraph

#endif    // THREAD_QUEUE_H_