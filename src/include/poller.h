#pragma once

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <zmqpp/zmqpp.hpp>

namespace zmq_with_protobuf {

typedef std::function<void(const std::string& id_or_topic,
                           const std::string& msg)>
    RecvCallback;
typedef std::function<void(const std::string& topic, const std::string& msg)>
    ErrCallback;

class Poller {
 public:
  typedef std::shared_ptr<zmqpp::socket> SocketPtr;
  class Session {
    friend class Poller;

   public:
    std::string GetName() { return name; }

   public:
    void SetRecvCallback(RecvCallback func);
    void SetErrCallback(ErrCallback func);

    void Send(const std::string& msg);

    // server reply with target id
    void Send(const std::string& id, const std::string& msg);

    Session(Poller& poller, SocketPtr sock, const std::string& name,
            const std::string& topic);

   private:
    Poller& poller;
    SocketPtr sock;
    std::string name, topic;

    RecvCallback recv_func;

    ErrCallback err_func;
  };

  typedef std::shared_ptr<Session> SessionPtr;
  struct PullOutEvent {
    std::string msg;
    std::string id;
  };

 public:
  friend class Session;
  Poller();

  ~Poller();

  void PollOnce(int time_ms = 500);

  SessionPtr AddPub(const std::string& name, const std::string& topic,
                    const std::string& endpoint);

  SessionPtr AddSub(const std::string& name, const std::string& topic,
                    const std::string& endpoint);

  SessionPtr AddServer(const std::string& name, const std::string& topic,
                       const std::string& endpoint);
  SessionPtr AddClient(const std::string& name, const std::string& topic,
                       const std::string& endpoint);

  bool AddPullOutEvent(const std::string& session_name, const PullOutEvent& ev);

 private:
  zmqpp::context ctx;
  std::string endpoint;
  zmqpp::poller poller_;
  std::map<std::string, std::queue<PullOutEvent>> pullout_event_queue;

  std::mutex mtx_;

 private:
  std::map<std::string, SessionPtr> sessions;
};

}  // namespace zmq_with_protobuf
