#include "operation.h"
#include "str.h"
#include "fss.h"
#include <functional>
#include <fmt/core.h>
#include "ops/general.h"
#include "ops/disk.h"

using namespace std;

namespace clipnest {

    map<string, shared_ptr<operation>> operation::all;
    map<string, vector<shared_ptr<operation>>> operation::cat_to_ops;

    operation_input::operation_input(const std::string& value) : value{ value }, is_file{ fss::file_exists(value) } {

    }

    // Custom operations

    class lambda_op : public operation {
    public:
        lambda_op(const string& category, const string& id, const string& name,
            function<bool(const operation_input&)> can_compute_fn,
            function<string(const operation_input&)> fn)
            : operation(category, id, name), can_compute_fn{ can_compute_fn }, fn { fn } {
        }

        bool can_compute(const operation_input& input) {
            return can_compute_fn(input);
        }

        void compute(const operation_input& input) {
            result = fn(input);
        }

    private:
        function<bool(const operation_input&)> can_compute_fn;
        function<string(const operation_input&)> fn;
    };


    // Operation members

    void operation::init() {
        clipnest::ops::general::init();
        clipnest::ops::disk::init();
    }

    std::map<std::string, std::vector<std::shared_ptr<operation>>> operation::compute_all(const operation_input& input) {

        bool is_too_large = input.value.length() > 1000;

        std::map<std::string, std::vector<std::shared_ptr<operation>>> result;

        for (auto& op : all) {

            vector<sop> rcat;

            auto o = op.second;
            o->result.clear();

            if (o->can_compute(input)) {
                if (is_too_large || o->is_expensive) {
                    o->is_dirty = true;
                } else {
                    try {
                        o->compute(input);
                        o->is_dirty = false;
                        rcat.push_back(o);
                    } catch (...) {
                    }
                }
            }

            if (!rcat.empty()) {
                result[op.first] = rcat;
            }
        }

        return result;
    }

    void operation::add(sop op)
    {
        all[op->id] = op;

        const std::string& cat = op->category;
        auto& vec = cat_to_ops[cat];
        vec.push_back(op);
    }

    operation::sop operation::add(const std::string& category, const std::string& id, const std::string& name, std::function<bool(const operation_input&)> can_compute_fn, std::function<std::string(const operation_input&)> calc_fn)
    {
        auto ptr = std::make_shared<lambda_op>(category, id, name, can_compute_fn, calc_fn);
        add(ptr);
        return ptr;
    }
}