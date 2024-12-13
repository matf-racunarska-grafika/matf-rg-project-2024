
#ifndef DECLARATIONS_HPP
#define DECLARATIONS_HPP

namespace engine {
    namespace controller {
        class Controller;
        class ControllerManager;
        class EngineSentinelController;
    };

    namespace core {
        class App;
    }

    namespace platform {
        class Key;
        struct MousePosition;
        class PlatformController;
    }

    namespace graphics {
        class Camera;
        class OpenGL;
        class GraphicsController;
    }

    namespace resources {
        class Mesh;
        class Model;
        class Shader;
        class ShaderProgram;
        class ShaderCompiler;
        class Skybox;
        class Texture;
        class ResourceController;

    }

    namespace util {
        class ArgParser;
        class Configuration;
        class Error;
        class EngineError;
    }
}

#endif //DECLARATIONS_HPP
