#include "operation.h"
#include "str.h"

using namespace std;

namespace clipnest {

    map<string, shared_ptr<operation>> operation::all;
    map<string, vector<shared_ptr<operation>>> operation::cat_to_ops;
    const string CatNone = "";
    const string CatEncoding = "encode";

    // Custom operations

    class length_op : public operation {
    public:
        length_op() : operation(CatNone, "len", "length") {}

        void compute(const string& input) {
            result = to_string(input.length());
        }
    };

    class word_count_op : public operation {
    public:
        word_count_op() : operation(CatNone, "wc", "words") {}

        void compute(const string& input) {
            result = to_string(str::word_count(input));
        }
    };

    class base64_op : public operation {
    public:
        base64_op() : operation(CatEncoding, "base64", "to base64") {}

        void compute(const string& input) {
            result = str::base64_encode((const unsigned char*)input.c_str(), input.length());
        }
    };

    // Operation members

    void operation::init() {
        add<length_op>();
        add<word_count_op>();
        add<base64_op>();
    }

    void operation::compute_all(const string& input) {
        for (auto& op : all) {
            op.second->compute(input);
        }
    }
}