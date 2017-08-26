/**
 * @author Christian Wansart
 * @since 2017-06-14
 *
 * Licensed under the terms of the MIT license. See LICENSE file in project root.
 */


#include "TcpConnection.h"
#include <iostream>

using std::cout;
using std::endl;
using std::string;

using boost::bind;
using boost::asio::async_write;
using boost::asio::buffer;

TcpConnection::TcpConnection(boost::asio::io_service &io_service,
                             boost::asio::ssl::context &context,
                             std::function<std::string(std::string)> processingCallback)
    : m_socket(io_service, context),
      m_headerFound(false),
      m_readComplete(false),
      m_packageCounter(0),
      m_packageType(Type::UNKNOWN),
      m_processingCallback(processingCallback),
      m_transferredTotal(0),
      m_contentLength(0)
{
}

/**
 * Gets called when a new connection was created from TcpServer. It calls Boost.Asio's async_read_some method to start
 * reading the receiving bytes.
 *
 * @author Christian Wansart
 * @since 2017-06-14
 */
void TcpConnection::processRequest()
{
    m_socket.async_handshake(boost::asio::ssl::stream_base::server,
                             boost::bind(&TcpConnection::handle_handshake,
                                         shared_from_this(),
                                         boost::asio::placeholders::error));

}

/**
 * Handles the SSL handshake.
 * 
 * @author Christian Wansart
 * @since 2017-08-25
 */
void TcpConnection::handle_handshake(const boost::system::error_code &error)
{
    if (!error) {
        m_socket.async_read_some(
                buffer(m_buffer),
                boost::bind(&TcpConnection::handleRead,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred)
        );
    }
    else {
        std::cerr << "SSL handshake failed... Aborting" << std::endl;
        delete this;
    }
}

/**
 * Handles a read call from processRequest. Checks for errors, closes the connection if necessary and copies the read
 * bytes into a string which will be passed to other methods later on.
 *
 * @param error error object of Boost.Asio
 * @param bytesTransferred transferred bytes
 * @author Christian Wansart
 * @since 2017-06-14
 */
void TcpConnection::handleRead(const boost::system::error_code &error, std::size_t bytesTransferred)
{
    // 89 is cancelled or closed
    if (error && error.value() != 89) {
        // 2 = End of file
        if (error.value() == 2) {
            m_socket.shutdown();
            return;
        }

        std::cerr << "An error occured while reading a request. Error code: " << error.value() << std::endl
                  << error.message() << std::endl << std::endl;
        std::cerr << "Bytes transferred: " << bytesTransferred << std::endl << std::endl;
        return;
    }

    string buf(std::begin(m_buffer), std::begin(m_buffer)+bytesTransferred);

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

/**
 * Processes a package according to its type.
 *
 * @param buf the buffer that was read
 * @param bytesTransferred the bytes that have been transferred
 * @author Christian Wansart
 * @since 2017-06-14
 */
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
                m_socket.shutdown();
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
            m_socket.shutdown();
        }
    }
    else {
        handlePostRequest(buf, bytesTransferred);
    }
}

/**
 * Extracts the header information "Content-Length" and saves it to m_contentLength.
 *
 * @author Christian Wansart
 * @since 2017-06-14
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
 * @author Christian Wansart
 * @since 2017-06-14
 */
void TcpConnection::extractHeaderAndBody(const std::string &buf, const std::size_t bytesTransferred)
{
    m_headerSize = buf.find("\r\n\r\n");

    // Check if only the header was received...
    if ((buf.length() - 4) == m_headerSize) {
        // if so we need to read again but m_transferredTotal remains 0
    }
    else {
        // we need to subtract 4 because we don't want to count \r\n\r\n
        m_transferredTotal += bytesTransferred - m_headerSize - 4;

        splitHeaderAndBody(buf);

        if ((m_transferredTotal + 2) >= m_contentLength) {
            // Need to check if the first read received the header + the whole body.
            processMessage();
            return;
        }

    }
    processRequest();
}

/**
 * Splits the header and body and saves the body in m_message.
 *
 * @param buf the received message
 * @author Christian Wansart
 * @since 2017-06-14
 */
void TcpConnection::splitHeaderAndBody(const std::string &buf)
{
    const std::string::size_type machineLength = buf.length() - (m_headerSize + 2);
    m_message << buf.substr((m_headerSize + 4), machineLength);
}

/**
 * Calls the m_processingCallback function to process the received graph. After that creates a package and sends it back
 * to the client with the altered graph.
 *
 * @author Christian Wansart
 * @since 2017-06-14
 * @todo the buffer might be too small perhaps it should be bigger or at least a check should be added to test for its size.
 */
void TcpConnection::processMessage()
{
    const string machine = m_message.str();
    const string newMachineJson(m_processingCallback(machine));

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

/**
 * Handles a simple get request which is used for pinging the server. It simply returns a message with "pong" in its body.
 *
 * @author Christian Wansart
 * @since 2017-06-14
 */
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
            bind(&TcpConnection::handleWrite,
                 shared_from_this(),
                 boost::asio::placeholders::error,
                 boost::asio::placeholders::bytes_transferred)
    );
}

/**
 * Handles post requests if they weren't able to be read in one piece in extractHeaderAndBody.
 *
 * @param buf the received message
 * @param bytesTransferred the transferred bytes
 * @author Christian Wansart
 * @since 2017-06-14
 */
void TcpConnection::handlePostRequest(const std::string &buf, const std::size_t bytesTransferred)
{
    m_transferredTotal += bytesTransferred;
    m_message << buf;

    // Check if message ends on \r\n, if so we got everything.
    if ((m_transferredTotal + 2) >= m_contentLength) {
        processMessage();
        return;
    }

    processRequest();
}

/**
 * Handles OPTIONS requests which will be send from the client prior to sending the machine JSON string.
 *
 * @author Christian Wansart
 * @since 2017-06-14
 */
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
            bind(&TcpConnection::handleWrite,
                 shared_from_this(),
                 boost::asio::placeholders::error,
                 boost::asio::placeholders::bytes_transferred)
    );
}

/**
 * This method will be called from every method that writes back messages to the client. It only prints an error in the
 * console if one appears. Otherwise it doesn't do anything.
 *
 * @param error a Boost.Asio error object
 * @param bytesTransferred the transferred bytes
 * @author Christian Wansart
 * @since 2017-06-14
 */
void TcpConnection::handleWrite(const boost::system::error_code &error, std::size_t bytesTransferred)
{
    if (error) {
        std::cerr << "An error occured while sending answer to get request. Error code: " << error.value() << std::endl
                  << error.message() << std::endl << std::endl;
        std::cerr << "bytes transferred while writing: " << bytesTransferred << std::endl << std::endl;
        return;
    }
}
