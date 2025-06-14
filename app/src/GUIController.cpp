#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>

#include <GUIController.hpp>
#include <ProgramStateController.hpp>
#include <imgui.h>

namespace app {

void GuiController::initialize() { set_enable(false); }

void GuiController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_F2).state() == engine::platform::Key::State::JustPressed) { set_enable(!is_enabled()); }
}

void GuiController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto program_state = engine::core::Controller::get<ProgramStateController>();
    auto camera = graphics->camera();
    graphics->begin_gui();

    ImGui::Begin("Camera info");
    ImGui::Text("Camera position: (%f, %f, %f)", camera->Position.x, camera->Position.y, camera->Position.z);
    ImGui::SliderFloat3("Island position: ", program_state->m_island_position, -1.0f, 1.0f);
    ImGui::DragFloat("Exposure", &program_state->m_exposure, 0.01, 0.0, 1.0);
    ImGui::Checkbox("Bloom:", &program_state->m_bloom_enabled);
    ImGui::End();

    graphics->end_gui();
}

void GuiController::terminate() {}

}// app
