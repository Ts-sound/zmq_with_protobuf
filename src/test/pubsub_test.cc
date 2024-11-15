
#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <zmqpp/zmqpp.hpp>

#include "publisher.h"
#include "subscriber.h"

void publisher() {
  zmqpp::context context;
  zmqpp::socket socket(context, zmqpp::socket_type::pub);
  socket.bind("tcp://127.0.0.1:5555");

  std::this_thread::sleep_for(std::chrono::seconds(1));  // 防止订阅者先启动

  while (true) {
    zmqpp::message message;
    message << "topic1"
            << "Hello from publisher!";
    socket.send(message);
    std::cout << "Published: Hello from publisher!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void subscriber() {
  zmqpp::context context;
  zmqpp::socket socket(context, zmqpp::socket_type::sub);
  socket.connect("tcp://127.0.0.1:5555");
  socket.subscribe("topic1");

  while (true) {
    zmqpp::message message;
    socket.receive(message);

    std::string topic, content;
    message >> topic >> content;
    std::cout << "Received: [" << topic << "] " << content << std::endl;
  }
}

// TEST(pubsub, tcp_test) {
//     std::thread pub(publisher);
//     std::thread sub(subscriber);

//     pub.join();
//     sub.join();

// }

TEST(pubsub, tcp_test) {
  zmqpp::context ctx;
  std::string end = "tcp://127.0.0.1:5555";

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
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  t.join();
}