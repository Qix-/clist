FROM alpine:3.7 AS base_alpine
WORKDIR /src
RUN apk --update add gcc g++ musl-dev cmake make ninja
COPY . ./
WORKDIR /src/build

FROM ubuntu:18.04 AS base_ubuntu
WORKDIR /src
RUN apt update
RUN apt install -y build-essential cmake ninja-build
COPY . ./
WORKDIR /src/build

FROM base_alpine
RUN cmake .. -GNinja -DBUILD_TESTING=On -DAUTOTEST_MEMCHECK=Off -DCMAKE_BUILD_TYPE=Debug
RUN ninja
RUN ctest
RUN test/clist-benchmark

FROM base_alpine
RUN cmake .. -GNinja -DBUILD_TESTING=On -DAUTOTEST_MEMCHECK=Off -DCMAKE_BUILD_TYPE=Release
RUN ninja
RUN ctest
RUN test/clist-benchmark

FROM base_ubuntu
RUN cmake .. -GNinja -DBUILD_TESTING=On -DCMAKE_BUILD_TYPE=Debug
RUN ninja
RUN ctest
RUN test/clist-benchmark

FROM base_ubuntu
RUN cmake .. -GNinja -DBUILD_TESTING=On -DCMAKE_BUILD_TYPE=Release
RUN ninja
RUN ctest
RUN test/clist-benchmark
