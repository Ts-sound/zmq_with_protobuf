# zmq_with_protobuf

# build docker 

```bash

docker build -t zmq_with_protobuf .

```

# start docker

```bash
docker run -d  --privileged -p 2202:22 -v /opt/tong/ws/zmq_with_protobuf:/opt/tong/ws/zmq_with_protobuf zmq_with_protobuf:latest 
```

docker start <id>
docker attach <id>

# build gtest
在 Debian/Ubuntu 系统中，`gtest`（Google Test）通常可以通过 `libgtest-dev` 包来安装。不过，这个包只包含 Google Test 的源代码，你需要手动编译它。

### 步骤 1: 安装 `libgtest-dev`

首先，你可以安装 `libgtest-dev` 包来获取 Google Test 的源代码。

```bash
sudo apt update
sudo apt install libgtest-dev
```

### 步骤 2: 编译 Google Test

`libgtest-dev` 包安装后，Google Test 的源代码会被下载到 `/usr/src/gtest` 目录中。你需要手动编译 Google Test 并将其安装到系统中，或者你可以只在本地使用。

```bash
cd /usr/src/gtest
cmake CMakeLists.txt && make -j install

```

这样，Google Test 就会安装在系统中，通常会安装到 `/usr/local/lib` 和 `/usr/local/include` 中。

### 步骤 3: 在项目中使用 Google Test

在你的 CMake 项目中使用 Google Test 时，你可以像这样链接 Google Test：

```cmake
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(my_test my_test.cpp)
target_link_libraries(my_test ${GTEST_LIBRARIES} pthread)
```

### 总结

- 使用 `libgtest-dev` 安装 Google Test 的源代码。
- 手动编译和安装 Google Test。
- 在项目中通过 CMake 使用 Google Test。

这应该能够帮助你在 Ubuntu 系统中安装并使用 Google Test。如果你需要更简便的方法，也可以考虑直接使用 `vcpkg` 或 `conan` 等包管理器来管理 Google Test。