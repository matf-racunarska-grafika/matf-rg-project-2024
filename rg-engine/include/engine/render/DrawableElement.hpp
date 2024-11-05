
//

#ifndef MATF_RG_PROJECT_DRAWABLEELEMENT_HPP
#define MATF_RG_PROJECT_DRAWABLEELEMENT_HPP
#include <string_view>
namespace rg {
    class ShaderProgram;
    class DrawableElement {
    public:
        virtual void initialize() = 0;
        virtual void draw(ShaderProgram *shader) = 0;
        virtual void destroy() = 0;
        virtual std::string_view name() const = 0;
        virtual ~DrawableElement() = default;
    };
}// namespace rg
#endif//MATF_RG_PROJECT_DRAWABLEELEMENT_HPP
