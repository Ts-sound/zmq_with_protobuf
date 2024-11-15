#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <zmqpp/zmqpp.hpp>

namespace zmq_with_protobuf {
class Subscriber {
 public:
  typedef std::function<void(const std::string& topic, const std::string& msg)>
      MsgCallback;

 public:
  Subscriber(zmqpp::context& ctx, const std::string& endpoint);
  ~Subscriber();

  void Subscribe(const std::string& topic);

  void Set(int32_t timeout_ms = 1000, int32_t reconnect_interval_ms = 5000,
           int recv_buff_size = 1024 * 1024);

  bool Recv(std::string& topic, std::string& msg);

  template <class T>
  bool Recv(std::string& topic, T& msg) {
    std::string msg_str;
    if (!Recv(topic, msg_str)) return false;
    return msg.ParseFromString(msg_str);
  }

  void StartAsyncRecv(MsgCallback func);
  void Stop();

 private:
  zmqpp::context& ctx;
  zmqpp::socket socket;
  std::string endpoint;

  std::atomic_bool alive = {true};
};

}  // namespace zmq_with_protobuf
