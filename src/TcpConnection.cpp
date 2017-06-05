#include "TcpConnection.h"

#include <iostream>

using boost::bind;
using boost::asio::async_write;
using boost::asio::buffer;

TcpConnection::TcpConnection(boost::asio::io_service &io_service)
	: m_socket(io_service)
{
}

void TcpConnection::handleWrite()
{
	std::cout << "HANDLE WRITE CALLED" << std::endl;
}

void TcpConnection::start()
{
	m_message = "Hello World";

	async_write(
		m_socket,
		buffer(m_message),
		bind(&TcpConnection::handleWrite, shared_from_this())
	);
}
