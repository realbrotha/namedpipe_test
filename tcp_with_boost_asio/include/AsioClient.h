//
// Created by realbro on 1/20/20.
//

#ifndef TESTIPC_TCP_WITH_BOOST_ASIO_INCLUDE_ASIOCLIENT_H_
#define TESTIPC_TCP_WITH_BOOST_ASIO_INCLUDE_ASIOCLIENT_H_

class AsioClient {
 public :
  AsioClient();
  virtual ~AsioClient();

  virtual bool Initialize();
  virtual bool Finalize();

};
#endif //TESTIPC_TCP_WITH_BOOST_ASIO_INCLUDE_ASIOCLIENT_H_
