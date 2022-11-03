#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <functional>

namespace clipnest {
	class operation {
    public:
        typedef std::vector<std::shared_ptr<operation>> sopvec;

        operation(const std::string& category, const std::string& id, const std::string& name) 
            : category{ category }, id { id }, name{ name } {

        }

        virtual void compute(const std::string& input) = 0;

        const std::string category;
        const std::string id;
        const std::string name;

        std::string result;

        static std::map<std::string, std::vector<std::shared_ptr<operation>>> cat_to_ops;
        static std::map<std::string, std::shared_ptr<operation>> all;

        static void init();

        static void compute_all(const std::string& input);
        
    private:

        template<class TOp> static void add() {
            auto ptr = std::make_shared<TOp>();
            all[ptr->id] = ptr;

            const std::string& cat = ptr->category;
            auto& vec = cat_to_ops[cat];
            vec.push_back(ptr);
        }

        static void add(const std::string& id, const std::string& name,
            std::function<std::string(const std::string&)> fn,
            const std::string& category);
	};
}