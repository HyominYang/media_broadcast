//
// Created by wind on 20. 4. 4..
//

#include "ControlMessage.h"
#include <arpa/inet.h>
#include <QDebug>

namespace protocol {
std::string CodeToString(const uint32_t &code)
{
  switch(code) {
    case Code::kKeepAlive: return "KeepAlive";
    case Code::kIdentification: return "Identification";
    case Code::kBroadcastMicOpen: return "BroadcastMicOpen";
    case Code::kBroadcastMicOpenSuccess: return "BroadcastMicOpenSuccess";
    case Code::kBroadcastMicClose: return "BroadcastMicClose";
    case Code::kBroadcastMicCloseSuccess: return "BroadcastMicCloseSuccess";
    case Code::kBroadcastTextType1: return "BroadcastTextType1";
  }
  return "Unknown";
}
bool CheckValidation(const zmq::message_t &message)
{
  if (!message.data()) {
    qDebug()<<"invalid argument";
    return false;
  }
  if (message.size() < Code::REQ_MINIMUM_SIZE) {
    qDebug() << "invalid data size (" << message.size() << "/" << Code::REQ_MINIMUM_SIZE << ")";
    return false;
  }
  return true;
}
uint32_t GetCode(const zmq::message_t &message)
{
  return ntohl(*reinterpret_cast<const uint32_t*>(message.data()));
}
const char* GetID(const zmq::message_t &message)
{
  return reinterpret_cast<const char *>(message.data()) + Code::CODE_SIZE;
}
zmq::message_t *MakeRequest(const uint32_t &code, const char *id, const void *data, const size_t &data_size)
{
  if (!id) {
    qDebug()<<"invalid argument";
    return NULL;
  }
  if (!data && data_size) {
    qDebug()<<"invalid argument";
    return NULL;
  }
  zmq::message_t *message = new zmq::message_t(Code::REQ_MINIMUM_SIZE + data_size);
  uint8_t *message_buff = reinterpret_cast<uint8_t*>(message->data());
  *(reinterpret_cast<uint32_t*>(message_buff)) = htonl(code);
  memcpy(message_buff + Code::CODE_SIZE, id, Code::ID_SIZE);
  if (data_size) {
    memcpy(message_buff + Code::CODE_SIZE + Code::ID_SIZE,
        data, data_size);
  }
  return message;
}
zmq::message_t *MakeReply(const uint32_t &result, const void *data, const size_t &data_size)
{
  if (!data && data_size) {
    qDebug()<<"invalid argument";
    return NULL;
  }
  zmq::message_t *message = new zmq::message_t(Code::REP_MINIMUM_SIZE + data_size);
  uint8_t *message_buff = reinterpret_cast<uint8_t*>(message->data());
  memcpy(message_buff, &result, Code::RESULT_SIZE);
  memcpy(message_buff + Code::RESULT_SIZE, data, data_size);
  return message;
}

}
