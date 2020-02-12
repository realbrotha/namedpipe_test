//
// Created by realbro on 1/20/20.
//

#ifndef TESTIPC_TCP_WITH_BOOST_ASIO_INCLUDE_ASIOSERVER_H_
#define TESTIPC_TCP_WITH_BOOST_ASIO_INCLUDE_ASIOSERVER_H_

#include <thread>
#include <memory>
#include <atomic>

#include "AsioImpl.hpp"
#include "boost/asio.hpp"

class AsioServer {
 public :
  AsioServer();
  virtual ~AsioServer();

  virtual bool Initialize();
  virtual bool Finalize();

 private:
  void Run(const int port_num, const int thread_pool_size);
  void Stop();

  std::atomic <bool> stopped_;
  std::unique_ptr<boost::asio::io_service::work> worker_;
  std::unique_ptr<Acceptor> acceptor_;
  std::vector<std::unique_ptr<std::thread>> thread_pool_;
  boost::asio::io_service ios_;
};

#endif //TESTIPC_TCP_WITH_BOOST_ASIO_INCLUDE_ASIOSERVER_H_
