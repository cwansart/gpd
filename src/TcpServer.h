#ifndef __TCP_SERVER__
#define __TCP_SERVER__

#include <iostream>
#include <memory>
#include <array>
#include <boost/asio.hpp>
#include "TcpConnection.h"

class TcpServer
{
    const unsigned short m_port = 2909;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::array<char, 8192> m_buffer;

    void startAccept();
    void handleAccept(std::shared_ptr<TcpConnection> newConnection, const boost::system::error_code &error);
    void handleRead(std::shared_ptr<TcpConnection> newConnection, const boost::system::error_code &error, std::size_t bytesTransferred);

public:
    TcpServer(boost::asio::io_service &io_service);
};

#endif
