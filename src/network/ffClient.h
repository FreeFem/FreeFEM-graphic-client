#ifndef FF_CLIENT_H_
#define FF_CLIENT_H_

#include <asio/buffer.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read_until.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>
#include <asio/steady_timer.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <deque>

namespace ffGraph {

using asio::steady_timer;
using asio::ip::tcp;

class ffClient {
    public:
        ffClient(std::string Host,  std::string Port, std::shared_ptr<std::deque<std::string>>&);

        void Start();

        void Stop();
    private:
        void StartConnect(tcp::resolver::results_type::iterator);
        void HandleConnect(const std::error_code&, tcp::resolver::results_type::iterator);
        void StartRead();
        void HandleRead(const std::error_code&, std::size_t);
        void StartWrite();
        void HandleWrite(const std::error_code&);
        void CheckDeadline();

        bool Stopped = false;
        bool Updated = false;
        asio::io_context IoContext;
        tcp::resolver Resolver;
        tcp::resolver::results_type Endpoints;
        tcp::socket Socket;
        std::string InputBuffer;
        steady_timer Deadline;
        steady_timer HeartBeat;
        std::shared_ptr<std::deque<std::string>> SharedDataQueue;
};

}

#endif // FF_CLIENT_H_