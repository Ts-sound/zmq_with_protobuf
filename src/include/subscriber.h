#pragma once

#include <string>
#include <zmqpp/zmqpp.hpp>

namespace zmq_with_protobuf {
class Subscriber {
 public:
  Subscriber(zmqpp::context& ctx, const std::string& endpoint);
  ~Subscriber();

  void Subscribe(const std::string& topic);

  void Recv(std::string& topic, std::string& msg);

 private:
  zmqpp::context& ctx;
  zmqpp::socket socket;
  std::string endpoint;
};

}  // namespace zmq_with_protobuf
