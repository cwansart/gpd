#include "TcpConnection.h"
#include <iostream>
#include <string>

using boost::bind;
using boost::asio::async_write;
using boost::asio::buffer;

TcpConnection::TcpConnection(boost::asio::io_service &io_service, std::function<std::string(std::string)> processingCallback)
    : m_socket(io_service), m_headerFound(false), m_readComplete(false), m_packageCounter(0),
      m_packageType(Type::UNKNOWN), m_processingCallback(processingCallback)
{
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
            else if (buf.find("OPTIONS") == 0) {
                m_packageType = Type::OPTIONS;
            }
            else {
                // Not a valid package. Abort!
                std::cerr << "Received an invalid package. Abort request.";
                std::cerr << buf << std::endl;
                m_socket.close();
            }
        }

        if (m_packageType == Type::OPTIONS) {
            handleOptionsRequest();
        }
        else {
            if (buf.find("\r\n\r\n") != std::string::npos) {
                m_headerFound = true;
            } // Else we could check if buf.size equals the size of m_buffer. If not, the package is invalid.

            m_packageCounter++;
            if (m_headerFound && m_packageType == Type::POST && buf.find("\r\n")) {
                m_message << buf;
                processMessage();
            }
            else if (m_headerFound && m_packageType == Type::GET) {
                handleGetRequest();
            }
            else {
                std::string ty(m_packageType == Type::GET ? "GET" : (m_packageType == Type::POST ? "POST" : "UKWN"));
                m_message << buf;
                processRequest();
            }
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
    const std::string message = m_message.str();
    const std::string::size_type pos = message.find("\r\n\r\n");
    const std::string::size_type machineLength = message.length() - (pos + 2);
    const std::string machine = message.substr(pos, machineLength);

    // TODO: We may need to change the return value of the callback function or
    //       add a parameter for another callback which will send the altered machine.
    const std::string newMachineJson(m_processingCallback(machine));

    // TODO: Send back the altered machine
    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Length: " << newMachineJson.length() << "\r\n"
             << "Content-Type: text/html\r\n"
             << "Access-Control-Allow-Origin: *\r\n"
             << "Connection: close\r\n\r\n"
             << newMachineJson << "\r\n";
    const std::string responseMessage(response.str());
    std::array<char, 32768> messageArray;
    std::copy(responseMessage.begin(), responseMessage.end(), messageArray.data());
    async_write(
            m_socket,
            buffer(responseMessage),
            bind(&TcpConnection::handleWrite, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
    );
}

void TcpConnection::handleGetRequest()
{
    const std::string message = "HTTP/1.1 200 OK\r\n"
            "Content-Length: 4\r\n"
            "Content-Type: text/html\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Connection: close\r\n\r\npong\r\n";
    std::array<char, 512> messageArray;
    std::copy(message.begin(), message.end(), messageArray.data());
    async_write(
            m_socket,
            buffer(message),
            bind(&TcpConnection::handleWrite, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
    );
}

void TcpConnection::handleOptionsRequest()
{
    const std::string message = "HTTP/1.1 200 OK\r\n"
            "Content-Length: 0\r\n"
            "Content-Type: text/html\r\n"
            "Access-Control-Allow-Methods: OPTIONS, GET, POST\r\n"
            "Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Connection: close\r\n\r\n";
    std::cout << "Options answer length: " << message.length() << std::endl;
    std::array<char, 512> messageArray;
    std::copy(message.begin(), message.end(), messageArray.data());
    async_write(
            m_socket,
            buffer(message),
            bind(&TcpConnection::handleWrite, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
    );
}

void TcpConnection::handleWrite(const boost::system::error_code &error, std::size_t bytesTransferred)
{
    if (error) {
        std::cerr << "An error occured while sending answer to get request. Error code: " << error.value() << std::endl
                  << error.message() << std::endl << std::endl;
        std::cerr << "bytes transferred while writing: " << bytesTransferred << std::endl << std::endl;
        return;
    }
}