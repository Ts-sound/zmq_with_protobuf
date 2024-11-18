#pragma once

#include <map>
#include <string>
#include <zmqpp/zmqpp.hpp>

namespace zmq_with_protobuf {
class Poller {
 public:
  Poller(zmqpp::context& ctx, const std::string& endpoint);

  ~Poller();

  bool AddPub(const std::string& name, const std::string& topic,
              const std::string& endpoint);

 private:
  zmqpp::context& ctx;
  std::string endpoint;

 private:
  std::map<std::string, zmqpp::socket> sockets;
};

}  // namespace zmq_with_protobuf
