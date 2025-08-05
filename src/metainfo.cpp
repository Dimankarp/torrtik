#include "metainfo.h"
#include "bencoding/benc.h"
#include "bencoding/benctypes.h"
#include <cstddef>
#include <filesystem>
#include <span>
#include <string_view>

namespace trrt {

namespace {

FileMeta extract_single_file(const benc::BencDict& info) {
    return { .path = info.at("name")->get_str().val,
             // TODO(dimankarp): Possible value checks (i.e. length >=0);
             .length = static_cast<size_t>(info.at("length")->get_int()) };
}

std::vector<FileMeta> extract_multiple_files(const benc::BencDict& info) {
    std::vector<FileMeta> results{};
    for(const auto& item_val : info.at("files")->get_list().val) {
        const benc::BencDict& item = item_val.get_dict();
        std::filesystem::path p{};
        for(const auto& path_part : item.at("path")->get_list().val) {
            p.append(path_part.get_str().val);
        }
        results.emplace_back(p, item.at("length")->get_int().val);
    }
    return results;
}

} // namespace

Metainfo extract_metainfo(benc::BencDict benc) {

    std::string announce = std::move(benc.at("announce")->get_str().val);

    const benc::BencDict& info = benc.at("info")->get_dict();
    std::string info_encoded = benc::bencode(info);
    sha1_hash_t info_hash =
    sha1_hash(std::span<char>(info_encoded.begin(), info_encoded.size()));

    std::size_t piece_length = info.at("piece length")->get_int();

    std::string_view pieces_view = info.at("pieces")->get_str().val;
    std::size_t pieces_count = pieces_view.length() / SHA1_HASH_SZ;
    std::vector<sha1_hash_t> piece_hashes(pieces_count);

    for(int i = 0; i < static_cast<int>(pieces_count); i++) {
        piece_hashes.emplace_back();
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        pieces_view.copy(reinterpret_cast<char*>(piece_hashes.back().data()),
                         SHA1_HASH_SZ, static_cast<std::size_t>(i) * SHA1_HASH_SZ);
    }

    std::vector<FileMeta> files;
    if(info.find("length") != info.end()) {
        files.emplace_back(extract_single_file(info));
    } else {
        files = extract_multiple_files(info);
    }
    return { .announce = announce,
             .piece_length = piece_length,
             .pieces = piece_hashes,
             .info_hash = info_hash,
             .files = files };
}

} // namespace trrt