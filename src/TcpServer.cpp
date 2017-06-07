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

void TcpServer::handleAccept(std::shared_ptr<TcpConnection> newConnection, const boost::system::error_code &error)
{
    if (!error) {
        //newConnection->start();

        std::cout << "Accepting new connection" << std::endl;
        newConnection->getSocket().async_read_some(
            buffer(m_buffer),
            boost::bind(&TcpServer::handleRead, this, newConnection, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
        );
    }

    startAccept();
}

void TcpServer::handleRead(std::shared_ptr<TcpConnection> newConnection, const boost::system::error_code &error, std::size_t bytesTransferred)
{
    // 89 is cancalled or closed
    if (error && error.value() != 89) {
        std::cerr << "An error occured while reading a request. Error code: " << error.value() << std::endl
                  << error.message() << std::endl << std::endl;
        std::cerr << "Bytes transferred: " << bytesTransferred << std::endl << std::endl;
        return;
    }

    std::string buf(std::begin(m_buffer), std::begin(m_buffer)+bytesTransferred);
    std::cout << "buf.size(): " << buf.length() << std::endl << std::endl;
    if (buf.size() > 0) {
        //auto found = buf.find("\r\n\r\n");
        //std::cout << "Found: " << found << ", buf size: " << buf.size() << std::endl << std::endl;
        std::cout << ">>>>MSG START" << std::endl
                  << buf << std::endl
                  << "<<<<MSG END" << std::endl << std::endl;
    }
    else {
        newConnection->getSocket().async_read_some(
                buffer(m_buffer),
                boost::bind(&TcpServer::handleRead, this, newConnection, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
        );
    }

}
