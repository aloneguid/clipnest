#include "operation.h"
#include "str.h"
#include <functional>
#include "win32/cng.h"
#include <nlohmann/json.hpp>
#include <fmt/core.h>

#include "ops/disk.h"

using namespace std;
using json = nlohmann::json;

namespace clipnest {

    map<string, shared_ptr<operation>> operation::all;
    map<string, vector<shared_ptr<operation>>> operation::cat_to_ops;
    const string CatNone = "";
    const string CatEncoding = "encode";
    const string CatBasics = "basics";
    const string CatNumbers = "numbers";
    const string CatHash = "hash";
    const string CatWeb = "web";
    const string CatExtract = "extract";

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

        // todo: move these out elsewhere

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

        add("trim", "trim", [](const string& input) {
            string r = input;
            str::trim(r);
            return r;
        }, CatBasics);

        add("plain", "remove non-ASCII chars", [](const string& input) {
            return str::remove_non_ascii(input);
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

        add("md5", "MD5", [](const string& input) {
            return win32::cng::md5(input);
        }, CatHash);

        add("sha256", "SHA-256", [](const string& input) {
            return win32::cng::sha256(input);
        }, CatHash);

        add("sha512", "SHA-512", [](const string& input) {
            return win32::cng::sha512(input);
        }, CatHash);

        add("html", "strip HTML", [](const string& input) {
            return str::strip_html(input);
        }, CatWeb);

        add("json", "format JSON", [](const string& input) {
            try {
                auto j = json::parse(input);
                return j.dump(4);
            } catch (const json::parse_error& err) {
                return string();
            }
        }, CatWeb);

        add("x-email", "email", [](const string& input) {
            return str::rgx_extract(input, "\\w+([-+.']\\w+)*@\\w+([-.]\\w+)*.\\w+([-.]\\w+)*");
        }, CatExtract);

        add("x-ipv4", "IP v4", [](const string& input) {
            return str::rgx_extract(input, "(\\b25[0-5]|\\b2[0-4][0-9]|\\b[01]?[0-9][0-9]?)(\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)){3}");
        }, CatExtract);

        add("x-ipv4", "IP v6", [](const string& input) {
            return str::rgx_extract(input, "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))");
        }, CatExtract);

        add("dec2hex", "dec -> hex", [](const string& input) {
            try {
                long l = stol(input);
                return fmt::format("{0:x}", l);
            } catch (...) {
                return string();
            }
        }, CatBasics);

        add("hex2dec", "hex -> dec", [](const string& input) {
            try {
                long l = stol(input, nullptr, 16);
                return fmt::format("{0}", l);
            } catch (...) {
                return string();
            }
        }, CatBasics);

        add("dec2bin", "dec -> bin", [](const string& input) {
            try {
                long l = stol(input);
                return fmt::format("{0:b}", l);
            } catch (...) {
                return string();
            }
        }, CatBasics);

        add("bin2dec", "bin -> dec", [](const string& input) {
            try {
                long l = stol(input, nullptr, 2);
                return fmt::format("{0}", l);
            } catch (...) {
                return string();
            }
        }, CatBasics);

        clipnest::ops::disk::init();
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