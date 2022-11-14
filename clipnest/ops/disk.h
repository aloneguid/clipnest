#pragma once
#include "../operation.h"

namespace clipnest::ops {
    class disk {
    public:
        static void init();

    private:
        static void add(const std::string& id, const std::string& name, std::function<std::string(const operation_input&)> fn);
    };
}