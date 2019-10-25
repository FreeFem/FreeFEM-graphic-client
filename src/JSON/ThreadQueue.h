#ifndef THREAD_QUEUE_H_
#define THREAD_QUEUE_H_

#include <vector>
#include <string>
#include <cstdint>
#include <ctpl.h>
#include <queue>
#include <mutex>
#include "Array.h"
#include "Mesh.h"
#include "LabelTable.h"

namespace ffGraph {
namespace JSON {

class ThreadSafeQueue {
    public:
        Geometry pop();
        void pop(Geometry& item);
        void push(const Geometry& item);
        void push(Geometry&& item);

        bool empty();

        ThreadSafeQueue(const ThreadSafeQueue&) = delete;
        ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

        ThreadSafeQueue() {};

    private:

        std::queue<Geometry> Queue;
        std::mutex Mutex;
        std::condition_variable Conditional;
};

}    // namespace JSON
}    // namespace ffGraph

#endif    // THREAD_QUEUE_H_