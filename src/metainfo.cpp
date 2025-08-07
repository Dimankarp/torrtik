#include "metainfo.h"
#include "bencoding/benc.h"
#include "bencoding/benctypes.h"
#include <cstddef>
#include <filesystem>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

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

const std::unordered_map<std::string, TrackerProtocol> protocol_by_name = {
    { "http", TrackerProtocol::HTTP },
    { "udp", TrackerProtocol::UDP },
};

TrackerMeta extract_tracker_meta(const std::string& announce) {
    const std::string SCHEME_DELIM_STR = "://";
    const std::string QUERY_DELIM_STR = "/";
    auto scheme_delim = announce.find(SCHEME_DELIM_STR);
    if(scheme_delim == std::string::npos)
        throw std::runtime_error{ "Failed to parse tracker announce url - no "
                                  "scheme delimeter" };
    std::string scheme = announce.substr(0, scheme_delim);
    auto hostname_start = scheme_delim + SCHEME_DELIM_STR.size();
    auto query_delim = announce.find(QUERY_DELIM_STR, hostname_start);

    std::string hostname;
    std::optional<std::string> query;

    if(query_delim == std::string::npos) {
        hostname = announce.substr(hostname_start);
    } else {
        query = announce.substr(query_delim);
        hostname = announce.substr(hostname_start, query_delim - hostname_start);
    }

    auto protocol_it = protocol_by_name.find(scheme);
    if(protocol_it == protocol_by_name.end()) {
        throw std::runtime_error{ "Failed to parse scheme " + scheme + " from announce url" };
    }

    return { .protocol = protocol_it->second, .hostname = hostname, .query = query };
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

    auto tracker_meta = extract_tracker_meta(announce);

    return { .announce = tracker_meta,
             .piece_length = piece_length,
             .pieces = piece_hashes,
             .info_hash = info_hash,
             .files = files };
}

} // namespace trrt