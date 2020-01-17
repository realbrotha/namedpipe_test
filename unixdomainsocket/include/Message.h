//
// Created by realbro on 1/10/20.
//

#ifndef TESTIPC_UNIXDOMAINSOCKET_INCLUDE_MESSAGE_H_
#define TESTIPC_UNIXDOMAINSOCKET_INCLUDE_MESSAGE_H_

#include <sys/types.h>

#include <map>
#include <cstdint>
#include <vector>
#include <iostream>

struct MessageHeader {
  int32_t header_checker;
  int32_t message_length;
  int16_t is_request;
  int16_t need_response;         //< 0: normal, 1: high
  int16_t request_timeout;
  int16_t listener_type;    //< message type
  int32_t message_id;       // message key
}__attribute__((packed));

class Message;
class MessageParser {
 public:
  //MessageParser();
  ~MessageParser();
  MessageParser(const uint8_t *message, const size_t message_length);
  MessageParser(Message &message);

  bool IsHeader();
  const uint8_t *GetData();
  int32_t GetHeaderSize() const;
  int32_t GetDataSize() const;
  int32_t GetMessageFullSize() const;
  int16_t GetListenerType() const;
  int32_t GetMessageId() const;
  int16_t GetIsRequest() const;
  int16_t GetNeedResponse() const;
  int16_t GetRequestTimeOut() const;

 private:
  bool Parse(Message &message);
  bool Parse(const uint8_t *message, const size_t message_length);

  const MessageHeader *message_header_ptr;

  MessageHeader message_header_;
  const uint8_t *data_;
  int32_t data_length;
};

class Message {
 public:
  friend class MessageParser;
  friend class MessageManager;
  Message();
  Message(const uint8_t *buff, const size_t message_size);
  Message(const char *data,
          int32_t data_length,
          int16_t listener_type,
          int32_t message_id,
          int16_t need_response = 0,
          int16_t is_request = 0,
          int16_t request_timeout = 0);
  ~Message();
  std::vector<uint8_t> &GetRawData();
  void AppendData(const char *data, int32_t data_size);
  int32_t GetMessageSize() const;
 private:
  void SetHeader(int32_t message_length,
                 int16_t listener_type,
                 int32_t message_id,
                 int16_t need_response,
                 int16_t is_request,
                 int16_t requestTimeout);
  void SetBody(const char *data, const int size);

  size_t message_size_;
  std::vector<uint8_t> message_;
};

class MessageManager {
 public :
  bool Add(int32_t& product_type, Message& msg);
  bool Remove(int32_t& product_type);
  bool IsPerfectMessage(int32_t& product_type);
  bool IsExistMessage(int32_t& product_type);
  Message MessagePop(int32_t& product_type);
 private :
  std::map<int32_t, Message> message_map_;
};
#endif //TESTIPC_UNIXDOMAINSOCKET_INCLUDE_MESSAGE_H_
