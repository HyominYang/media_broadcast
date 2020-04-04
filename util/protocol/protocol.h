//
// Created by wind on 20. 4. 4..
//

#ifndef GSTREAMER_PRATICE_IPPA_SERVER_PROTOCOL_H_
#define GSTREAMER_PRATICE_IPPA_SERVER_PROTOCOL_H_

#include <cstdint>
#include <iostream>
#include <thirdparty/include/zmq/zmq.hpp>

namespace protocol {
enum Code
{
  kKeepAlive                 = 0x00000000,
  kIdentification            = 0x00000001,
  kBroadcastMicOpen          = 0x00000100,
  kBroadcastMicOpenSuccess   = 0x00000101,
  kBroadcastMicClose         = 0x00000102,
  kBroadcastMicCloseSuccess  = 0x00000103,
  kBroadcastTextType1        = 0x00001000,
  kResultOk                  = 0x00000000,
  kResultError               = 0x00000001,
  ID_SIZE = 8,
  CODE_SIZE = sizeof(uint32_t),
  REQ_MINIMUM_SIZE = CODE_SIZE+ID_SIZE,
  RESULT_SIZE = sizeof(uint32_t),
  REP_MINIMUM_SIZE = RESULT_SIZE
};
std::string CodeToString(const uint32_t &code);
bool CheckValidation(const zmq::message_t &message);
uint32_t GetCode(const zmq::message_t &message);
const char* GetID(const zmq::message_t &message);
zmq::message_t *MakeRequest(const uint32_t &code, const char *id, const void *data=NULL, const size_t &data_size=0);
zmq::message_t *MakeReply(const uint32_t &result, const void *data=NULL, const size_t &data_size=0);
}

#endif //GSTREAMER_PRATICE_IPPA_SERVER_PROTOCOL_H_
