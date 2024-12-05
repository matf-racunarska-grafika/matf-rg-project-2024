
//

#ifndef MATF_RG_PROJECT_MODEL_HPP
#define MATF_RG_PROJECT_MODEL_HPP
#include <engine/util/Utils.hpp>
#include <engine/resources/Mesh.hpp>
#include <algorithm>
#include <utility>

namespace rg {
    class Model {
        friend class ResourcesController;

    public:
        void draw(Shader *shader);

        std::string_view class_name() const {
            return "Model";
        }

        static std::string_view type_name() {
            return "Model";
        }

        void destroy();

        const std::vector<Mesh> &meshes() const {
            return m_meshes;
        }

        const std::filesystem::path &path() const {
            return m_path;
        }

        const std::string &name() const {
            return m_name;
        }

    private:
        std::vector<Mesh> m_meshes;
        std::filesystem::path m_path;
        std::string m_name;

        Model() = default;

        Model(std::vector<Mesh> meshes, std::filesystem::path path,
              std::string name) : m_meshes(std::move(meshes))
                              , m_path(std::move(path))
                              , m_name(std::move(name)) {
        }
    };
} // namespace rg

#endif//MATF_RG_PROJECT_MODEL_HPP
