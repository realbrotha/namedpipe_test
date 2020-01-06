//
// Created by realbro on 1/6/20.
//

#ifndef TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETFACTORY_H_
#define TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETFACTORY_H_

class UnixDomainSocketFactory
{
 public:
  UnixDomainSocketFactory() {};
  virtual ~UnixDomainSocketFactory() {};
  virtual void Release()
  {
    delete this;
  }
};

#endif //TESTEPOLLPIPE_UNIXDOMAINSOCKET_INCLUDE_UNIXDOMAINSOCKETFACTORY_H_
