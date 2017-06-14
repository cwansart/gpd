/**
 * @author Christian Wansart
 * @since 2017-06-14
 *
 * Licensed under the terms of the MIT license. See LICENSE file in project root.
 */

#include "TcpServer.h"

using boost::bind;
using boost::asio::io_service;
using boost::asio::ip::tcp;

TcpServer::TcpServer(io_service &io_service, std::function<std::string(std::string)> processingCallback)
    : m_acceptor(io_service, tcp::endpoint(tcp::v4(), m_port)), m_processingCallback(processingCallback)
{
    startAccept();
}

/**
 * Creates new TcpConnection objects for every connection and calls handleAccept for it.
 *
 * @author Christian Wansart
 * @since 2017-06-14
 */
void TcpServer::startAccept()
{
    auto newConnection = std::make_shared<TcpConnection>(m_acceptor.get_io_service(), m_processingCallback);

    m_acceptor.async_accept(
        newConnection->getSocket(),
        bind(&TcpServer::handleAccept, this, newConnection, boost::asio::placeholders::error)
    );
}

/**
 * If no errors occurred TcpConnection::processRequest() gets called. After that a new connection will be accepted.
 *
 * @param clientConnection the instantiated TcpConnection object passed from startAccept
 * @param error Boost.Asio's error object
 * @author Christian Wansart
 * @since 2017-06-14
 */
void TcpServer::handleAccept(std::shared_ptr<TcpConnection> clientConnection, const boost::system::error_code &error)
{
    if (!error) {
        clientConnection->processRequest();
    }

    startAccept();
}
