//
// Created by wind on 20. 3. 28..
//

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <zmq.hpp>
#include <iostream>
#include <memory>
#include <mutex>
#include <deque>

#include "glog/logging.h"
#include "util/check_id.h"
#include "util/env.h"
#include "util/protocol/protocol.h"

#define BROADCAST_PORT ":5556"
#define CONTROL_PORT ":5555"
class MediaListenWorker
{
 public:
  ~MediaListenWorker()
  {
    Off();
  }
  static MediaListenWorker& instance() {
    static MediaListenWorker instance;
    return instance;
  }
  enum ListenerTypeCode {
    kTypeNone=0,
    kTypeMic,
  };
  void Off(const ListenerTypeCode &type = kTypeNone)
  {
    if (handle_ > 0) {
      if (kill(handle_, 15)) {
        PLOG(ERROR);
      }
      handle_ = -1;
      sleep(1);
      ListenerTypeCode off_type = type == kTypeNone ? listen_type_ : type;

      if (off_type == kTypeMic) {
        system("/usr/local/bin/mic_broadcast_off");
      }
      sleep(1);
    }
  }
  bool On(const ListenerTypeCode &type) {
    Off(listen_type_);
    if (type == kTypeNone) {
      LOG(ERROR)<<"listen-type is wrong";
      return false;
    }
    handle_ = fork();
    if (handle_ == 0) {
      std::vector<std::string> params;
      if (type == kTypeMic) {
        params.push_back("/usr/local/bin/mic_broadcast");
        params.push_back(Environment::instance().ip());
      } else {
        return 0;
      }
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
 private:
  MediaListenWorker()
  : handle_(-1)
  {}
  ListenerTypeCode listen_type_;
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
 private:
  std::mutex lock_broadcast_queue_;
  std::deque<zmq::message_t*> broadcast_queue_;
};
zmq::message_t *BroadCastProcedure(zmq::message_t &req)
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
    LOG(INFO)<<"listen media on";
    MediaListenWorker::instance().On(MediaListenWorker::kTypeMic);
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kBroadcastMicClose) {
    return protocol::MakeReply(protocol::Code::kResultOk);
  }
  if (code == protocol::Code::kBroadcastMicCloseSuccess) {
    LOG(INFO)<<"listen media close";
    MediaListenWorker::instance().Off(MediaListenWorker::kTypeMic);
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
  do {
    zmq::context_t ctx(1);
    zmq::socket_t socket(ctx, zmq::socket_type::req);
    socket.connect(Environment::instance().ip() + CONTROL_PORT);
    std::unique_ptr<zmq::message_t> msg(
        protocol::MakeRequest(protocol::Code::kIdentification, Environment::instance().id().c_str()));
    socket.send(*msg);
    msg.reset(new zmq::message_t);
    socket.recv(*msg);
    LOG(INFO)<<"reply-server start";
    std::cout<<"Control-server is closed";
    socket.close();
//    std::cout<<"Control-server is restart";
    sleep(1);
  } while (0);
  return NULL;
}
void* BroadCastServerWorker(void* param)
{
  ControlData *data = static_cast<ControlData*>(param);
  while(true) {
    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::sub);
    socket.connect(Environment::instance().ip() + BROADCAST_PORT);
    std::unique_ptr<zmq::message_t> msg;
    while (true) {
      sleep(1);
      try {
        do {
          msg.reset(new zmq::message_t);
          socket.recv(*msg);
          std::string req_id(protocol::GetID(*msg), protocol::ID_SIZE);
          if (!util::check_id(Environment::instance().id().c_str(), req_id.c_str(), protocol::ID_SIZE)) {
            continue;
          }
          BroadCastProcedure(*msg);
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
void* MediaListenWorker(void* param)
{
  while (true)
  {
    sleep(1);
  }
  return NULL;
}
int main(int argc, char **argv)
{
  if (!Environment::instance().Load()) {
    LOG(ERROR)<<"load conf error"<<std::endl;
    return 0;
  }
  ControlData data;
  pthread_t request_client_worker;
  pthread_t broadcast_server_worker;
  pthread_t media_listen_worker;
  pthread_create(&request_client_worker, NULL, RequestClientWorker, &data);
  pthread_create(&broadcast_server_worker, NULL, BroadCastServerWorker, &data);
  pthread_create(&media_listen_worker, NULL, MediaListenWorker, &data);
  pthread_join(request_client_worker, NULL);
  pthread_join(broadcast_server_worker, NULL);
  return 0;
}

