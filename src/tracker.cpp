#include "tracker.h"
#include "bencoding/benctypes.h"
#include "serial/read.h"
#include "utils/urlencode.h"
#include <boost/asio/ip/address_v4.hpp>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
namespace trrt::http {

namespace {

const std::unordered_map<TrackerEvent, std::string> event_to_name = {
    { TrackerEvent::STARTED, "started" },
    { TrackerEvent::STOPPED, "stopped" },
    { TrackerEvent::COMPLETED, "completed" },
};
}

std::string tracker_request_to_url_params(const TrackerRequest& req) {
    std::ostringstream str{};
    str << "info_hash=" << urlencode(req.info_hash) << "&";
    str << "peer_id=" << urlencode(req.peer_id) << "&";
    str << "port=" << req.port << "&";
    str << "uploaded=" << req.uploaded << "&";
    str << "downloaded=" << req.downloaded << "&";
    str << "left=" << req.left << "&";
    str << "compact=" << req.compact << "&";
    str << "event=" << event_to_name.at(req.event);
    return str.str();
}

namespace {
std::optional<uint64_t>
extract_optional_uint64(const benc::BencDict& dict, const std::string& key) {
    auto it = dict.find(key);
    std::optional<std::uint64_t> item =
    it != dict.end() ? std::optional{ it->second->get_int().val } : std::nullopt;
    return item;
}


std::optional<std::string>
extract_optional_str(benc::BencDict& dict, const std::string& key) {
    auto it = dict.find(key);
    std::optional<std::string> item = it != dict.end() ?
    std::optional{ std::move(it->second->get_str().val) } :
    std::nullopt;
    return item;
}

} // namespace


std::expected<TrackerResponse, TrackerFailResponse>
extract_tracker_response(benc::BencDict dict) {
    auto failure_msg = dict.find("failure message");
    if(failure_msg != dict.end()) {
        return std::unexpected(
        TrackerFailResponse{ failure_msg->second->get_str().val });
    }

    auto warning = extract_optional_str(dict, "warning message");
    auto tracker_id = extract_optional_str(dict, "tracker id");
    auto min_interval = extract_optional_uint64(dict, "min interval");

    uint64_t interval = dict.at("interval")->get_int();
    auto complete = extract_optional_uint64(dict, "complete");
    auto incomplete = extract_optional_uint64(dict, "incomplete");

    std::string peer_str = std::move(dict.at("peers")->get_str().val);

    const size_t IP_BYTES = 4 * sizeof(char);
    const size_t PORT_BYTES = 2 * sizeof(char);

    std::vector<boost::asio::ip::tcp::endpoint> peers;

    char* ptr = peer_str.data();
    for(int i = 0; i < peer_str.size() / (IP_BYTES + PORT_BYTES); i++) {

        uint32_t ip = trrt::serial::read_uint32(ptr);
        uint16_t port = trrt::serial::read_uint16(ptr);
        peers.emplace_back(boost::asio::ip::address_v4{ ip }, port);
    }
    return TrackerResponse{ .warning = warning,
                            .interval = interval,
                            .min_interval = min_interval,
                            .tracker_id = tracker_id,
                            .complete = complete,
                            .incomplete = incomplete,
                            .peers = peers };
};


} // namespace trrt::http