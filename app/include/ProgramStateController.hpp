#ifndef PROGRAMSTATECONTROLLER_HPP
#define PROGRAMSTATECONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {

class ProgramStateController : public engine::core::Controller {
public:
    float m_exposure = 0.1;

    bool m_bloom_enabled = true;

    std::string_view name() const override { return "app::ProgramStateController"; }
};

}// app

#endif //PROGRAMSTATECONTROLLER_HPP
