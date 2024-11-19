
#include "poller.h"

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <zmqpp/zmqpp.hpp>

static void PollerTest(std::string end) {
  std::cout << "\n---test with " << end << std::endl;

  std::string g_topic = "topic1";

  zmq_with_protobuf::Poller poller;

  auto pub1 = poller.AddPub("pub1", g_topic, end);
  auto sub1 = poller.AddSub("sub1", g_topic, end);
  zmq_with_protobuf::RecvCallback func = [](const std::string& topic,
                                            const std::string& msg) {
    std::cout << "sub1 received: " << topic << " " << msg << std::endl;
  };
  sub1->SetRecvCallback(&func);

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

TEST(poller, poller_test) { PollerTest("tcp://127.0.0.1:5555"); }