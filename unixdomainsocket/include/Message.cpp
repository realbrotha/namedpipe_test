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
  message_header_.need_response = message_header_ptr->need_response;
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
  message_header_.need_response = message_header_ptr->need_response;
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
int32_t MessageParser::GetHeaderSize() const{
  return sizeof(message_header_);
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
int16_t MessageParser::GetIsRequest() const { // Sync = 0 , async =1
  return message_header_.is_request;
}
int16_t MessageParser::GetNeedResponse() const {
  return message_header_.need_response;
}
int16_t MessageParser::GetRequestTimeOut() const {
  return message_header_.request_timeout;
}
Message::Message() {

}
Message::Message(const char *data,
                 int32_t data_length,
                 int16_t listener_type,
                 int32_t message_id,
                 int16_t need_response,
                 int16_t is_request,
                 int16_t request_timeout) {
  if (0 > data_length) {
    printf("error\n");
    return;
  }
  message_.clear();
  message_.resize(sizeof(MessageHeader) + data_length);
  message_size_ = sizeof(MessageHeader) + data_length;

  SetHeader(message_.size(), listener_type, message_id, is_request, need_response, request_timeout);
  SetBody(data, data_length);
}
Message::Message(const uint8_t *buff, const size_t message_size) {
  message_.clear();
  message_.resize(message_size);
  message_size_ = message_size;

  //MessageHeader *h = reinterpret_cast<MessageHeader *>(&message_[0]);
  memcpy(reinterpret_cast<void *>(&message_[0]), buff, message_size);
}
Message::~Message() {

}
std::vector<uint8_t> &Message::GetRawData() {
  return message_;
}

void Message::AppendData(const char *data, int32_t data_size) {
  size_t message_size_buffer = message_size_ ; // null
  size_t needed_size = message_size_buffer + data_size ; // null

  if (message_.size() < needed_size) {
    message_.resize(needed_size );
  }
  message_size_ = needed_size ;
  //constexpr char nullchar = '\0';
  memcpy(reinterpret_cast<void *>(&message_[message_size_buffer]), data, data_size);
  //memcpy(reinterpret_cast<void *>(&message_[needed_size]), &nullchar, 1);

}
int32_t Message::GetMessageSize() const{
  return message_size_;
}
void Message::SetHeader(int32_t message_length,
                        int16_t listener_type,
                        int32_t message_id,
                        int16_t need_response,
                        int16_t is_request,
                        int16_t requestTimeout) {
  MessageHeader *h = reinterpret_cast<MessageHeader *>(&message_[0]);
  h->header_checker = 0x98765432;
  h->message_length = message_length;
  h->is_request = is_request;
  h->need_response = need_response;
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

bool MessageManager::Add(int32_t& product_type, Message& msg) {
  if (0 > product_type)
    return false;

  message_map_[product_type] = msg;
  return true;
}
bool MessageManager::Remove(int32_t& product_type) {
  if (message_map_.count(product_type)) {
    message_map_.erase(product_type);
    return true;
  }
  return false;
}
bool MessageManager::IsPerfectMessage(int32_t& product_type) {
  if (message_map_.count(product_type)) {
    MessageParser message_parser(message_map_[product_type]);
    if (message_parser.GetMessageFullSize() == message_map_[product_type].GetMessageSize())
      return true;
  }
  return false;
}
bool MessageManager::IsExistMessage(int32_t& product_type){
  return (message_map_.count(product_type)) ? true : false;
}

Message MessageManager::MessagePop(int32_t& product_type) {
  Message message_buffer;
  if (message_map_.count(product_type)) {
    message_buffer = message_map_[product_type];
    message_map_.erase(product_type);
  }
  return message_buffer;
}