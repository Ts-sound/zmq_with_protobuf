sh ./build.sh

./build/src/test/bin/project_test --gtest_filter=pubsub.pubsub_test

./build/src/test/bin/project_test --gtest_filter=pubsub.pubsub_ipc_test

./build/src/test/bin/project_test --gtest_filter=pubsub.async_sub_test

./build/src/test/bin/project_test --gtest_filter=pubsub.proto_test

./build/src/test/bin/project_test --gtest_filter=poller.poller_pubsub_test

./build/src/test/bin/project_test --gtest_filter=poller.poller_server_client_test