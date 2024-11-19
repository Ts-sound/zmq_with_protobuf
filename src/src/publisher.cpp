#include "publisher.h"
namespace zmq_with_protobuf {
Publisher::Publisher(zmqpp::context& ctx, const std::string& endpoint)
    : ctx(ctx),
      socket(zmqpp::socket(ctx, zmqpp::socket_type::pub)),
      endpoint(endpoint) {
  this->socket.bind(endpoint);
}

Publisher::~Publisher() { socket.close(); }

bool Publisher::Set(int pub_buff_size, int reconnet_ivl) {
  this->socket.set(zmqpp::socket_option::send_buffer_size, pub_buff_size);
  this->socket.set(zmqpp::socket_option::reconnect_interval, reconnet_ivl);
}

bool Publisher::Send(const std::string& topic, const std::string& msg) {
  zmqpp::message msg_zmq;
  msg_zmq << topic << msg;
  this->socket.send(msg_zmq);
}

}  // namespace zmq_with_protobuf
