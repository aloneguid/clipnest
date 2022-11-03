#pragma once
#include "operation.h"
#include "grey.h"
#include <memory>

namespace clipnest {
    class mini_result_popup : public grey::window {
    public:
        mini_result_popup(grey::texture_mgr& mgr);

        static std::unique_ptr<grey::backend> show(bool& is_ui_open);

    private:
        std::shared_ptr<grey::complex_table<operation>> tbl;
        operation::sopvec results;

        void calculate();
    };
}