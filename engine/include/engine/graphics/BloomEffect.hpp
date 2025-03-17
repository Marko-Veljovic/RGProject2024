#ifndef BLOOMEFFECT_HPP
#define BLOOMEFFECT_HPP

class BloomEffect {
private:
    unsigned int m_hdr_FBO;
    unsigned int m_color_buffers[3];

    unsigned int m_ping_pong_FBO[2];
    unsigned int m_ping_pong_color_buffers[2];

    unsigned int m_quad_VAO = 0;
    unsigned int m_quad_VBO;

public:
    void init(unsigned int screen_width, unsigned int screen_height);

    void bind_hdr_fbo();

    void bind_ping_pong_fbo(bool horizontal);

    void bind_ping_pong_texture(bool first_iteration, bool horizontal);

    void bind_default_fbo();

    void clear_color_depth_buffers();

    void finalize(bool horizontal);

    void active_dark(bool horizontal);

    void render_quad();
};

#endif //BLOOMEFFECT_HPP
