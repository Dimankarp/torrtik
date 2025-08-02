
#include <array>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
namespace trrt {

struct FileMeta {
    std::filesystem::path path;
    std::size_t length;
};


struct Metainfo {
    private:
    const static int HASH_SIZE = 20;

    public:
    std::string announce;
    std::size_t piece_length;
    std::vector<std::array<unsigned char, HASH_SIZE>> pieces;
    std::unique_ptr<std::array<unsigned char, HASH_SIZE>> info_hash;
    std::vector<FileMeta> files;
};

} // namespace trrt