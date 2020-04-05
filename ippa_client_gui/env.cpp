#include "env.h"
#include <fstream>
#include "glog/logging.h"

struct Environment::Data
{
    std::string ip;
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
std::string Environment::ip() const
{
    if (!data_->ip.empty()) {
        return data_->ip;
    }
    std::ifstream f;
    f.open("/etc/ippa.conf");
    if (!f.is_open()) {
        LOG(ERROR)<<"file open error";
        return std::string();
    }
    f>>data_->ip;
    return data_->ip;
}
