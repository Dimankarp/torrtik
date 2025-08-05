#include "hashing.h"
namespace trrt {

/// Calculates 20-byte sha1 of @ref str
sha1_hash_t sha1_hash(std::span<char> str) {
    sha1_hash_t hash{};
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha1(), nullptr);
    EVP_DigestUpdate(ctx, str.data(), str.size());
    EVP_DigestFinal_ex(ctx, hash.data(), nullptr);
    EVP_MD_CTX_free(ctx);
    return hash;
}
} // namespace trrt
