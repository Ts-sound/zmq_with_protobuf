#include "poller.h"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace zmq_with_protobuf {

static std::string StringToHex(const std::string& input) {
  std::stringstream hex_stream;

  for (unsigned char c : input) {
    hex_stream << std::hex << std::setfill('0') << std::setw(2)
               << static_cast<int>(c);
  }

  return hex_stream.str();
}

Poller::Poller() : ctx(zmqpp::context()) {}

Poller::~Poller() {
  std::lock_guard<std::mutex> lck(mtx_);

  for (auto& i : this->sessions) {
    try {
      poller_.remove(*(i.second->sock));
      i.second->sock->close();
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
    }
  }
}

void Poller::PollOnce(int time_ms) {
  if (poller_.poll(time_ms)) {
    // std::cout << "poller_.poll" << std::endl;
    std::lock_guard<std::mutex> lck(mtx_);
    for (auto& i : sessions) {
      auto& session = *(i.second);
      auto& sock = *(session.sock);
      if (poller_.has_input(sock)) {
        // std::cout << "has_input" << std::endl;
        if (session.recv_func) {
          try {
            zmqpp::message message;
            std::string msg, temp, topic;
            if (session.sock->receive(message)) {
              if (sock.type() == zmqpp::socket_type::router) {
                topic = message.get(0);
                msg = message.get(1);
              } else if (sock.type() == zmqpp::socket_type::dealer) {
                msg = message.get(0);
              } else {
                message >> topic >> msg;
              }
              mtx_.unlock();
              (session.recv_func)(topic, msg);
              mtx_.lock();
            }
          } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
          }
        }
      }
      if (this->pullout_event_queue[i.first].size() > 0) {
        if (poller_.has_output(sock)) {
          // std::cout << "has_output" << std::endl;
          zmqpp::message msg_zmq;
          auto& ev = this->pullout_event_queue[i.first].front();

          if (sock.type() == zmqpp::socket_type::router) {
            msg_zmq << ev.id << ev.msg;
            // std::cout << "out : " << StringToHex(ev.id) << "|" << ev.msg
            //           << std::endl;
          } else if (sock.type() == zmqpp::socket_type::dealer) {
            msg_zmq << ev.msg;
          } else {
            msg_zmq << session.topic << ev.msg;
          }

          this->pullout_event_queue[i.first].pop();
          session.sock->send(msg_zmq);
        }
      }

      if (poller_.has_error(sock)) {
        std::cout << "has_error" << std::endl;
        if (session.err_func) {
          try {
            std::cerr << "Error detected on socket :"
                      << zmq_strerror(zmq_errno()) << std::endl;
            zmqpp::message message;
            std::string msg, topic;
            mtx_.unlock();
            (session.err_func)(topic, msg);
            mtx_.lock();
          } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
          }
        }
      }
    }
  }
}

Poller::SessionPtr Poller::AddPub(const std::string& name,
                                  const std::string& topic,
                                  const std::string& endpoint) {
  std::lock_guard<std::mutex> lck(mtx_);
  SocketPtr sock =
      std::make_shared<zmqpp::socket>(ctx, zmqpp::socket_type::pub);
  SessionPtr se = std::make_shared<Session>(*this, sock, name, topic);
  sock->bind(endpoint);
  this->poller_.add(*sock, zmqpp::poller::poll_out | zmqpp::poller::poll_error);
  this->sessions.insert({name, se});
  return se;
}

Poller::SessionPtr Poller::AddSub(const std::string& name,
                                  const std::string& topic,
                                  const std::string& endpoint) {
  std::lock_guard<std::mutex> lck(mtx_);
  SocketPtr sock =
      std::make_shared<zmqpp::socket>(ctx, zmqpp::socket_type::sub);
  SessionPtr se = std::make_shared<Session>(*this, sock, name, topic);
  sock->connect(endpoint);
  sock->subscribe(topic);
  this->poller_.add(*sock, zmqpp::poller::poll_in | zmqpp::poller::poll_error);
  this->sessions.insert({name, se});
  return se;
}

Poller::SessionPtr Poller::AddServer(const std::string& name,
                                     const std::string& topic,
                                     const std::string& endpoint) {
  std::lock_guard<std::mutex> lck(mtx_);
  SocketPtr sock =
      std::make_shared<zmqpp::socket>(ctx, zmqpp::socket_type::router);
  SessionPtr se = std::make_shared<Session>(*this, sock, name, topic);
  sock->bind(endpoint);
  this->poller_.add(*sock, zmqpp::poller::poll_in | zmqpp::poller::poll_out |
                               zmqpp::poller::poll_error);
  this->sessions.insert({name, se});
  return se;
}

Poller::SessionPtr Poller::AddClient(const std::string& name,
                                     const std::string& topic,
                                     const std::string& endpoint) {
  std::lock_guard<std::mutex> lck(mtx_);
  SocketPtr sock =
      std::make_shared<zmqpp::socket>(ctx, zmqpp::socket_type::dealer);

  SessionPtr se = std::make_shared<Session>(*this, sock, name, topic);
  sock->connect(endpoint);
  this->poller_.add(*sock, zmqpp::poller::poll_in | zmqpp::poller::poll_out |
                               zmqpp::poller::poll_error);
  this->sessions.insert({name, se});
  return se;
}

bool Poller::AddPullOutEvent(const std::string& session_name,
                             const PullOutEvent& ev) {
  std::lock_guard<std::mutex> lck(mtx_);
  this->pullout_event_queue[session_name].push(ev);
  return true;
}

void Poller::Session::SetRecvCallback(RecvCallback func) {
  this->recv_func = func;
}

void Poller::Session::SetErrCallback(ErrCallback func) {
  this->err_func = func;
}

void Poller::Session::Send(const std::string& msg) {
  PullOutEvent ev;
  ev.msg = msg;
  poller.AddPullOutEvent(this->name, ev);
}

void Poller::Session::Send(const std::string& id, const std::string& msg) {
  PullOutEvent ev;
  ev.msg = msg;
  ev.id = id;
  poller.AddPullOutEvent(this->name, ev);
}

Poller::Session::Session(Poller& poller, SocketPtr sock,
                         const std::string& name, const std::string& topic)
    : poller(poller), sock(sock), name(name), topic(topic) {}

}  // namespace zmq_with_protobuf
