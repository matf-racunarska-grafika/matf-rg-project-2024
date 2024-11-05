//
// Created by spaske00 on 13.5.24..
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <engine/platform/Platform.hpp>
#include <engine/util/Utils.hpp>
#include <engine/render/ShaderController.hpp>
#include <engine/render/Shader.hpp>
#include <engine/controller/ControllerManager.hpp>
#include <spdlog/spdlog.h>
#include <utility>
#include <engine/render/Model.hpp>
#include <engine/render/Texture.hpp>
#include <engine/render/Mesh.hpp>


#include <assimp/Importer.hpp>
namespace rg {

    /*
     * Platform IMPL
     */
    static std::array<int, KEY_COUNT> g_engine_to_glfw_key;
    static MousePosition g_mouse_position;

    static void glfw_mouse_callback(GLFWwindow *window, double x, double y);

    void initialize_key_maps();

    struct WindowImpl {
        GLFWwindow *handle;
        int width;
        int height;
        std::string title;
        WindowImpl(GLFWwindow *handle, int width, int height, std::string title)
            : handle(handle), width(width), height(height), title(std::move(title)) {
        }
    };

    void PlatformController::initialize() {
        bool glfw_initialized = glfwInit();
        RG_GUARANTEE(glfw_initialized, "GLFW platform failed to initialize_controllers.");
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        Configuration::json &config = Configuration::config();
        int window_width = config["window"]["width"];
        int window_height = config["window"]["height"];
        std::string window_title = config["window"]["title"];
        GLFWwindow *handle = glfwCreateWindow(window_width, window_height, window_title.c_str(), nullptr, nullptr);
        RG_GUARANTEE(handle, "GLFW3 platform failed to create a Window.");
        m_window = new WindowImpl(handle, window_width, window_height, window_title);
        RG_GUARANTEE(m_window != nullptr, "Must instantiate m_window_impl first");

        glfwMakeContextCurrent(m_window->handle);
        glfwSetCursorPosCallback(m_window->handle, glfw_mouse_callback);
        RG_GUARANTEE(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress), "GLAD failed to init!");

