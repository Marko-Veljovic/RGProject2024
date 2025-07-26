#ifndef BLOOMEFFECT_HPP
#define BLOOMEFFECT_HPP

class BloomEffect {
private:
    unsigned int m_hdr_fbo;
    unsigned int m_rbo_depth;
    unsigned int m_color_buffers[3];

    unsigned int m_ping_pong_fbo[4];
    unsigned int m_ping_pong_color_buffers[4];

    unsigned int m_quad_vao = 0;
    unsigned int m_quad_vbo;

public:
    void init(unsigned int buffer_width, unsigned int buffer_height);

    void resize(unsigned int width, unsigned int height);

    void bind_hdr_fbo();

    void bind_ping_pong_fbo(bool horizontal);

    void bind_ping_pong_fbo2(bool horizontal);

    void bind_ping_pong_texture(bool first_iteration, bool horizontal);

    void bind_ping_pong_texture2(bool first_iteration, bool horizontal);

    void bind_default_fbo();

    void clear_color_depth_buffers();

    void finalize(bool horizontal);

    void active_volumetric_texture(bool horizontal);

    void render_quad();
};

#endif //BLOOMEFFECT_HPP
