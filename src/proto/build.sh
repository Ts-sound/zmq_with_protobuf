#!/bin/bash

# 获取当前脚本所在的目录
SCRIPT_DIR=$(dirname "$0")

cd $SCRIPT_DIR && mkdir include  && protoc --cpp_out=./include ./*.proto