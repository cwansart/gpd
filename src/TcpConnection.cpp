#include "TcpConnection.h"

#include <iostream>

using boost::bind;
using boost::asio::async_write;
using boost::asio::buffer;

TcpConnection::TcpConnection(boost::asio::io_service &io_service)
    : m_socket(io_service)
{
}

void TcpConnection::handleWrite(const boost::system::error_code &error, std::size_t bytesTransferred)
{
    if (error) {
        std::cerr << "An error occured while wiring. Error code: " << error.value() << std::endl
                  << error.message() << std::endl << std::endl;
        std::cerr << "bytes transferred while writing: " << bytesTransferred << std::endl << std::endl;
        return;
    }

    //std::cerr << "bytes transferred while writing: " << bytesTransferred << std::endl << std::endl;
}

void TcpConnection::start()
{
    m_message = "HTTP/1.1 200 OK\r\n"
            "Content-Length: 0\r\n"
            "Content-Type: text/html\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Connection: Closed\r\n\r\n";

    async_write(
        m_socket,
        buffer(m_message),
        bind(&TcpConnection::handleWrite, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
    );
}
