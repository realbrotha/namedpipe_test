//
// Created by realbro on 1/20/20.
//

#ifndef TESTIPC_TCP_WITH_BOOST_ASIO_INCLUDE_ASIOFACTORY_H_
#define TESTIPC_TCP_WITH_BOOST_ASIO_INCLUDE_ASIOFACTORY_H_

class AsioFactory {
 public:
  AsioFactory() {};
  virtual ~AsioFactory() {};
  virtual void Release() {
    delete this;
  }
};

#endif //TESTIPC_TCP_WITH_BOOST_ASIO_INCLUDE_ASIOFACTORY_H_
