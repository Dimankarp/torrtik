
#include "bencoding/benctypes.h"
#include "utils/hashing.h"
#include <array>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace trrt {

struct FileMeta {
    std::filesystem::path path;
    std::size_t length;
};


struct Metainfo {
    std::string announce;
    std::size_t piece_length;
    std::vector<sha1_hash_t> pieces;
    sha1_hash_t info_hash;
    std::vector<FileMeta> files;
};

/// Extracts Metainfo from bencoded metainfo (i.e. *.torrent file) &
/// calculates info_hash
/**

    @throws std::runtime_error on failure

*/
Metainfo extract_metainfo(benc::BencDict benc);

} // namespace trrt