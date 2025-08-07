#include "tracker.h"
#include "utils/urlencode.h"
#include <sstream>
#include <string>
#include <string_view>
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


} // namespace trrt::http