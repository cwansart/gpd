#include "TcpServer.h"

using boost::bind;
using boost::asio::io_service;
using boost::asio::ip::tcp;

TcpServer::TcpServer(io_service &io_service)
	: m_acceptor(io_service, tcp::endpoint(tcp::v4(), m_port))
{
	startAccept();
}

void TcpServer::startAccept()
{
	TcpConnection::Pointer newConnection = TcpConnection::create(m_acceptor.get_io_service());

	m_acceptor.async_accept(
		newConnection->getSocket(),
		bind(&TcpServer::handleAccept, this, newConnection, boost::asio::placeholders::error)
	);
}

void TcpServer::handleAccept(TcpConnection::Pointer newConnection, const boost::system::error_code &error)
{
	if (!error) {
		newConnection->start();
	}

	startAccept();
}
