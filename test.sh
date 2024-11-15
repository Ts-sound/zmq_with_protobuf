sh ./build.sh

./build/src/test/bin/project_test --gtest_filter=pubsub.pubsub_test

./build/src/test/bin/project_test --gtest_filter=pubsub.pubsub_ipc_test

./build/src/test/bin/project_test --gtest_filter=pubsub.async_sub_test
