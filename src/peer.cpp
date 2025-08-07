
#include "peer.h"
#include "trrtconfig.h"
#include <random>
#include <string>

namespace trrt::peer {

trrt::sha1_hash_t generate_peer_id() {
    const int MAGIC_PRIME_1 = 7229;
    const int MAGIC_PRIME_2 = 1759;
    const int PEER_ID_BYTES = 20;
    const std::string client_prefix = "TK";
    const std::string version_str = std::to_string(PROJECT_VERSION_MAJOR) +
    std::to_string(PROJECT_VERSION_MINOR) + std::to_string(PROJECT_VERSION_PATCH);

    std::random_device rd{};
    std::linear_congruential_engine<uint16_t, MAGIC_PRIME_1, MAGIC_PRIME_2, 0> lce{
        static_cast<uint16_t>(rd())
    };
    std::string str{};

    str.reserve(PEER_ID_BYTES);
    str += "-" + client_prefix + version_str + "-";
    while(str.length() != PEER_ID_BYTES) {
        str += std::to_string(lce() % 10);
    }
    trrt::sha1_hash_t result{};
    str.copy(reinterpret_cast<char*>(result.data()), result.size());
    return result;
}
} // namespace trrt::peer