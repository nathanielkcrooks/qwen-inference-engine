#include <cstddef>
#include <memory>
#include <string>
#include <vector>

struct TensorView {
    std::string dtype;
    std::vector<size_t> shape;
    const void* data = nullptr;
    std::size_t nbytes = 0;
};

class SafetensorsLoader {
    public:
        explicit SafetensorsLoader(const std::string& file_path);
        ~SafetensorsLoader();

        SafetensorsLoader(SafetensorsLoader&&) noexcept;
        SafetensorsLoader& operator=(SafetensorsLoader&&) noexcept;
        SafetensorsLoader(const SafetensorsLoader&) = delete;
        SafetensorsLoader& operator=(const SafetensorsLoader&) = delete;
        TensorView get_tensor(const std::string& name) const;
    private:
        struct Impl;
        std::unique_ptr<Impl> pimpl_;
};