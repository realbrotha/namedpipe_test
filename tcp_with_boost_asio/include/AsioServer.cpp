//
// Created by realbro on 1/20/20.
//
#include <stdio.h>

#include "AsioServer.h"

AsioServer::AsioServer() {

}
AsioServer::~AsioServer() {

}

bool AsioServer::Initialize() {
  constexpr int kPORT_NUM = 11234;
  unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;
  if (!thread_pool_size) thread_pool_size = 2;

  worker_.reset(new boost::asio::io_service::work(ios_));
  Run(kPORT_NUM, thread_pool_size);

}
bool AsioServer::Finalize() {

}

void AsioServer::Run(const int port_num, const int thread_pool_size) {
  printf ("run called\n");
  acceptor_.reset(new Acceptor(ios_, port_num));
  acceptor_->Start();

  for (int i = 0; i<thread_pool_size; ++i)
  {
    std::unique_ptr<std::thread> th(
        new std::thread([this](){
          ios_.run();
    }));
    printf ("thread pool push\n");
    thread_pool_.push_back(std::move(th));
  }
}
void AsioServer::Stop() {
  acceptor_->Stop();
  ios_.stop();

  for (auto& th : thread_pool_)
    th->join();
}
