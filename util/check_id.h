//
// Created by wind on 20. 5. 1..
//

#ifndef MEDIA_BROADCAST_UTIL_CHECK_ID_H_
#define MEDIA_BROADCAST_UTIL_CHECK_ID_H_

#include "util/protocol/protocol.h"

namespace util {
inline bool check_id(const char *current_id, const char* request_id, const size_t &id_size)
{
  if (!request_id || id_size != protocol::ID_SIZE) {
    return false;
  }
  bool result = true;
  for (int i=protocol::ID_SIZE-1; i>= 0; --i) {
    if (request_id[i] == '*') {
      continue;
    }
    if (current_id[i] == request_id[i]) {
      continue;
    }
    result = false;
    break;
  }
  return result;
}
}
#endif //MEDIA_BROADCAST_UTIL_CHECK_ID_H_
