//
// Created by wind on 20. 3. 24..
//

#include <pthread.h>
#include <unistd.h>
#include <zmq.hpp>
#include <iostream>
#include <vector>
#include <mutex>
#include <deque>

#include <signal.h>

#include <glog/logging.h>

#include "util/protocol/protocol.h"

class MicWorker {
 public:
  ~MicWorker()
  {
    Off();
  }
  static MicWorker& instance()
  {
    static MicWorker instance;
    return instance;
  }
  bool On()
  {
    Off();
    // todo: make env info
    handle_ = fork();
    if (handle_ == 0) {
      std::vector<std::string> params;
      params.push_back("/usr/local/bin/mic_broadcast");
      char ** argv = new char*[params.size()+1];
      for(size_t counter=0; counter<params.size(); ++counter) {
        argv[counter] = const_cast<char*>(params[counter].c_str());
      }
      argv[params.size()] = NULL;
      execvp(argv[0], argv);
      PLOG(ERROR)<<"mic on failed";
      exit(0);
    }
    else if (handle_ < 0) {
      PLOG(ERROR)<<"create handle error";
      return false;
    }
    return true;
  }
  void Off()
  {
    if (handle_ > 0) {
      if (kill(handle_, 15)) {
        PLOG(ERROR);
      }
      handle_ = -1;
      sleep(1);
      system("/usr/local/bin/mic_broadcast_off");
      sleep(1);

    }
  }
 private:
  MicWorker()
  : handle_(-1)
  {}
  pid_t handle_;
};
class ControlData {
 public:
  ControlData()
  {}
  void append_broadcast_message(const zmq::message_t &message)
  {
    zmq::message_t *m = new zmq::message_t(message.size());
    memcpy(m->data(), message.data(),message.size());
    append_broadcast_message(m);
  }
  void append_broadcast_message(zmq::message_t *message)
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
  static ControlData& instance() {
    static ControlData control_data;
    return control_data;
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
  std::string req_str(req.to_string());
  LOG(INFO)<<"REQ: "<<req_str;
  uint32_t code = protocol::GetCode(req);
	std::string id(protocol::GetID(req), protocol::Code::ID_SIZE);
  LOG(INFO)<<id<<"|"<<std::setw(8)<<std::hex<<std::setfill('0')<<code<<"|"<<protocol::CodeToString(code);
  if (code == protocol::Code::kKeepAlive) {
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kIdentification) {
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kBroadcastMicOpen) {
    MicWorker::instance().On();
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kBroadcastMicOpenSuccess) {
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kBroadcastMicClose) {
    MicWorker::instance().Off();
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kBroadcastMicCloseSuccess) {
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kBroadcastTextType1) {
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  LOG(INFO)<<"not processing....";
  return NULL;
}
void* RequestClientWorker(void* param)
{
  ControlData *data = static_cast<ControlData*>(param);
  while(true) {
    zmq::context_t ctx(1);
    zmq::socket_t socket(ctx, zmq::socket_type::rep);
    socket.bind("tcp://*:5555");
    LOG(INFO)<<"reply-server start";
    while (true) {
      try {
        zmq::message_t req;
        socket.recv(req);
        zmq::message_t *rep = RequestProcedure(req);
        if (!rep) {
          break;
        }
        LOG(INFO)<<"reply... ("<<rep->size()<<")";
        socket.send(*rep);
        delete rep;
        data->append_broadcast_message(req);
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
    zmq::socket_t socket(ctx, zmq::socket_type::pub);
    socket.bind("tcp://*:5556");
    while (true) {
      sleep(1);
      try {
        zmq::message_t *msg;
        do {
          usleep(1000000);
          msg = data->get_broadcast_message();
          if (msg) {
            LOG(INFO)<<"broad-casting...";
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
  pthread_t reply_server_worker;
  pthread_t broadcast_server_worker;
  pthread_create(&reply_server_worker, NULL, RequestClientWorker, &data);
  pthread_create(&broadcast_server_worker, NULL, BroadCastServerWorker, &data);
  pthread_join(reply_server_worker, NULL);
  pthread_join(broadcast_server_worker, NULL);
  return 0;
}
