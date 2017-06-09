#include "TcpConnection.h"
#include <iostream>

using boost::bind;
using boost::asio::async_write;
using boost::asio::buffer;

TcpConnection::TcpConnection(boost::asio::io_service &io_service)
    : m_socket(io_service), m_headerFound(false), m_readComplete(false), m_packageCounter(0), m_packageType(Type::UNKNOWN)
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
    /*
    auto m_message = "HTTP/1.1 200 OK\r\n"
            "Content-Length: 0\r\n"
            "Content-Type: text/html\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Connection: Closed\r\n\r\n";

    async_write(
            m_socket,
            buffer(m_message),
            bind(&TcpConnection::handleWrite, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
    );
     */
}

void TcpConnection::processRequest()
{
    m_socket.async_read_some(
            buffer(m_buffer),
            boost::bind(&TcpConnection::handleRead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
    );
}

void TcpConnection::handleRead(const boost::system::error_code &error, std::size_t bytesTransferred)
{
    // 89 is cancelled or closed
    if (error && error.value() != 89) {
        std::cerr << "An error occured while reading a request. Error code: " << error.value() << std::endl
                  << error.message() << std::endl << std::endl;
        std::cerr << "Bytes transferred: " << bytesTransferred << std::endl << std::endl;
        return;
    }

    std::string buf(std::begin(m_buffer), std::begin(m_buffer)+bytesTransferred);

    if (buf.size() > 0) {
        // We need to differentiate between GET and POST packages, since GET request don't have a body part so we
        // must skip the second check for \r\n after \r\n\r\n has been read.
        if (m_packageCounter == 0) {
            if (buf.find("GET") == 0) {
                m_packageType = Type::GET;
            }
            else if (buf.find("POST") == 0) {
                m_packageType = Type::POST;
            }
            else {
                // Not a valid package. Abort!
                std::cerr << "Received an invalid package. Abort request.";
                // TODO: find a way to close the connection excplitily.
            }
        }

        if (buf.find("\r\n\r\n") != std::string::npos) {
            m_headerFound = true;
        } // Else we could check if buf.size equals the size of m_buffer. If not, the package is invalid.

        m_packageCounter++;
        if (m_headerFound && m_packageType == Type::POST && buf.find("\r\n")) {
            m_message << buf;
            processMessage();
        }
        else {
            m_message << buf;
            processRequest();
        }
    }
    else {
        // TODO: Add some kind of timeout, since async_read_some does not guarantee to read anything. Either a
        //       timer based or a try based timer is possible.
        std::cerr << "read empty buffer" << std::endl;
        processRequest();
    }
}

void TcpConnection::processMessage()
{

}
