#pragma once

#ifndef NO_UDP_SENDER

#include <cstdint>
#include <iostream>
#include <boost/asio.hpp>
#include <cereal/types/array.hpp>
#include "archive.hpp"
#include "utils.hpp"

namespace simulink {

    template<typename MessageType>
    class udp_sender {
        boost::asio::ip::udp::socket _socket;
        boost::asio::ip::udp::endpoint _endpoint;

        void message_to_buffer(const MessageType& message, boost::asio::streambuf& buffer) const;
    public:
        udp_sender(boost::asio::io_service& io_service,
                const std::string& endpoint,
                std::uint16_t port,
                const std::string& local_endpoint,
                std::uint16_t local_port);
        udp_sender(boost::asio::io_service& io_service,
                const std::string& endpoint,
                std::uint16_t port);

        template<typename WriteHandler>
        void async_send(const MessageType& buffer,
                        WriteHandler handler);

        std::size_t send(const MessageType& buffer);
    };
}

template<typename MessageType>
simulink::udp_sender<MessageType>::udp_sender(
    boost::asio::io_service& io_service,
    const std::string& endpoint,
    std::uint16_t port,
    const std::string& local_endpoint,
    std::uint16_t local_port) 
    : _socket(io_service)
{
    using boost::asio::ip::udp;
    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), endpoint, std::to_string(port));
    _endpoint = *resolver.resolve(query);
    _socket.open(udp::v4());

    udp::endpoint _local_endpoint = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address::from_string(local_endpoint), local_port);
    _socket.bind(_local_endpoint);
    
    if (simulink::is_broadcast_ip_address(endpoint)){
        _socket.set_option(boost::asio::socket_base::broadcast(true));
    }
}

template<typename MessageType>
simulink::udp_sender<MessageType>::udp_sender(
    boost::asio::io_service& io_service,
    const std::string& endpoint,
    std::uint16_t port) 
    : _socket(io_service)
{
    using boost::asio::ip::udp;
    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), endpoint, std::to_string(port));
    _endpoint = *resolver.resolve(query);
    _socket.open(udp::v4());
    
    if (simulink::is_broadcast_ip_address(endpoint)){
        _socket.set_option(boost::asio::socket_base::broadcast(true));
    }
}

template<typename MessageType>
template<typename WriteHandler>
void simulink::udp_sender<MessageType>::async_send(
    const MessageType& message,
    WriteHandler handler)
{
    auto buffer = message_to_buffer(message);
    _socket.async_send_to(buffer, _endpoint, handler);
}

template<typename MessageType>
std::size_t simulink::udp_sender<MessageType>::send(const MessageType& message)
{
    boost::asio::streambuf buffer;
    message_to_buffer(message, buffer);
    return _socket.send_to(buffer.data(), _endpoint);
}

template<typename MessageType>
void simulink::udp_sender<MessageType>::message_to_buffer(
    const MessageType& message, boost::asio::streambuf& buffer) const 
{
    std::ostream os(&buffer);
    SimulinkOutputArchive archive(os);
    archive(message);
}

#endif // NO_UDP_SENDER