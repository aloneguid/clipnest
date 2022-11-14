#pragma once
#include "operation.h"
#include "grey.h"
#include <memory>
#include "ext/alg_tracker.h"

namespace clipnest {
    class mini_result_popup : public grey::window {
    public:
        mini_result_popup(grey::texture_mgr& mgr, alg::tracker& t);

        static std::unique_ptr<grey::backend> show(bool& is_ui_open, alg::tracker& t);

    private:
        std::string last_input;
        std::shared_ptr<grey::label> lbl_input;
        std::shared_ptr<grey::complex_table<operation>> tbl;
        operation::sopvec results;
        alg::tracker& t;

        void calculate();
        std::string ellipse(const std::string& s, size_t max = 50);
        void make_result_cell(std::shared_ptr<grey::table_cell> cell, operation::sop op);

    };
}