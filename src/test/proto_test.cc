
#include <google/protobuf/util/json_util.h>
#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <zmqpp/zmqpp.hpp>

#include "TestMsg.pb.h"
#include "publisher.h"
#include "subscriber.h"

static google::protobuf::util::JsonOptions MyJsonOptions() {
  google::protobuf::util::JsonOptions opt;
  opt.always_print_primitive_fields = true;
};

static void ProtoSubTest(std::string end) {
  std::cout << "\n---test with " << end << std::endl;
  zmqpp::context ctx;

  std::string g_topic = "topic1";

  Person person;
  person.set_name("John Doe");
  person.set_id(123);
  person.set_email("john.doe@example.com");

  zmq_with_protobuf::Publisher pub(ctx, end);
  zmq_with_protobuf::Subscriber sub(ctx, end);
  sub.Subscribe(g_topic);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::thread t([&sub]() {
    Person p;
    int cnt = 4;
    while (cnt--) {
      std::string topic, s;
      sub.Recv(topic, p);

      google::protobuf::util::MessageToJsonString(p, &s,MyJsonOptions());
      s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
      std::cout << "sub recv : " << topic << " - " << s << std::endl;
    }
  });

  std::string msg = "hello ";
  for (size_t i = 0; i < 4; i++) {
    person.set_id(i);
    std::string sendmsg;
    person.SerializeToString(&sendmsg);
    pub.Send(g_topic, sendmsg);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  t.join();
}

TEST(pubsub, proto_test) {
  {
    Person person;
    person.set_name("John Doe");
    person.set_id(0);
    person.set_email("john.doe@example.com");
    std::string s;
    google::protobuf::util::MessageToJsonString(person, &s);
    std::cout << s << std::endl;
  }

  ProtoSubTest("tcp://127.0.0.1:5555");
}
