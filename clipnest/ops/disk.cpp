#include "disk.h"
#include "../operation.h"
#include "fss.h"

using namespace clipnest;
using namespace std;

namespace clipnest::ops {
    void disk::init() {
        add("f_size", "file size", [](const string& input) {
            auto size = fss::get_file_size(input);
            if (size == -1) return string{ "" };

            return std::to_string(size);
        });
    }

    void disk::add(const std::string& id, const std::string& name, std::function<std::string(const std::string&)> fn) {
        operation::add(id, name, fn, "Files");
    }
}