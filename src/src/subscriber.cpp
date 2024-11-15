#include "subscriber.h"

#include <iostream>

namespace zmq_with_protobuf {
Subscriber::Subscriber(zmqpp::context& ctx, const std::string& endpoint)
    : ctx(ctx),
      socket(zmqpp::socket(ctx, zmqpp::socket_type::sub)),
      endpoint(endpoint) {
  socket.connect(endpoint);
}

Subscriber::~Subscriber() { socket.close(); }

void Subscriber::Subscribe(const std::string& topic) {
  socket.subscribe(topic);
}

void Subscriber::Recv(std::string& topic, std::string& msg) {
  topic.clear();
  msg.clear();

  zmqpp::message message;
  socket.receive(message);
  message >> topic >> msg;
}

}  // namespace zmq_with_protobuf
