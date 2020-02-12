//
// Created by realbro on 2/11/20.
//
#include "boost/asio.hpp"
#include <iostream>

class Acceptor {
 public:
  Acceptor(boost::asio::io_service& ios, unsigned short port_num) : ios_(ios),
                                                                   acceptor_(ios_,
                                                                             boost::asio::ip::tcp::endpoint(
                                                                                 boost::asio::ip::address_v4::any(),
                                                                                 port_num)),
                                                                   is_stopped(false) { ; }

  void Start() {
    acceptor_.listen();
  }
  void Stop() {
    is_stopped.store(true);
  }

 private :
  void InitAccept() {
    std::shared_ptr<boost::asio::ip::tcp::socket> sock(new boost::asio::ip::tcp::socket(ios_));
    acceptor_.async_accept(*sock.get(),
        [this,sock](const boost::system::error_code& error)
    {
      OnAccept(error,sock);
    });
  }
  void OnAccept(const boost::system::error_code &ec, std::shared_ptr<boost::asio::ip::tcp::socket> sock) {
    if (!ec) {
      // Start
    } else {
      std::cout << "Error !!!!";
    }
  }

  boost::asio::io_service& ios_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::atomic<bool> is_stopped;
};

class ServerProcedure {
 public :
  ServerProcedure(std::shared_ptr<boost::asio::ip::tcp::socket> sock) : sock_(sock) {

  }
  void Start() {
    boost::asio::async_read_until(*sock_.get(), request_, '\n',
                                  [this](const boost::system::error_code &ec, std::size_t bytes_transferred) {
                                    onRequestReceived(ec, bytes_transferred);
                                  });
  }

 private :
  void onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred) {
  if (ec)
  {
   std::cout << "error !!";
    Shutdown();
    return;
  }
  response_ = ProcessRequest(request_);
    boost::asio::async_write(*sock_.get(), boost::asio::buffer(response_),
                             [this](
                                 const boost::system::error_code &ec,
                                 std::size_t bytes_transferred) {
                               onResponseSent(ec, bytes_transferred);
                             });
  }
  void onResponseSent(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    if (ec) {
      std::cout << "error !!";
      Shutdown();
    }
  }
  std::string ProcessRequest(boost::asio::streambuf& request) {
      std::string response = "Response\n";
      return response;
  }

  void Shutdown()
  {
    delete this;
  }
  std::shared_ptr<boost::asio::ip::tcp::socket> sock_;
  std::string response_;
  boost::asio::streambuf request_;
};