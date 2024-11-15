
#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <zmqpp/zmqpp.hpp>

#include "publisher.h"
#include "subscriber.h"

static void AsyncSubTest(std::string end) {
  std::cout << "\n---test with " << end << std::endl;
  zmqpp::context ctx;

  std::string g_topic = "topic1";

  zmq_with_protobuf::Publisher pub(ctx, end);
  zmq_with_protobuf::Subscriber sub(ctx, end);
  sub.Subscribe(g_topic);
  sub.Set(1000);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::thread t([&sub]() {
    zmq_with_protobuf::Subscriber::MsgCallback func =
        [](const std::string& topic, const std::string& msg) {
          std::cout << "sub recv : " << topic << " - " << msg << std::endl;
        };
    sub.StartAsyncRecv(func);
  });

  std::string msg = "hello ";
  for (size_t i = 0; i < 4; i++) {
    std::string sendmsg = msg + std::to_string(i);
    pub.Send(g_topic, sendmsg);
    std::cout << "pub send : " << sendmsg << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  sub.Stop();
  t.join();
}

TEST(pubsub, async_sub_test) {
  AsyncSubTest("tcp://127.0.0.1:5555");
  AsyncSubTest("ipc://demo");
  AsyncSubTest("inproc://demo");
}
