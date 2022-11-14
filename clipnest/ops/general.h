#pragma once
#include <functional>
#include <string>
#include "../operation.h"

namespace clipnest::ops {
    class general {
    public:
        static void init();

    private:
        static void add(const std::string& cat, const std::string& id, const std::string& name,
            std::function<std::string(const operation_input&)> calc_fn);
    };
}