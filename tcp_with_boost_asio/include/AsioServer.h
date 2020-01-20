//
// Created by realbro on 1/20/20.
//

#ifndef TESTIPC_TCP_WITH_BOOST_ASIO_INCLUDE_ASIOSERVER_H_
#define TESTIPC_TCP_WITH_BOOST_ASIO_INCLUDE_ASIOSERVER_H_

class AsioServer {
 public :
  AsioServer();
  virtual ~AsioServer();

  virtual bool Initialize();
  virtual bool Finalize();

};

#endif //TESTIPC_TCP_WITH_BOOST_ASIO_INCLUDE_ASIOSERVER_H_
