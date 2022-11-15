#include "general.h"
#include "win32/cng.h"
#include <nlohmann/json.hpp>
#include "str.h"
#include <fmt/core.h>

using namespace std;
using json = nlohmann::json;

namespace clipnest::ops {

    const string CatNone = "";
    const string CatEncoding = "encode";
    const string CatBasics = "basics";
    const string CatNumbers = "numbers";
    const string CatHash = "hash";
    const string CatWeb = "web";
    const string CatExtract = "extract";

    void general::init()
    {
        GEN_OP(CatNone, "len", "length", to_string(input.value.length()));

        add(CatNone, "wc", "words",
            [](const operation_input& input) {
            size_t wc = str::word_count(input.value);
            return wc > 0 ? to_string(wc) : string("");
        });

        GEN_OP(CatEncoding, "to_base64", "to base64",
            str::base64_encode((const unsigned char*)input.value.c_str(), input.value.length()));

        GEN_OP(CatEncoding, "from_base64", "from base64", str::base64_decode(input.value));

        add(CatBasics, "upper", "to upper", [](const operation_input& input) {
            string r = input.value;
            str::upper(r);
            return r;
        });

        add(CatBasics, "lower", "to lower", [](const operation_input& input) {
            string r = input.value;
            str::lower(r);
            return r;
        });

        add(CatBasics, "trim", "trim", [](const operation_input& input) {
            string r = input.value;
            str::trim(r);
            return r;
        });

        GEN_OP(CatBasics, "plain", "remove non-ASCII chars", str::remove_non_ascii(input.value));

        add(CatNumbers, "filesize", "size", [](const operation_input& input) {
            long l = stol(input.value);
            return str::to_human_readable_size(static_cast<unsigned long>(l));
        });

        GEN_OP(CatHash, "md5", "MD5", win32::cng::md5(input.value));
        GEN_OP(CatHash, "sha256", "SHA-256", win32::cng::sha256(input.value));
        GEN_OP(CatHash, "sha512", "SHA-512", win32::cng::sha512(input.value));

        GEN_OP(CatWeb, "html", "strip HTML", str::strip_html(input.value));

        add(CatWeb, "json", "format JSON", [](const operation_input& input) {
            auto j = json::parse(input.value);
            return j.dump(4);
        });

        add(CatExtract, "x-email", "email", [](const operation_input& input) {
            return str::rgx_extract(input.value, "\\w+([-+.']\\w+)*@\\w+([-.]\\w+)*.\\w+([-.]\\w+)*");
        });

        add(CatExtract, "x-ipv4", "IP v4", [](const operation_input& input) {
            return str::rgx_extract(input.value, "(\\b25[0-5]|\\b2[0-4][0-9]|\\b[01]?[0-9][0-9]?)(\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)){3}");
        });

        add(CatExtract, "x-ipv6", "IP v6", [](const operation_input& input) {
            return str::rgx_extract(input.value, "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))");
        });

        add(CatBasics, "dec2hex", "dec -> hex", [](const operation_input& input) {
            long l = stol(input.value);
            return fmt::format("{0:x}", l);
        });

        add(CatBasics, "hex2dec", "hex -> dec", [](const operation_input& input) {
            long l = stol(input.value, nullptr, 16);
            return fmt::format("{0}", l);
        });

        add(CatBasics, "dec2bin", "dec -> bin", [](const operation_input& input) {
            long l = stol(input.value);
            return fmt::format("{0:b}", l);
        });

        add(CatBasics, "bin2dec", "bin -> dec", [](const operation_input& input) {
            long l = stol(input.value, nullptr, 2);
            return fmt::format("{0}", l);
        });

        GEN_OP(CatBasics, "dedupe", "deduplicate lines", str::deduplicate_lines(input.value));
    }

    void general::add(const std::string& cat, const std::string& id, const std::string& name,
        std::function<std::string(const operation_input&)> calc_fn)
    {
        operation::add(cat, id, name, [](const operation_input&) { return true; }, calc_fn);
    }
}