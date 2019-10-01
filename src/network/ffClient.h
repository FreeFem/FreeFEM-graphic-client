/**
 * @file ffClient.h
 * @brief TCP async client.
 */
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

/**
 * @brief TCP async client made using asio, check the pro-actor pattern for more information on how it works.
 */
class ffClient {
   public:
    /**
     * @brief Constructor
     *
     * @param Hosts [in] - Server's address.
     * @param Port [in] - Server's port.
     * @param SharedQueue [in] - Thread safe queue, used by the ffGraph::ffClient to post data for the
     * ffGraph::ResourceManager.
     */
    ffClient(std::string Host, std::string Port, std::shared_ptr<std::deque<std::string>>& SharedQueue);

    /**
     * @brief Start the ffGraph::ffClient.
     *
     * @return void
     */
    void Start( );

    /**
     * @brief Stop the ffGraph::ffClient.
     *
     * @return void
     */
    void Stop( );

   private:
    /**
     * @brief Launch a async connection call.
     *
     * @param EndP_ITE [in] - Tested tcp::resolver::results_type.
     *
     * @return void
     */
    void StartConnect(tcp::resolver::results_type::iterator EndP_ITE);

    /**
     * @brief Called when StartConnect async connection is done.
     *
     * @param error [in] - Error code if the asyn connection failed.
     * @param EndP_ITE [in] - Tested tcp::resolver::results_type.
     *
     * @return void
     */
    void HandleConnect(const std::error_code& error, tcp::resolver::results_type::iterator EndP_ITE);

    /**
     * @brief Launch the read loop, calls a async read.
     *
     * @return void
     */
    void StartRead( );

    /**
     * @brief Called when StartRead async read is done.
     *
     * @param error [in] - Error code if the asyn read failed.
     * @param n [in] - Number of charactere read.
     *
     * @return void.
     */
    void HandleRead(const std::error_code& error, std::size_t size);

    /**
     * @brief Launch the write loop, calls a async write. Allow the client to stay alive in server connection list.
     *
     * @return void
     */
    void StartWrite( );

    /**
     * @brief Called when StartWrite async read is done. Waits a few seconds before sending a new one.
     *
     * @param error [in] - Error code if the asyn read failed.
     *
     * @return void.
     */
    void HandleWrite(const std::error_code& error);

    /**
     * @brief Check if any action as timed out.
     *
     * @return void
     */
    void CheckDeadline( );

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

}    // namespace ffGraph

#endif    // FF_CLIENT_H_