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

    void startAccept();
    void handleAccept(std::shared_ptr<TcpConnection> clientConnection, const boost::system::error_code &error);

public:
    TcpServer(boost::asio::io_service &io_service);
};

#endif
