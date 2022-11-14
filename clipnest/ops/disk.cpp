#include "disk.h"
#include "../operation.h"
#include "fss.h"

using namespace clipnest;
using namespace std;

namespace clipnest::ops {

    void disk::init() {
        add("f_size", "file size", [](const operation_input& input) {
            auto size = fss::get_file_size(input.value);
            if (size == -1) return string{ "" };

            return std::to_string(size);
        });

        /*add("f_md5", "file MD5", [](const operation_input& input) {
            return "todo";
        });*/
    }

    void disk::add(const std::string& id, const std::string& name, std::function<std::string(const operation_input&)> fn) {
        auto op = operation::add("Files", id, name,
            [](const operation_input& i) { return i.is_file; },
            fn);
        op->is_expensive = true;
    }
}