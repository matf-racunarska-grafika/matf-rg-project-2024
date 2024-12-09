//
// Created by spaske on 12/2/24.
//

#ifndef SKYBOX_HPP
#define SKYBOX_HPP
#include <cstdint>
#include <filesystem>
#include <utility>

namespace engine::resources {
    class Skybox {
        friend class ResourcesController;

    public:
        static Skybox create();

        uint32_t vao() const {
            return m_vao;
        }

        uint32_t texture() const {
            return m_texture_id;
        }

        void destroy();

    private:
        Skybox() = default;

        uint32_t m_vao;
        uint32_t m_texture_id;
        std::filesystem::path m_path;
        std::string m_name;

        Skybox(uint32_t vao, uint32_t texture_id, std::filesystem::path path, std::string name)
        : m_vao(vao)
      , m_texture_id(texture_id)
      , m_path(std::move(path))
      , m_name(std::move(name)) {
        }
    };
}

#endif //SKYBOX_HPP
