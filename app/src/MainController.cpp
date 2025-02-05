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
    }

    void MainController::initialize() {
        auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
        platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
        engine::graphics::OpenGL::enable_depth_testing();

        // configure framebuffers for bloom effect
        int screen_width  = platform->window()->width();
        int screen_height = platform->window()->height();

        m_bloom_effect = std::make_unique<BloomEffect>();
        m_bloom_effect->init(screen_width, screen_height);

        auto resources     = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto screen_shader = resources->shader("tmp");
        screen_shader->use();
        screen_shader->set_int("screenTexture", 0);

        auto blur_shader = resources->shader("blur");
        blur_shader->use();
        blur_shader->set_int("image", 0);
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

        // directional lighting
        shader->set_vec3("dirLight.direction", glm::vec3(0.57f, -0.15f, 0.8f));
        // correct dir light
        shader->set_vec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shader->set_vec3("dirLight.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
        shader->set_vec3("dirLight.specular", glm::vec3(0.2f, 0.2f, 0.2f));

        // DEBUG dir light
        // shader->set_vec3("dirLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        // shader->set_vec3("dirLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
        // shader->set_vec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

        shader->set_vec3("viewPos", graphics->camera()->Position);

        // spotlight
        auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
        float time    = platform->frame_time().current;

        shader->set_vec3("spotLight.position", glm::vec3(0.0f, 2.67f, -3.0f));
        shader->set_vec3("spotLight.direction", glm::vec3(glm::sin(2 * time), 0.0f, glm::cos(2 * time)));
        shader->set_float("spotLight.cutOff", glm::cos(glm::radians(34.0f)));
        shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(38.0f)));
        shader->set_float("spotLight.constant", 1.0f);
        shader->set_float("spotLight.linear", 0.009f);
        shader->set_float("spotLight.quadratic", 0.0032f);
        shader->set_vec3("spotLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        shader->set_vec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        lighthouse->draw(shader);
    }

    void MainController::draw_reflector() {
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto shader    = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("lightCube");
        auto reflector = engine::core::Controller::get<engine::resources::ResourcesController>()->model("cube");
        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(0.0f, 2.67f, -3.0f)); // lightCube position
        model           = glm::scale(model, glm::vec3(0.1f));
        shader->set_mat4("model", model);
        reflector->draw(shader);
    }

    void MainController::update_camera() {
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        auto gui_controller = engine::core::Controller::get<GuiController>();
        if (gui_controller->is_enabled()) {
            //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
        auto resources     = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto screen_shader = resources->shader("tmp");
        auto blur_shader   = resources->shader("blur");

        // 1. render scene into floating point framebuffer
        m_bloom_effect->bind_hdr_fbo();

        draw_lighthouse();
        draw_reflector();
        draw_skybox();

        m_bloom_effect->bind_default_fbo();

        // 2. blur bright fragments with two-pass Gaussian Blur
        bool horizontal     = true, first_iteration = true;
        unsigned int amount = 10;
        blur_shader->use();
        for (unsigned int i = 0; i < amount; i++) {
            m_bloom_effect->bind_ping_pong_fbo(horizontal);
            blur_shader->set_int("horizontal", horizontal);
            m_bloom_effect->bind_ping_pong_texture(first_iteration, horizontal);
            m_bloom_effect->render_quad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        m_bloom_effect->bind_default_fbo();
        m_bloom_effect->clear_color_depth_buffers();

        screen_shader->use();
        m_bloom_effect->temporary_function();

        m_bloom_effect->render_quad();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }

} // app
