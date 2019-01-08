#pragma once

#include <cstdint>
#include <iostream>
#include <boost/asio.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/array.hpp>

#include "archive.hpp"


namespace simulink {

    template<typename MessageType>
    class udp_receiver {
    public:
        typedef std::function<void(const MessageType&)> handler_t;
        typedef boost::asio::streambuf buffer_t;

    private:
        const handler_t& _on_recv_handler;
        boost::asio::ip::udp::socket _socket;
        buffer_t _recv_buffer;


        void recv_data();
        MessageType buffer_to_message(buffer_t& buffer) const;
    public:
        udp_receiver(boost::asio::io_service& io_service,
                    std::uint16_t port,
                    const handler_t& on_recv_handler);
    };
}

template<typename MessageType>
simulink::udp_receiver<MessageType>::udp_receiver(
    boost::asio::io_service& io_service,
    std::uint16_t port,
    const handler_t& on_recv_handler)
    : _on_recv_handler(on_recv_handler),
    _socket(io_service,
            boost::asio::ip::udp::endpoint(
                boost::asio::ip::udp::v4(), port))
{
    using boost::asio::ip::udp;
    recv_data();
}

template<typename MessageType>
void simulink::udp_receiver<MessageType>::recv_data(){
    _socket.async_receive(
        _recv_buffer.prepare(sizeof(MessageType)),
        [&](const boost::system::error_code& error, std::size_t trans_bytes){
            if (error)
                return;
            
            _recv_buffer.commit(sizeof(MessageType));
            MessageType msg = buffer_to_message(_recv_buffer);
            _on_recv_handler(msg);
            recv_data();
        }
    );
}

template<typename MessageType>
MessageType simulink::udp_receiver<MessageType>::buffer_to_message(
    buffer_t& buffer) const
{
    MessageType message;
    std::istream is(&buffer);
    
    SimulinkInputArchive archive(is);
    archive(message);
    return message;
}