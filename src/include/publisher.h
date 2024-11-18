#pragma once

#include <string>
#include <zmqpp/zmqpp.hpp>

namespace zmq_with_protobuf {
class Publisher {
 public:
  /**
   * @brief Construct a new Publisher object
   *
   * @param ctx
   * @param endpoint "tcp://127.0.0.1:8411"
   */
  Publisher(zmqpp::context& ctx, const std::string& endpoint);

  ~Publisher();

  bool Set(int pub_buff_size = 1024 * 1024, int reconnet_ivl = 1000);

  bool Send(const std::string& topic, const std::string& msg);

  template <class T>
  bool Send(const std::string& topic, const T& msg) {
    std::string sendmsg;
    msg.SerializeToString(&sendmsg);
    Send(topic, sendmsg);
  }

 private:
  zmqpp::context& ctx;
  zmqpp::socket socket;
  std::string endpoint;
};

}  // namespace zmq_with_protobuf