        initialize_key_maps();
        m_keys.resize(KEY_COUNT);
        for (int key = 0; key < m_keys.size(); ++key) {
            m_keys[key].m_key = static_cast<KeyId>(key);
        }
    }

    void PlatformController::terminate() {
        glfwDestroyWindow(m_window->handle);
        delete m_window;
        m_window = nullptr;
        glfwTerminate();
    }

    bool PlatformController::loop() {
        return !glfwWindowShouldClose(m_window->handle);
    }

    void PlatformController::poll_events() {
        glfwPollEvents();
    }

    void PlatformController::update() {
        m_mouse = g_mouse_position;
        for (int i = 0; i < KEY_COUNT; ++i) {
            update_key(key(static_cast<KeyId>(i)));
        }

        glfwSwapBuffers(m_window->handle);
    }

    void PlatformController::update_key(Key &key_data) {
        int engine_key_code = key_data.key();
        int glfw_key_code = g_engine_to_glfw_key.at(engine_key_code);
        auto window = m_window->handle;
        int action = glfwGetKey(window, glfw_key_code);
        switch (key_data.state()) {
        case rg::Key::State::Released: {
            if (action == GLFW_PRESS) {
                key_data.m_state = Key::State::JustPressed;
            }
            break;
        }
        case rg::Key::State::JustReleased: {
            if (action == GLFW_PRESS) {
                key_data.m_state = rg::Key::State::JustPressed;
            } else if (action == GLFW_RELEASE) {
                key_data.m_state = Key::State::Released;
            }
            break;
        }
        case rg::Key::State::JustPressed: {
            if (action == GLFW_RELEASE) {
                key_data.m_state = Key::State::JustReleased;
            } else if (action == GLFW_PRESS) {
                key_data.m_state = Key::State::Pressed;
            }
            break;
        }
        case rg::Key::State::Pressed: {
            if (action == GLFW_RELEASE) {
                key_data.m_state = rg::Key::State::JustReleased;
            }
            break;
        }
        }
    }


    std::string_view Key::to_string() const {
        switch (m_state) {
        case Key::State::Released: return "Released";
        case Key::State::JustPressed: return "JustPressed";
        case Key::State::Pressed: return "Pressed";
        case Key::State::JustReleased: return "JustReleased";
        default: return "UNIMPLEMENTED";
        }
    }

    Key &PlatformController::key(KeyId key) {
        RG_GUARANTEE(key >= 0 && key < m_keys.size(), "KeyId out of bounds!");
        return m_keys[key];
    }

    const Key &PlatformController::key(KeyId key) const {
        RG_GUARANTEE(key >= 0 && key < m_keys.size(), "KeyId out of bounds!");
        return m_keys[key];
    }


    std::unique_ptr<PlatformController> PlatformController::create() {
        return std::make_unique<PlatformController>();
    }

    const MousePosition &PlatformController::mouse() const {
        return m_mouse;
    }

    std::string_view PlatformController::name() const {
        return "PlatformGLFW3Controller";
    }

    int PlatformController::window_width() const {
        return m_window->width;
    }

    int PlatformController::window_height() const {
        return m_window->height;
    }

    const std::string &PlatformController::window_title() const {
        return m_window->title;
    }

    const std::string_view PlatformController::shader_language() const {
        return "glsl";
    }

    void initialize_key_maps() {
#include "glfw_key_mapping.h"
    }


    static void glfw_mouse_callback(GLFWwindow *window, double x, double y) {
        double last_x = g_mouse_position.x;
        double last_y = g_mouse_position.y;
        g_mouse_position.dx = x - last_x;
        g_mouse_position.dy = last_y - y;// because in glfw the top left corner is the (0,0)
        g_mouse_position.x = x;
        g_mouse_position.y = y;
    }


    /*
     * SHADER IMPL
     */
    int to_opengl_type(ShaderType type);
    //    std::string_view to_string(ShaderType type);

    struct ShaderParsingResult {
        std::string vertex_shader;
        std::string fragment_shader;
        std::string geometry_shader;
    };

    void ShaderProgram::use() const {
        glUseProgram(m_shaderId);
    }

    void ShaderProgram::destroy() const {
        glDeleteProgram(m_shaderId);
    }

    ShaderCompilationResult ShaderCompiler::compile_from_source(std::string shader_name, std::string shader_source) {
        try {
            spdlog::info("Compiling: {}", shader_name);
            ShaderCompiler compiler(std::move(shader_name), std::move(shader_source));
            ShaderParsingResult parsing_result = compiler.parse_source();
            ShaderProgram shader_program = compiler.compile(parsing_result);
            return shader_program;
        } catch (const ShaderCompilationError &e) { return e; }
    }

    bool opengl_compilation_failed(int shader_id) {
        int success;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        return !success;
    };

    std::string opengl_get_compilation_error_message(int shader_id) {
        char infoLog[512];
        glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
        return infoLog;
    }

    ShaderProgram ShaderCompiler::compile(const ShaderParsingResult &shader_sources) {
        int shader_program_id = glCreateProgram();
        int vertex_shader_id = 0;
        int fragment_shader_id = 0;
        int geometry_shader_id = 0;
        defer {
            glDeleteShader(vertex_shader_id);
            glDeleteShader(fragment_shader_id);
            glDeleteShader(geometry_shader_id);
        };

        vertex_shader_id = compile(shader_sources.vertex_shader, ShaderType::Vertex);
        glAttachShader(shader_program_id, vertex_shader_id);
        fragment_shader_id = compile(shader_sources.fragment_shader, ShaderType::Fragment);
        glAttachShader(shader_program_id, fragment_shader_id);

        if (!shader_sources.geometry_shader.empty()) {
            geometry_shader_id = compile(shader_sources.geometry_shader, ShaderType::Geometry);
            glAttachShader(shader_program_id, geometry_shader_id);
        }
        glLinkProgram(shader_program_id);
        ShaderProgram result(shader_program_id);
        return result;
    }

    int ShaderCompiler::compile(const std::string &shader_source, ShaderType type) {
        int shader_id = glCreateShader(to_opengl_type(type));
        const char *shader_source_cstr = shader_source.c_str();
        glShaderSource(shader_id, 1, &shader_source_cstr, nullptr);
        glCompileShader(shader_id);
        if (opengl_compilation_failed(shader_id)) {
            throw ShaderCompilationError(std::format("Shader compilation {} failed:\n{}", m_shader_name,
                                                     opengl_get_compilation_error_message(shader_id)));
        }
        return shader_id;
    }

    static std::string *get_current_shader(ShaderParsingResult &result, const std::string &line);

    ShaderParsingResult ShaderCompiler::parse_source() {
        ShaderParsingResult parsing_result;
        std::istringstream ss(m_sources);
        std::string line;
        std::string *current_shader;
        while (std::getline(ss, line)) {
            if (line.starts_with("#shader")) {
                current_shader = get_current_shader(parsing_result, line);
                continue;
            }
            current_shader->append(line);
            current_shader->push_back('\n');
        }
        if (parsing_result.vertex_shader.empty() || parsing_result.fragment_shader.empty()) {
            throw ShaderCompilationError(std::format(
                    "Error compiling: {}. Source for vertex and fragment shader must be defined.", m_shader_name));
        }
        return parsing_result;
    }

    ShaderCompilationResult ShaderCompiler::compile_from_file(std::string shader_name,
                                                              const std::filesystem::path &path) {
        if (!exists(path)) {
            return FileNotFoundError(
                    path, std::format("Shader source file {} for shader {} not found.", path.string(), shader_name));
        }
        return compile_from_source(std::move(shader_name), rg::read_file(path));
    }


    std::string *get_current_shader(ShaderParsingResult &result, const std::string &line) {
        if (line.ends_with(to_string(ShaderType::Vertex))) {
            return &result.vertex_shader;
        }
        if (line.ends_with(to_string(ShaderType::Fragment))) {
            return &result.fragment_shader;
        }
        if (line.ends_with(to_string(ShaderType::Geometry))) {
            return &result.geometry_shader;
        }
        RG_SHOULD_NOT_REACH_HERE("Unknown type of shader prefix: {}. Did you mean: #shader {}|{}|{}", line,
                                 to_string(ShaderType::Vertex), to_string(ShaderType::Fragment),
                                 to_string(ShaderType::Geometry));
    }

    int to_opengl_type(ShaderType type) {
        switch (type) {
        case ShaderType::Vertex: return GL_VERTEX_SHADER;
        case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
        case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
        default: RG_SHOULD_NOT_REACH_HERE("Unhandled ShaderType");
        }
    }


    void Model::draw(ShaderProgram *shader) {
        rg::once([] { spdlog::info("Model::draw"); });
        for (auto &mesh: m_meshes) {
            mesh.draw(shader);
        }
    }

    void Model::destroy() {
        rg::once([] { spdlog::info("Model::destroy"); });
        for (auto &mesh: m_meshes) {
            mesh.destroy();
        }
    }

    void Model::initialize() {
    }


    Mesh Mesh::create(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
                      const std::unordered_set<Texture *> &textures) {
        static_assert(std::is_trivial_v<Vertex>);
        uint32_t VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tex_cords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));

        glBindVertexArray(0);

        return Mesh(VAO, std::vector<Texture *>(range(textures)));
    }

    void Mesh::draw(ShaderProgram *shader) {
        // TODO(mspasic): draw meshes
    }

    Texture Texture::create_from_file(std::filesystem::path path) {
        uint32_t texture_id;
        glGenTextures(1, &texture_id);

        int32_t width, height, nr_components;
        uint8_t *data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);
        defer {
            stbi_image_free(data);
        };
        if (data) {
            GLenum format;
            if (nr_components == 1)
                format = GL_RED;
            else if (nr_components == 3)
                format = GL_RGB;
            else if (nr_components == 4)
                format = GL_RGBA;
            else
                throw AssetLoadingError("Unknown number of channels");

            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            throw AssetLoadingError(std::format("Failed to load texture {}", path.string()));
        }
        return Texture(texture_id);
    }

    void Texture::initialize() {
        RG_UNIMPLEMENTED("texture initialize");
    }

    void Texture::draw(ShaderProgram *shader) {
        // TODO(mspasic): do we need this?
        RG_UNIMPLEMENTED("texture draw");
    }

    void Texture::destroy() {
        glDeleteTextures(1, &m_id);
    }


    struct MeshImpl {
        uint32_t vao_id;
    };

    Mesh platform_initialize_mesh();
}// namespace rg
