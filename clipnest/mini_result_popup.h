#pragma once
#include "grey.h"
#include <memory>

namespace clipnest {
    class mini_result_popup : public grey::window {
    public:
        mini_result_popup(grey::texture_mgr& mgr);

        static std::unique_ptr<grey::backend> show();
    };
}