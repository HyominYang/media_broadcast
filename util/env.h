#ifndef ENV_H
#define ENV_H

#include <iostream>

class Environment
{
public:
    static Environment& instance();
    ~Environment();
    std::string ip() const;
    std::string id() const;
    bool Load();
private:
    Environment();
    void Clear();
    struct Data;
    struct Data *data_;
};

#endif // ENV_H
