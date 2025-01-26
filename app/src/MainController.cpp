#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

#include <GUIController.hpp>
#include <MainController.hpp>
#include <spdlog/spdlog.h>

namespace app {

    class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
        void on_mouse_move(engine::platform::MousePosition position) override;

        void on_scroll(engine::platform::MousePosition position) override;
    };

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
        auto gui_controller = engine::core::Controller::get<GuiController>();
        if (!gui_controller->is_enabled()) {
            auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
            auto camera   = graphics->camera();
            camera->rotate_camera(position.dx, position.dy);
        }
    }

    void MainPlatformEventObserver::on_scroll(engine::platform::MousePosition position) {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera   = graphics->camera();
        auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
        auto mouse    = platform->mouse();

        camera->zoom(mouse.scroll);
        graphics->perspective_params().FOV = glm::radians(camera->Zoom);
        spdlog::info("Camera FOV: {}, {}", graphics->perspective_params().FOV, mouse.scroll);
    }

    void MainController::initialize() {
        auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
        platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
        engine::graphics::OpenGL::enable_depth_testing();
    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }

    void MainController::draw_lighthouse() {
        // Model
        auto resource                        = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics                        = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model *lighthouse = resource->model("lighthouse");
        // Shader
        engine::resources::Shader *shader = resource->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
        model           = glm::scale(model, glm::vec3(0.3f));
        shader->set_mat4("model", model);

        // lighting
        shader->set_vec3("dirLight.direction", glm::vec3(0.57f, -0.15f, 0.8f));
        shader->set_vec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shader->set_vec3("dirLight.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
        shader->set_vec3("dirLight.specular", glm::vec3(0.2f, 0.2f, 0.2f));
        shader->set_vec3("viewPos", graphics->camera()->Position);

        lighthouse->draw(shader);
    }

    void MainController::update_camera() {
        auto gui_controller = engine::core::Controller::get<GuiController>();
        if (gui_controller->is_enabled()) {
            return;
        }

        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera   = graphics->camera();
        float dt      = platform->dt();
        if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
        }
        if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
        }
    }

    void MainController::update() {
        update_camera();
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw_skybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto skybox    = resources->skybox("sunset_skybox");
        auto shader    = resources->shader("skybox");
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        graphics->draw_skybox(shader, skybox);
    }

    void MainController::draw() {
        draw_lighthouse();
        draw_skybox();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

} // app
