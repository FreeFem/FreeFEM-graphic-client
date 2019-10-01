#include "ffClient.h"
#include <nlohmann/json.hpp>

namespace ffGraph
{

    ffClient::ffClient(std::string Host, std::string Port, std::shared_ptr<std::deque<std::string>>& SharedQueue)
    : Resolver(IoContext), Socket(IoContext), Deadline(IoContext), HeartBeat(IoContext), SharedDataQueue(SharedQueue)
    {
        Endpoints = Resolver.resolve(Host, Port);
    }

    void ffClient::Start()
    {
        StartConnect(Endpoints.begin());
        Deadline.async_wait(std::bind(&ffClient::CheckDeadline, this));
        IoContext.run();
    }

    void ffClient::Stop()
    {
        Stopped = true;
        std::error_code ignored_error;
        Socket.close(ignored_error);
        Deadline.cancel();
        HeartBeat.cancel();
        IoContext.stop();
    }

    void ffClient::StartConnect(tcp::resolver::results_type::iterator EndP_ITE)
    {
        if (EndP_ITE != Endpoints.end()) {
            Deadline.expires_after(std::chrono::seconds(60));
            Socket.async_connect(EndP_ITE->endpoint(), std::bind(&ffClient::HandleConnect, this, std::placeholders::_1, EndP_ITE));
        } else {
            Stop();
        }
    }

    void ffClient::HandleConnect(const std::error_code& Error, tcp::resolver::results_type::iterator EndP_ITE)
    {
        if (Stopped)
            return;
        if (!Socket.is_open()) {
            StartConnect(++EndP_ITE);
        } else if (Error) {
            Socket.close();
            StartConnect(++EndP_ITE);
        } else {
            std::cout << "Connected !\n";
            StartRead();
            StartWrite();
        }
    }

    void ffClient::StartRead()
    {
        //Deadline.expires_after(std::chrono::seconds(30));
        asio::async_read(Socket, asio::dynamic_buffer(InputBuffer, 64), std::bind(&ffClient::HandleRead, this, std::placeholders::_1, std::placeholders::_2));
    }

    void ffClient::HandleRead(const std::error_code& Error, std::size_t n)
    {
        using json = nlohmann::json;

        if (Stopped)
            return;
        if (!Error) {
            std::string line(InputBuffer.substr(0, n - 1));
            InputBuffer.erase(0, n);
            if (!line.empty()) {
                auto j = json::parse(line);
                size_t ReadSize = j["Size"];

                std::error_code err;
                asio::read(Socket, asio::dynamic_buffer(InputBuffer, ReadSize), err);
                if (err)
                    StartRead();
                else {
                    SharedDataQueue->push_back(InputBuffer.substr(0, ReadSize));
                    InputBuffer.erase(0, ReadSize);
                }
            }
            StartRead();
        } else {
            Stop();
        }
    }

    void ffClient::StartWrite()
    {
        asio::async_write(Socket, asio::buffer("\n", 1), std::bind(&ffClient::HandleWrite, this, std::placeholders::_1));
    }

    void ffClient::HandleWrite(const std::error_code& Error)
    {
        if (Stopped)
            return;
        if (!Error) {
            HeartBeat.expires_after(std::chrono::seconds(10));
            HeartBeat.async_wait(std::bind(&ffClient::StartWrite, this));
        } else {
            Stop();
        }
    }

    void ffClient::CheckDeadline()
    {
        if (Stopped)
            return;
        if (Deadline.expiry() <= steady_timer::clock_type::now()) {
            Socket.close();
            Deadline.expires_at(steady_timer::time_point::max());
        }
        Deadline.async_wait(std::bind(&ffClient::CheckDeadline, this));
    }

} // namespace ffGraph