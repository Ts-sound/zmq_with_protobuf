#include "subscriber.h"

#include <iostream>

namespace zmq_with_protobuf {
Subscriber::Subscriber(zmqpp::context& ctx, const std::string& endpoint)
    : ctx(ctx),
      socket(zmqpp::socket(ctx, zmqpp::socket_type::sub)),
      endpoint(endpoint) {
  socket.connect(endpoint);
}

Subscriber::~Subscriber() { this->Stop(); }

void Subscriber::Subscribe(const std::string& topic) {
  socket.subscribe(topic);
}

void Subscriber::Set(int32_t timeout_ms, int32_t reconnect_interval_ms,
                     int recv_buff_size) {
  this->socket.set(zmqpp::socket_option::receive_timeout, timeout_ms);
  this->socket.set(zmqpp::socket_option::reconnect_interval,
                   reconnect_interval_ms);
  this->socket.set(zmqpp::socket_option::receive_buffer_size, recv_buff_size);
}

bool Subscriber::Recv(std::string& topic, std::string& msg) {
  topic.clear();
  msg.clear();

  try {
    zmqpp::message message;

    if (socket.receive(message)) {
      message >> topic >> msg;
      return true;
    }
    return false;
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return false;
  }
}

void Subscriber::StartAsyncRecv(MsgCallback func) {
  while (this->alive) {
    zmqpp::message message;
    std::string topic, msg;
    if (this->Recv(topic, msg)) {
      func(topic, msg);
    } else {
      if (this->alive == false) {
        std::cout << "return ";
        return;
      }

      continue;
    }
  }
}

void Subscriber::Stop() {
  this->alive = false;
  try {
    socket.disconnect(endpoint);
    // socket.close();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}

}  // namespace zmq_with_protobuf
