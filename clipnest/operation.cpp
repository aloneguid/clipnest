#include "operation.h"
#include "str.h"
#include <functional>

using namespace std;

namespace clipnest {

    map<string, shared_ptr<operation>> operation::all;
    map<string, vector<shared_ptr<operation>>> operation::cat_to_ops;
    const string CatNone = "";
    const string CatEncoding = "encode";
    const string CatBasics = "basics";
    const string CatNumbers = "numbers";

    // Custom operations

    class lambda_op : public operation {
    public:
        lambda_op(const string& category, const string& id, const string& name, function<string(const string&)> fn) 
            : operation(category, id, name), fn{ fn } {
        }

        void compute(const string& input) {
            result = fn(input);
        }

    private:
        function<string(const string&)> fn;
    };


    // Operation members

    void operation::init() {
        add("len", "length", 
            [](const string& input) { return to_string(input.length()); }, CatNone);

        add("wc", "words", 
            [](const string& input) {
            size_t wc = str::word_count(input);
            return wc > 0 ? to_string(wc) : string("");
        }, CatNone);

        add("to_base64", "to base64",
            [](const string& input) {
            return str::base64_encode((const unsigned char*)input.c_str(), input.length());
        }, CatEncoding);

        add("from_base64", "from base64",
            [](const string& input) {
            return str::base64_decode(input);
        }, CatEncoding);

        add("upper", "to upper", [](const string& input) {
            string r = input;
            str::upper(r);
            return r;
        }, CatBasics);

        add("lower", "to lower", [](const string& input) {
            string r = input;
            str::lower(r);
            return r;
        }, CatBasics);

        add("filesize", "size", [](const string& input) {
            string r;
            try {
                long l = stol(input);
                return str::to_human_readable_size(static_cast<unsigned long>(l));
            } catch(...) {
            }
            return r;
        }, CatNumbers);
    }

    void operation::compute_all(const string& input) {
        for (auto& op : all) {
            op.second->compute(input);
        }
    }

    void operation::add(
        const std::string& id, const std::string& name, function<string(const string&)> fn, const std::string& category) {
        auto ptr = std::make_shared<lambda_op>(category, id, name, fn);
        all[ptr->id] = ptr;

        const std::string& cat = ptr->category;
        auto& vec = cat_to_ops[cat];
        vec.push_back(ptr);
    }
}