
#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <zmqpp/zmqpp.hpp>

#include "publisher.h"
#include "subscriber.h"

static void PubSubTest(std::string end) {
  std::cout << "\n---test with " << end << std::endl;
  zmqpp::context ctx;

  std::string g_topic = "topic1";

  zmq_with_protobuf::Publisher pub(ctx, end);
  zmq_with_protobuf::Subscriber sub(ctx, end);
  sub.Subscribe(g_topic);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::thread t([&sub]() {
    std::string topic, msg;
    int cnt = 4;
    while (cnt--) {
      sub.Recv(topic, msg);
      std::cout << "sub recv : " << topic << " - " << msg << std::endl;
    }
  });

  std::string msg = "hello ";
  for (size_t i = 0; i < 4; i++) {
    std::string sendmsg = msg + std::to_string(i);
    pub.Send(g_topic, sendmsg);
    std::cout << "pub send : " << sendmsg << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  t.join();
}

TEST(pubsub, pubsub_test) {
  PubSubTest("tcp://127.0.0.1:5555");
  PubSubTest("ipc://demo");
  PubSubTest("inproc://demo");
}

static bool PubSubIpcTest(std::string end) {
  std::cout << "\n--- ipc test with " << end << std::endl;

  std::string g_topic = "topic1";
  std::string sendmsg = "hello";

  if (fork() != 0) {
    zmqpp::context ctx;
    zmq_with_protobuf::Publisher pub(ctx, end);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    pub.Send(g_topic, sendmsg);
    std::cout << "pub send : " << sendmsg << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    exit(0);
  } else {
    zmqpp::context ctx;
    zmq_with_protobuf::Subscriber sub(ctx, end);
    sub.Subscribe(g_topic);
    sub.Set(2000);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::string topic, msg;
    sub.Recv(topic, msg);
    std::cout << "sub recv : " << msg << std::endl;
    return (msg == sendmsg);
  }
}

TEST(pubsub, pubsub_ipc_test) {
  EXPECT_TRUE(PubSubIpcTest("tcp://127.0.0.1:5555"));
  EXPECT_TRUE(PubSubIpcTest("ipc://demo"));
  EXPECT_FALSE(PubSubIpcTest("inproc://demo"));
}