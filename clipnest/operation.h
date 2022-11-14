#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <functional>

namespace clipnest {

#define GEN_OP(cat, id, name, calc_fn_body) operation::add(cat, id, name, \
    [](const operation_input&) { return true;}, [](const operation_input& input) { return calc_fn_body; });

    class operation_input {
    public:
        operation_input() : is_file{ false } {}
        operation_input(const std::string& value);

        std::string value;
        bool is_file;
    };

	class operation {
    public:
        typedef std::shared_ptr<operation> sop;
        typedef std::vector<sop> sopvec;

        operation(const std::string& category, const std::string& id, const std::string& name) 
            : category{ category }, id { id }, name{ name } {

        }

        /**
         * @brief Whether this operation can be computed on the input (best efforts, cheap estimation)
         * @param input 
         * @return 
        */
        virtual bool can_compute(const operation_input& input) { return true; }

        virtual void compute(const operation_input& input) = 0;

        const std::string category;
        const std::string id;
        const std::string name;

        // is it expensive to calculate?
        bool is_expensive{ false };

        // when set, indicates operation has not finished calculations
        bool is_dirty{ false };
        std::string result;

        static std::map<std::string, std::vector<std::shared_ptr<operation>>> cat_to_ops;
        static std::map<std::string, std::shared_ptr<operation>> all;

        static void init();

        static std::map<std::string, std::vector<std::shared_ptr<operation>>> compute_all(const operation_input& input);

        static void add(sop op);
        
        static sop add(const std::string& category,
            const std::string& id, const std::string& name,
            std::function<bool(const operation_input&)> can_compute_fn,
            std::function<std::string(const operation_input&)> calc_fn);
	};
}