#include "safetensors.hpp"

#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct SafetensorsLoader::Impl {
    int fd = -1;
    void* mapped_ptr = nullptr;
    size_t file_size = 0;
    const uint8_t* data_base = nullptr;
    json metadata;

    ~Impl() {
        if (mapped_ptr != nullptr && mapped_ptr != MAP_FAILED) {
            munmap(mapped_ptr, file_size);
        }
        if (fd != -1) {
            close(fd);
        }
    }
};

SafetensorsLoader::SafetensorsLoader(const std::string& file_path)
    : pimpl_(std::make_unique<Impl>()){
        pimpl_->fd = open(file_path.c_str(), O_RDONLY);
        if (pimpl_->fd == -1) {
            throw std::runtime_error("Failed to open file: " + file_path);
        }
        
        struct stat st;
        if (fstat(pimpl_->fd, &st) == -1) {
            throw std::runtime_error("Failed to find stat file");
        }
        pimpl_->file_size = st.st_size;

        pimpl_->mapped_ptr = mmap(nullptr, pimpl_->file_size, PROT_READ, MAP_PRIVATE, pimpl_->fd, 0);
        if (pimpl_>mapped_ptr == MAP_FAILED) {
            throw std::runtime_error("mmap failed");
        }

        madvise(pimpl_->mapped_ptr, pimpl_->file_size, MADV_SEQUENTIAL);

        const auto* base = static_cast<const uint8_t*>(pimpl_->mapped_ptr);
        uint64_t header_size = 0;
        std::memcpy(&header_size, base, sizeof(uint64_t));
        
        std::string json_str(reinterpret_cast<const char*>(base + 8), header_size);
        pimpl_->metadata = json::parse(json_str)
        pimpl_->data_base = base + 8 + header_size
    }