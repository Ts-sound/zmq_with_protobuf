
#include "poller.h"

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <zmqpp/zmqpp.hpp>

static void PollerPubSubTest(std::string end) {
  std::cout << "\n---test with " << end << std::endl;

  std::string g_topic = "topic1";

  zmq_with_protobuf::Poller poller;

  auto pub1 = poller.AddPub("pub1", g_topic, end);
  auto sub1 = poller.AddSub("sub1", g_topic, end);
  zmq_with_protobuf::RecvCallback func = [](const std::string& topic,
                                            const std::string& msg) {
    std::cout << "sub1 received: " << topic << " " << msg << std::endl;
  };
  sub1->SetRecvCallback(func);

  std::thread t([&pub1]() {
    for (size_t i = 0; i < 4; i++) {
      std::string msg = "hello " + std::to_string(i);
      pub1->Send(msg);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  });

  for (size_t i = 0; i < 10; i++) {
    poller.PollOnce();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  t.join();
}

TEST(poller, poller_pubsub_test) { PollerPubSubTest("tcp://127.0.0.1:5555"); }

static std::string stringToHex(const std::string& input) {
  std::ostringstream hex_stream;

  for (unsigned char c : input) {
    hex_stream << std::hex << std::setfill('0') << std::setw(2)
               << static_cast<int>(c);
  }

  return hex_stream.str();
}

static void PollerServerClientTest(std::string end) {
  std::cout << "\n---test with " << end << std::endl;

  std::string g_topic = "topic1";

  zmq_with_protobuf::Poller poller;

  auto server = poller.AddServer("server1", g_topic, end);
  zmq_with_protobuf::RecvCallback func = [&server](const std::string& id,
                                                   const std::string& msg) {
    std::cout << "server received: " << stringToHex(id) << " " << msg
              << std::endl;
    std::cout << "server send: " << stringToHex(id) << " " << msg + "_handled"
              << std::endl;
    server->Send(id, msg + "_handled");
  };
  server->SetRecvCallback(func);

  std::vector<zmq_with_protobuf::Poller::SessionPtr> clients;
  std::vector<zmq_with_protobuf::RecvCallback> funcs;

  auto clientrecv = [](zmq_with_protobuf::Poller::SessionPtr cli,
                       const std::string&, const std::string& msg) {
    std::cout << cli->GetName() << "  received: "
              << " " << msg << std::endl;
  };

  for (size_t i = 0; i < 3; i++) {
    auto cli = poller.AddClient("client" + std::to_string(i), g_topic, end);
    clients.push_back(cli);
    funcs.push_back(std::bind(clientrecv, cli, std::placeholders::_1,
                              std::placeholders::_2));

    cli->SetRecvCallback(funcs.back());
  }

  std::thread t([&clients]() {
    for (size_t i = 0; i < 2; i++) {
      for (auto& client : clients) {
        std::string msg = "hello " + std::to_string(i);
        std::cout << client->GetName() << " send: "
                  << " " << msg << std::endl;
        client->Send(msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
      }
    }
  });

  for (size_t i = 0; i < 20; i++) {
    poller.PollOnce();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  t.join();
}

TEST(poller, poller_server_client_test) {
  PollerServerClientTest("tcp://127.0.0.1:5555");
}
