#include "TcpConnection.h"
#include <iostream>
#include <string>

using std::string;

using boost::bind;
using boost::asio::async_write;
using boost::asio::buffer;

TcpConnection::TcpConnection(boost::asio::io_service &io_service, std::function<std::string(std::string)> processingCallback)
    : m_socket(io_service), m_headerFound(false), m_readComplete(false), m_packageCounter(0),
      m_packageType(Type::UNKNOWN), m_processingCallback(processingCallback), m_transferredTotal(0), m_contentLength(0)
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
        processPackage(buf, bytesTransferred);
    }
    else {
        // TODO: Add some kind of timeout, since async_read_some does not guarantee to read anything. Either a
        //       timer based or a try based timer is possible.
        std::cerr << "read empty buffer" << std::endl;
        processRequest();
    }
}

void TcpConnection::processPackage(const std::string &buf, const std::size_t bytesTransferred)
{
    // We need to differentiate between GET and POST packages, since GET request don't have a body part so we
    // must skip the second check for \r\n after \r\n\r\n has been read.
    if (m_packageCounter == 0) {
        if (buf.find("GET") == 0) {
            m_packageType = Type::GET;
            handleGetRequest();
        }
        else if (buf.find("POST") == 0) {
            m_packageType = Type::POST;
            extractContentLength(buf);

            if (m_contentLength <= 0) {
                std::cerr << "Content-Length is 0, but must be non-zero on a POST request. Closing connection" << std::endl;
                m_socket.close();
            }

            extractHeaderAndBody(buf, bytesTransferred);
            m_packageCounter++;
            processRequest();
        }
        else if (buf.find("OPTIONS") == 0) {
            m_packageType = Type::OPTIONS;
            handleOptionsRequest();
        }
        else {
            // Not a valid package. Abort!
            std::cerr << "Received an invalid package. Abort request.";
            std::cerr << buf << std::endl;
            m_socket.close();
        }
    }
    else {
        handlePostRequest();
    }
}

/**
 * Extracts the header information "Content-Length" and saves it to m_contentLength.
 */
void TcpConnection::extractContentLength(const string &buf)
{
    const string headerName = "Content-Length:";
    const string::size_type contentLengthPos = buf.find(headerName);
    const string newBuf = string(buf.begin()+contentLengthPos, buf.end());
    const string::size_type newLinePos = newBuf.find("\r\n");
    const string contentLengthStr = newBuf.substr(headerName.length(), (newLinePos - headerName.length()));
    m_contentLength = std::stoi(contentLengthStr);
}

/**
 * Extracts, or rather separates the header and body. If only the header was send, it saves the header size in m_headerSize
 * and keeps the m_transferredTotal as is. If the body was send, partially or complete, it will be stored in m_message
 * and the received bytes of the body will be added to m_transferredTotal.
 *
 * @param buf reference to the buffer string which was read
 * @param bytesTransferred the transferred bytes
 */
void TcpConnection::extractHeaderAndBody(const std::string &buf, const std::size_t bytesTransferred)
{
    m_headerSize = buf.find("\r\n\r\n");

    // Check if only the header was received...
    if ((buf.length() - 4) == m_headerSize) {
        // if so we need to read again but m_transferredTotal remains 0
        processRequest();
    }
    else {
        // we need to subtract 4 because we don't want to count \r\n\r\n
        m_transferredTotal += bytesTransferred - m_headerSize - 4;

        if ((m_transferredTotal + 2) >= m_contentLength) {
            // Need to check if the first read received the header + the whole body.
            // Work in progress here...
        }

    }
    // TODO:  Remove later...
    exit(0);
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

void TcpConnection::handlePostRequest()
{

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