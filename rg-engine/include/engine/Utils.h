//
// Created by spaske00 on 13.5.24..
//

#ifndef MATF_RG_PROJECT_UTILS_H
#define MATF_RG_PROJECT_UTILS_H

#include <source_location>
#include <memory>

namespace rg::utils {

    class EngineError {
    public:
        static void guarantee(bool expr, std::string_view msg, std::source_location source_location = std::source_location::current());
        static void should_not_reach_here(std::string_view msg, std::source_location source_location = std::source_location::current());
        static void unimplemented(std::string_view msg, std::source_location source_location = std::source_location::current());
    };

}


#endif //MATF_RG_PROJECT_UTILS_H
