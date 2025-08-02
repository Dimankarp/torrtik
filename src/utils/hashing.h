#include <array>
#include <openssl/evp.h>
#include <span>
namespace trrt {

const int SHA1_HASH_SZ = 20;

std::array<unsigned char, SHA1_HASH_SZ> sha1_hash(std::span<char> str);
} // namespace trrt
