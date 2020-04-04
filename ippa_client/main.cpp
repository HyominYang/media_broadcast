//
// Created by wind on 20. 3. 28..
//

#include <pthread.h>
#include <unistd.h>
#include <zmq/zmq.hpp>
#include <iostream>
#include <mutex>
#include <deque>

#include "glog/logging.h"
#include "util/protocol/protocol.h"

#define CONTROL_SERVER_ADDR "tcp://192.168.0.21:5555"
#define BROADCAST_SERVER_ADDR "tcp://192.168.0.21:5556"
class ControlData {
 public:
  ControlData()
  {}
  zmq::message_t* append_broadcast_message(zmq::message_t *message)
  {
    std::lock_guard<std::mutex> lock_guard(lock_broadcast_queue_);
    broadcast_queue_.push_back(message);
  }
  zmq::message_t* get_broadcast_message()
  {
    std::lock_guard<std::mutex> lock_guard(lock_broadcast_queue_);
    if (broadcast_queue_.size() == 0) {
      return NULL;
    }
    zmq::message_t *message = broadcast_queue_.front();
    broadcast_queue_.pop_front();
    return message;
  }
 private:
  std::mutex lock_broadcast_queue_;
  std::deque<zmq::message_t*> broadcast_queue_;
};
zmq::message_t *RequestProcedure(zmq::message_t &req)
{
  if (!protocol::CheckValidation(req)){
    return NULL;
  }
  uint32_t code = protocol::GetCode(req);
  LOG(INFO)<<protocol::GetID(req)<<"|"<<std::setw(8)<<std::hex<<std::setfill('0')<<code<<"|"<<protocol::CodeToString(code);
  if (code == protocol::Code::kKeepAlive) {
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kIdentification) {
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kBroadcastMicOpen) {
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kBroadcastMicOpenSuccess) {
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kBroadcastMicClose) {
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kBroadcastMicCloseSuccess) {
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  return NULL;
}
void* RequestClientWorker(void* param)
{
  while(true) {
    zmq::context_t ctx(1);
    zmq::socket_t socket(ctx, zmq::socket_type::req);
    socket.bind(CONTROL_SERVER_ADDR);
    LOG(INFO)<<"reply-server start";
    while (true) {
      try {
        zmq::message_t req;
        socket.recv(req);
        zmq::message_t *rep = RequestProcedure(req);
        if (rep) {
          LOG(INFO)<<"reply... ("<<rep->size()<<")";
          socket.send(*rep);
          delete rep;
        }
      } catch (zmq::error_t &e) {
        std::cout<<e.what()<<" ("<<e.num()<<")";
        break;
      }
    }
    std::cout<<"Control-server is closed";
    socket.close();
    std::cout<<"Control-server is restart";
    sleep(1);
  }
  return NULL;
}
void* BroadCastServerWorker(void* param)
{
  ControlData *data = static_cast<ControlData*>(param);
  while(true) {
    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::sub);
    socket.bind(BROADCAST_SERVER_ADDR);
    while (true) {
      sleep(1);
      try {
        zmq::message_t *msg;
        do {
          usleep(100000);
          msg = data->get_broadcast_message();
          if (msg) {
            socket.send(*msg);
            delete msg;
          }
        } while(msg != NULL);
      } catch (zmq::error_t &e) {
        std::cout<<e.what()<<" ("<<e.num()<<")";
        break;
      }
    }
    std::cout<<"Broadcast-server is closed";
    socket.close();
    std::cout<<"Broadcast-server is restart";
    sleep(1);
  }
  return NULL;
}
int main(int argc, char **argv)
{
  ControlData data;
  pthread_t request_client_worker;
  pthread_t broadcast_server_worker;
  pthread_create(&request_client_worker, NULL, RequestClientWorker, &data);
  pthread_create(&broadcast_server_worker, NULL, BroadCastServerWorker, &data);
  pthread_join(request_client_worker, NULL);
  pthread_join(broadcast_server_worker, NULL);
  return 0;
}
