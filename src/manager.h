#pragma once
#include "message.h"
#include "metainfo.h"
#include "utils/hashing.h"
namespace trrt {


class TorrentManager {


    private:
    enum class PieceStatus : char {
        NOT_STARTED,
        DOWNLOADING,
        FAILED,
        FINISHED
    };


    private:
    TrackerMeta announce;
    std::size_t piece_length;
    std::vector<sha1_hash_t> pieces_hash;
    std::vector<FileMeta> files;
    sha1_hash_t _info_hash;
    sha1_hash_t _peer_id;

    std::vector<PieceStatus> pieces_status;

    public:
    TorrentManager(Metainfo meta, sha1_hash_t peer_id)
    : announce{ std::move(meta.announce) }, piece_length{ meta.piece_length },
      pieces_hash{ std::move(meta.pieces) }, files{ std::move(meta.files) },
      _info_hash{ meta.info_hash }, _peer_id{ peer_id },
      pieces_status(pieces_hash.size(), TorrentManager::PieceStatus::NOT_STARTED) {}


    sha1_hash_t info_hash() { return _info_hash; }

    sha1_hash_t peer_id() { return _peer_id; }
};


} // namespace trrt