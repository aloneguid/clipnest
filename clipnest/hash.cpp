#include "hash.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <sstream>

using namespace std;

string to_hex(unsigned char s) {
    stringstream ss;
    ss << hex << (int)s;
    return ss.str();
}

string md5(const string& content) {
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_md5();
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int  md_len;
    string        output;

    EVP_DigestInit_ex2(context, md, NULL);
    EVP_DigestUpdate(context, content.c_str(), content.length());
    EVP_DigestFinal_ex(context, md_value, &md_len);
    EVP_MD_CTX_free(context);

    output.resize(md_len * 2);
    for (unsigned int i = 0; i < md_len; ++i)
        std::sprintf(&output[i * 2], "%02x", md_value[i]);
    return output;
}

string sha256(const string& line) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, line.c_str(), line.length());
    SHA256_Final(hash, &sha256);

    string output = "";
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        output += to_hex(hash[i]);
    }
    return output;
}