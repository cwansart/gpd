/**
 * @author Christian Wansart
 * @since 2017-06-14
 *
 * Licensed under the terms of the MIT license. See LICENSE file in project root.
 */

#ifndef __TCP_SERVER__
#define __TCP_SERVER__

#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <boost/asio.hpp>

#include "TcpConnection.h"

class TcpServer
{
    const unsigned short m_port = 2909;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::function<std::string(std::string)> m_processingCallback;

    void startAccept();
    void handleAccept(std::shared_ptr<TcpConnection> clientConnection, const boost::system::error_code &error);

public:
    TcpServer(boost::asio::io_service &io_service, std::function<std::string(std::string)> processingCallback);
};

#endif
