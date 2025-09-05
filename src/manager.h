#pragma once
#include "message.h"
#include "metainfo.h"
#include "utils/hashing.h"
#include <cassert>
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

    [[nodiscard]]
    constexpr sha1_hash_t info_hash() noexcept {
        return _info_hash;
    }

    [[nodiscard]]
    constexpr sha1_hash_t peer_id() noexcept {
        return _peer_id;
    }

    [[nodiscard]]
    std::size_t reserve_piece(const std::vector<bool>& bitfield) {
        // assert(bitfield.size() == pieces_status.size());
        for(std::size_t i = 0; i < pieces_status.size(); i++) {
            if(pieces_status[i] == PieceStatus::NOT_STARTED && bitfield[i]) {
                pieces_status[i] = PieceStatus::DOWNLOADING;
                return i;
            }
        }
        assert(false);
    }
};


} // namespace trrt