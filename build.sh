# rm -r ./build

sh ./src/proto/build.sh

mkdir build 

cd build

cmake .. && make -j4