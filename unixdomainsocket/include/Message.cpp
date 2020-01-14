//
// Created by realbro on 1/10/20.
//

#include "Message.h"

#include <cstring>
MessageParser::MessageParser(const uint8_t *message, const size_t message_length) {
  Parse(message, message_length);
}
MessageParser::MessageParser(Message &message) {
  Parse(message);
}
MessageParser::~MessageParser() {

}

bool MessageParser::Parse(Message &message) {
  //  TODO : 에러처리 ??? 사용이 명확하다면 세그먼트 폴트는 피할수 있음
  message_header_ptr = reinterpret_cast<const MessageHeader *>(&message.GetRawData()[0]);

  message_header_.header_checker = message_header_ptr->header_checker;
  message_header_.message_length = message_header_ptr->message_length;
  message_header_.is_request = message_header_ptr->is_request;
  message_header_.priority = message_header_ptr->priority;
  message_header_.request_timeout = (message_header_ptr->request_timeout);
  message_header_.listener_type = (message_header_ptr->listener_type);
  message_header_.message_id = (message_header_ptr->message_id);

  data_ = reinterpret_cast<const uint8_t *>(&message_header_ptr[1]);
  data_length = message_header_.message_length - sizeof(MessageHeader);

  return true;
}
bool MessageParser::Parse(const uint8_t *message, const size_t message_length) {
  if (message == NULL || message_length < sizeof(MessageHeader)) {
    std::cout << "Parse failed\n" << std::endl;
    return false;
  }
  message_header_ptr = reinterpret_cast<const MessageHeader *>(message);
  message_header_.header_checker = message_header_ptr->header_checker;
  message_header_.message_length = message_header_ptr->message_length;
  message_header_.is_request = message_header_ptr->is_request;
  message_header_.priority = message_header_ptr->priority;
  message_header_.request_timeout = (message_header_ptr->request_timeout);
  message_header_.listener_type = (message_header_ptr->listener_type);
  message_header_.message_id = (message_header_ptr->message_id);

  data_ = reinterpret_cast<const uint8_t *>(&message_header_ptr[1]);
  data_length = message_header_.message_length - sizeof(MessageHeader);
  printf("Parser --> length : %d\n", data_length);
  return true;
}

const uint8_t *MessageParser::GetData() {
  return data_;
}
bool MessageParser::IsHeader() {
  return (message_header_.header_checker == 0x98765432) ? true : false;
}
int32_t MessageParser::GetDataSize() const{
  return data_length;
}
int32_t MessageParser::GetMessageFullSize() const {
  return message_header_.message_length;
}
int16_t MessageParser::GetListenerType() const {
  return message_header_.listener_type;
}
int32_t MessageParser::GetMessageId() const {
  return message_header_.message_id;
}
int16_t MessageParser::GetIsRequest() const {
  return message_header_.is_request;
}
int16_t MessageParser::GetPriority() const {
  return message_header_.priority;
}
int16_t MessageParser::GetRequestTimeOut() const {
  return message_header_.request_timeout;
}
Message::Message(const char *data,
                 int32_t data_length,
                 int16_t listener_type,
                 int32_t message_id,
                 int16_t is_request,
                 int16_t priority,
                 int16_t request_timeout) {
  if (0 > data_length) {
    printf("error\n");
    return;
  }
  message_.clear();
  message_.resize(sizeof(MessageHeader) + data_length);
  message_size_ = sizeof(MessageHeader) + data_length;

  SetHeader(message_.size(), listener_type, message_id, is_request, priority, request_timeout);
  SetBody(data, data_length);
}
Message::Message(const uint8_t *buff, const size_t message_size) {
  message_.clear();
  message_.resize(message_size);
  message_size_ = message_size;

  MessageHeader *h = reinterpret_cast<MessageHeader *>(&message_[0]);
  memcpy(reinterpret_cast<void *>(&message_[0]), buff, message_size);
}
Message::~Message() {

}
std::vector<uint8_t> &Message::GetRawData() {
  return message_;
}

void Message::AppendData(const char *data, int32_t data_size) {
  size_t message_size_buffer = message_size_;
  size_t needed_size = message_size_buffer + data_size;
  if (message_.capacity() < needed_size) {
    message_.resize(needed_size);
    message_size_ = needed_size;
  }
  memcpy(reinterpret_cast<void *>(&message_[message_size_buffer]), data, data_size);
}

void Message::SetHeader(int32_t message_length,
                        int16_t listener_type,
                        int32_t message_id,
                        int16_t is_request,
                        int16_t priority,
                        int16_t requestTimeout) {
  MessageHeader *h = reinterpret_cast<MessageHeader *>(&message_[0]);
  h->header_checker = 0x98765432;
  h->message_length = message_length;
  h->is_request = is_request;
  h->priority = priority;
  h->request_timeout = requestTimeout;
  h->listener_type = listener_type;
  h->message_id = message_id;

}
void Message::SetBody(const char *data, const int size) { // SetHeader Dependency Api
  if (data == NULL || size == 0) {
    return;
  }
  MessageHeader *h_ = reinterpret_cast<MessageHeader *>(&message_[0]);
  memcpy(reinterpret_cast<void *>(&h_[1]), data, size);
}

bool MessageManager::Add(int32_t product_type, Message msg, int32_t message_id) {
  if (!product_type || !message_id)
    return false;

  if (message_map_.count(product_type) &&
      message_map_[product_type].count(message_id)) { // 메세지가 있다면..
    return false;
  } else {
    std::map<int32_t, Message> msg_buffer;
    message_map_[product_type] = msg_buffer;
  }

  return true;
}
bool MessageManager::Remove(int32_t product_type, Message msg, int32_t message_id) {
  if (message_map_.count(product_type) &&
      message_map_[product_type].count(message_id)) {
    message_map_[product_type].erase(message_id);
    return true;
  }
  return false;
}
bool MessageManager::IsPerfectMessage(int32_t product_type, int32_t message_id) {
  if (message_map_.count(product_type) &&
      message_map_[product_type].count(message_id)) {
    //MessageParser message_parser(message_map_[product_type][message_id]);
    //message_parser.
  }
}
