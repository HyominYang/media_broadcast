#include "env.h"
#include <fstream>
#include <mutex>
#include "glog/logging.h"

struct Environment::Data
{
    std::string ip;
    std::string id;
    std::mutex mutex;
};
Environment& Environment::instance()
{
    static Environment env;
    return env;
}
Environment::Environment()
{
    data_ = new Environment::Data;
}
Environment::~Environment()
{
    delete data_;
}
bool Environment::Load() {
  std::lock_guard<std::mutex> lock_guard(data_->mutex);
  Clear();
  std::ifstream f;
  f.open("/etc/ippa.conf");
  if (!f.is_open()) {
    LOG(ERROR)<<"file open error";
    return false;
  }
  f>>data_->ip;
  f>>data_->id;
  f.close();
  LOG(INFO)<<"ID("<<data_->id<<"), SERVERADDR("<<data_->ip<<")";
  return true;
}
void Environment::Clear() {
  data_->ip.clear();
  data_->id.clear();
}
std::string Environment::ip() const
{
  return data_->ip;
}
std::string Environment::id() const
{
  return data_->id;
}
