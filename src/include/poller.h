#pragma once

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <zmqpp/zmqpp.hpp>

namespace zmq_with_protobuf {

typedef std::function<void(const std::string& topic, const std::string& msg)>
    RecvCallback;
typedef std::function<void(const std::string& topic, const std::string& msg)>
    ErrCallback;

class Poller {
 public:
  typedef std::shared_ptr<zmqpp::socket> SocketPtr;
  class Session {
    friend class Poller;

   public:
    void SetRecvCallback(RecvCallback* func);
    virtual void SetErrCallback(ErrCallback* func);

    void Send(const std::string& msg);

    Session(Poller& poller, SocketPtr sock, const std::string& name,
            const std::string& topic);

   private:
    Poller& poller;
    SocketPtr sock;
    std::string name, topic;

    std::atomic<RecvCallback*> recv_func = {nullptr};

    std::atomic<ErrCallback*> err_func = {nullptr};
  };

  typedef std::shared_ptr<Session> SessionPtr;
  struct PullOutEvent {
    std::string msg;
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
