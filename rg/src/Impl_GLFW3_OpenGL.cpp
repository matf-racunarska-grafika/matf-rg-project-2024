//
// Created by spaske00 on 13.5.24..
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <engine/platform/Platform.hpp>
#include <engine/util/Utils.hpp>
#include <engine/render/Shader.hpp>
#include <engine/controller/ControllerManager.hpp>
#include <engine/render/Model.hpp>
#include <engine/render/Texture.hpp>
#include <engine/render/Mesh.hpp>


#include <spdlog/spdlog.h>
#include <utility>

namespace rg {

    /*
     * Platform IMPL
     */
    static std::array<int, KEY_COUNT> g_engine_to_glfw_key;
    static std::array<KeyId, GLFW_KEY_LAST + 1> g_glfw_key_to_engine;
    static MousePosition g_mouse_position;

    static void glfw_mouse_callback(GLFWwindow *window, double x, double y);
    static void glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
    static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height);
    static int glfw_platform_action(GLFWwindow *window, int glfw_key_code);

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
        if (glfwPlatformSupported(GLFW_PLATFORM_X11)) {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
        } else if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND)) {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
        } else if (glfwPlatformSupported(GLFW_PLATFORM_WIN32)) {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WIN32);
        }
        bool glfw_initialized = glfwInit();
        RG_GUARANTEE(glfw_initialized, "GLFW platform failed to initialize_controllers.");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
        glfwSetScrollCallback(m_window->handle, glfw_scroll_callback);
        glfwSetKeyCallback(m_window->handle, glfw_key_callback);
        glfwSetFramebufferSizeCallback(m_window->handle, glfw_framebuffer_size_callback);
        const int opengl_initialized = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        RG_GUARANTEE(opengl_initialized, "GLAD failed to init!");

        int major, minor, revision;
        glfwGetVersion(&major, &minor, &revision);
        spdlog::info("Platform[GLFW {}.{}.{}]", major, minor, revision);
        initialize_key_maps();
        m_keys.resize(KEY_COUNT);
        for (int key = 0; key < m_keys.size(); ++key) {
            m_keys[key].m_key = static_cast<KeyId>(key);
        }
        register_platform_event_observer(std::make_unique<PlatformEventObserver>());
    }

    void PlatformController::terminate() {
        m_platform_event_observer.reset(nullptr);
        glfwDestroyWindow(m_window->handle);
        delete m_window;
        m_window = nullptr;
        glfwTerminate();
    }

    bool PlatformController::loop() {
        m_frame_time.previous = m_frame_time.current;
        m_frame_time.current = glfwGetTime();
        m_frame_time.dt = m_frame_time.current - m_frame_time.previous;

        return !glfwWindowShouldClose(m_window->handle);
    }

    void PlatformController::poll_events() {
        m_mouse = g_mouse_position;
        for (int i = 0; i < KEY_COUNT; ++i) {
            update_key(key(static_cast<KeyId>(i)));
        }
        glfwPollEvents();
    }

    void PlatformController::update() {
    }

    void PlatformController::draw() {
    }

    int glfw_platform_action(GLFWwindow *window, int glfw_key_code) {
        if (glfw_key_code >= GLFW_MOUSE_BUTTON_1 && glfw_key_code <= GLFW_MOUSE_BUTTON_LAST) {
            return glfwGetMouseButton(window, glfw_key_code);
        }
        return glfwGetKey(window, glfw_key_code);
    }

    void PlatformController::update_key(Key &key_data) const {
        int engine_key_code = key_data.key();
        int glfw_key_code = g_engine_to_glfw_key.at(engine_key_code);
        auto window = m_window->handle;
        int action = glfw_platform_action(window, glfw_key_code);
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

    void PlatformEventObserver::on_mouse(MousePosition position) {
    }
    void PlatformEventObserver::on_keyboard(Key key) {
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

    void PlatformController::register_platform_event_observer(std::unique_ptr<PlatformEventObserver> observer) {
        m_platform_event_observer = std::move(observer);
    }

    void PlatformController::_platform_on_mouse(double x, double y) const {
        double last_x = g_mouse_position.x;
        double last_y = g_mouse_position.y;
        g_mouse_position.dx = x - last_x;
        g_mouse_position.dy = last_y - y;// because in glfw the top left corner is the (0,0)
        g_mouse_position.x = x;
        g_mouse_position.y = y;
        m_platform_event_observer->on_mouse(g_mouse_position);
    }

    void PlatformController::_platform_on_keyboard(int key_code, int action) {
        const Key result = key(g_glfw_key_to_engine[key_code]);
        m_platform_event_observer->on_keyboard(result);
    }

    void PlatformController::_platform_on_scroll(double x, double y) const {
        g_mouse_position.scroll = y;
        m_platform_event_observer->on_mouse(g_mouse_position);
    }

    void PlatformController::_platform_on_framebuffer_resize(int width, int height) const {
        m_window->width = width;
        m_window->height = height;
    }


    void PlatformController::_platform_begin_frame() {

    }

    void PlatformController::_platform_end_frame() {
        glfwSwapBuffers(m_window->handle);
    }

    void PlatformController::set_enable_cursor(bool enabled) {
        if (enabled) {
            glfwSetInputMode(m_window->handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(m_window->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }


    void initialize_key_maps() {
#include "glfw_key_mapping.h"
    }


    static void glfw_mouse_callback(GLFWwindow *window, double x, double y) {
        rg::ControllerManager::get<PlatformController>()->_platform_on_mouse(x, y);
    }

    static void glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        g_mouse_position.scroll = yoffset;
        rg::ControllerManager::get<PlatformController>()->_platform_on_scroll(xoffset, yoffset);
    }


    static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        rg::ControllerManager::get<PlatformController>()->_platform_on_keyboard(key, action);
    }

    static void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
        rg::ControllerManager::get<PlatformController>()->_platform_on_framebuffer_resize(width, height);
    }

    /*
     * SHADER IMPL
     */
    int to_opengl_type(ShaderType type);

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
    unsigned ShaderProgram::id() const {
        return m_shaderId;
    }

    void ShaderProgram::set_bool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(m_shaderId, name.c_str()), (int) value);
    }

    void ShaderProgram::set_int(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(m_shaderId, name.c_str()), value);
    }

    void ShaderProgram::set_float(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(m_shaderId, name.c_str()), value);
    }
    void ShaderProgram::set_vec2(const std::string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, &value[0]);
    }
    void ShaderProgram::set_vec2(const std::string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(m_shaderId, name.c_str()), x, y);
    }
    void ShaderProgram::set_vec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, &value[0]);
    }
    void ShaderProgram::set_vec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(m_shaderId, name.c_str()), x, y, z);
    }
    void ShaderProgram::set_vec4(const std::string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, &value[0]);
    }
    void ShaderProgram::set_vec4(const std::string &name, float x, float y, float z, float w) {
        glUniform4f(glGetUniformLocation(m_shaderId, name.c_str()), x, y, z, w);
    }
    void ShaderProgram::set_mat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void ShaderProgram::set_mat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void ShaderProgram::set_mat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    ShaderProgram::ShaderProgram(unsigned shaderId) : m_shaderId(shaderId) {
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
            throw ShaderCompilationError(std::format("{} shader compilation {} failed:\n{}", to_string(type), m_shader_name,
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
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tex_cords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));

        glBindVertexArray(0);

        return {VAO, indices.size(), std::vector(range(textures))};
    }

    void Mesh::initialize() {
    }

    void Mesh::draw(ShaderProgram *shader) {
        std::unordered_map<std::string_view, uint32_t> counts;
        std::string uniform_name;
        uniform_name.reserve(32);
        for (int i = 0; i < m_textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            const auto& texture_type = texture_type_to_uniform_name_convention(m_textures[i]->type());
            uniform_name.append(texture_type);
            const auto count = (counts[texture_type] += 1);
            uniform_name.append(std::to_string(count));
            shader->set_int(uniform_name, i);
            glBindTexture(GL_TEXTURE_2D, m_textures[i]->id());
            uniform_name.clear();
        }
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::destroy() {
        glDeleteVertexArrays(1, &m_vao);
    }

    std::string_view Mesh::texture_type_to_uniform_name_convention(TextureType type) {
        switch (type) {
        case TextureType::Diffuse: return "texture_diffuse";
        case TextureType::Specular: return "texture_specular";
        case TextureType::Normal: return "texture_normal";
        case TextureType::Height: return "texture_height";
        default: RG_UNIMPLEMENTED("Unknown TextureType");
        }
    }

    std::string_view texture_type_to_string(TextureType type) {
        switch (type) {
        case TextureType::Diffuse: return "Diffuse";
        case TextureType::Specular: return "Specular";
        case TextureType::Normal: return "Normal";
        case TextureType::Height: return "Height";;
        default: RG_SHOULD_NOT_REACH_HERE("Unknown TextureType");
        }
    }

    Texture Texture::create_from_file(std::filesystem::path path, TextureType type) {
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
        return Texture(texture_id, type);
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



}// namespace rg
