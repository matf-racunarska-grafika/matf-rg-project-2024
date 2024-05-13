//
// Created by oracle on 13.5.24..
//

#ifndef MATF_RG_PROJECT_UTILS_H
#define MATF_RG_PROJECT_UTILS_H

#include<memory>

#define RG_GUARANTEE(expr, msg) do { rg::utils::EngineError::guarantee(expr, #expr, msg, __FILE__, __LINE__); } while(0)
#define RG_SHOULD_NOT_REACH_HERE(msg) do { rg::utils::EngineError::should_not_reach_here(msg, __FILE__, __LINE__); } while(0)
#define RG_UNIMPLEMENTED(msg) do { rg::utils::EngineError::unimplemented(msg, __FILE__, __LINE__); } while(0)

namespace rg::utils {

    class EngineError {
    public:
        static void guarantee(bool expr, std::string_view expr_str, std::string_view msg, std::string_view file, int line);
        static void should_not_reach_here(std::string_view msg, std::string_view file, int line);
        static void unimplemented(std::string_view msg, std::string_view file, int line);
    };

}


#endif //MATF_RG_PROJECT_UTILS_H
