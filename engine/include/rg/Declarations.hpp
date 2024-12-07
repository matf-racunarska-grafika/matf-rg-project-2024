//
// Created by spaske on 12/7/24.
//

#ifndef DECLARATIONS_HPP
#define DECLARATIONS_HPP

namespace rg { namespace controller {
        class Controller;
        class ControllerManager;
        class EngineSentinelController;
    };

    namespace core {
        class App;
    }

    namespace platform {
        class PlatformController;
    }

    namespace graphics {
        class Camera;
        class OpenGL;
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
        class Unimplemented;
        class ShouldNotReachHere;
        class GuaranteeViolation;
        class FileNotFoundError;
        class ConfigurationError;
        class ShaderCompilationError;
        class AssetLoadingError;

    }}

#endif //DECLARATIONS_HPP
