#pragma once

#include "bencoding/benctypes.h"
#include "utils/hashing.h"
#include <array>
#include <boost/asio/ip/tcp.hpp>
#include <expected>
#include <optional>
#include <string>
#include <unordered_map>
namespace trrt::http {

enum class TrackerEvent : char { STARTED = 0, STOPPED, COMPLETED };


struct TrackerRequest {

    sha1_hash_t info_hash;
    sha1_hash_t peer_id;
    uint16_t port;
    std::size_t uploaded = 0;
    std::size_t downloaded = 0;
    std::size_t left;
    // Nailing to compact=1 for now
    const bool compact = true;
    TrackerEvent event = TrackerEvent::STARTED;
};

/// Creates a URL-encoded param list from TrackerRequest
/**
    Produces a valid string of url-encoded params in the form of
   key1=value1&key2=.... from TrackerRequest

*/
std::string tracker_request_to_url_params(const TrackerRequest& req);


struct TrackerFailResponse {
    std::string reason;
};

struct TrackerResponse {

    std::optional<std::string> warning;
    uint64_t interval;
    std::optional<uint64_t> min_interval;
    std::optional<std::string> tracker_id;
    std::optional<uint64_t> complete;
    std::optional<uint64_t> incomplete;
    std::vector<boost::asio::ip::tcp::endpoint> peers;
};


std::expected<TrackerResponse, TrackerFailResponse>
extract_tracker_response(benc::BencDict dict);


} // namespace trrt::http