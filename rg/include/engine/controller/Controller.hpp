
//

#ifndef MATF_RG_PROJECT_CONTROLLER_HPP
#define MATF_RG_PROJECT_CONTROLLER_HPP

#include <string_view>
#include <vector>

namespace rg {

    class Controller {
        friend class ControllerManager;

    public:
        virtual std::string_view name() const = 0;

        virtual ~Controller() = default;

        /*
     * Make controller `next` execute next `this`.
     * c1->before(c2); means that all the controller functions
     * of c1 will execute next controller functions of c2;
     */
        void before(Controller *next) {
            m_next.push_back(next);
        }

        void after(Controller *prev) {
            prev->m_next.push_back(this);
        }

        const std::vector<Controller *> &next() const {
            return m_next;
        }

    private:
        virtual void initialize() {
        }

        virtual void terminate() {
        }

        virtual bool loop() {
            return true;
        }

        virtual void update() {
        }

        virtual void poll_events() {
        }

        virtual void draw() {
        }

        /* List of controller that are dependent on this controller */
        std::vector<Controller *> m_next{};
    };
}// namespace rg

#endif//MATF_RG_PROJECT_CONTROLLER_HPP
