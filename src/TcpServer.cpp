#include "TcpServer.h"

using boost::bind;
using boost::asio::buffer;
using boost::asio::io_service;
using boost::asio::ip::tcp;

#include <sstream>
#include <string>

TcpServer::TcpServer(io_service &io_service)
    : m_acceptor(io_service, tcp::endpoint(tcp::v4(), m_port))
{
    startAccept();
}

void TcpServer::startAccept()
{
    auto newConnection = std::make_shared<TcpConnection>(m_acceptor.get_io_service());

    m_acceptor.async_accept(
        newConnection->getSocket(),
        bind(&TcpServer::handleAccept, this, newConnection, boost::asio::placeholders::error)
    );
}

void TcpServer::handleAccept(std::shared_ptr<TcpConnection> clientConnection, const boost::system::error_code &error)
{
    if (!error) {
        //newConnection->start();

        newConnection->processRequest();
    }

    startAccept();
}
