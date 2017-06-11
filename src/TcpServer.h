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
    std::function<void()> m_processingCallback;

    void startAccept();
    void handleAccept(std::shared_ptr<TcpConnection> clientConnection, const boost::system::error_code &error);

public:
    TcpServer(boost::asio::io_service &io_service, std::function<void()> processingCallback);
};

#endif
