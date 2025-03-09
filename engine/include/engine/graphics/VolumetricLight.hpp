#ifndef VOLUMETRICLIGHT_HPP
#define VOLUMETRICLIGHT_HPP

class VolumetricLight {
    unsigned int m_hdr_FBO, m_dark_FBO;

    unsigned int m_color_buffer, m_dark_color_buffer;

    unsigned int m_depth_stencil_buffer, m_dark_depth_stencil_buffer;

public:
    void init(unsigned int screen_width, unsigned int screen_height);

    void bind_hdr_fbo();

    void bind_dark_fbo();

    void clear_color_depth_buffers();

    void enable_stencil_test();

    void enable_depth_test();

    void disable_depth_test();

    void stencil_func_not_equal();

    void stencil_op();

    void clear_stencil_buffer();

    void stencil_mask(unsigned int mask);

    void stencil_func_always(unsigned int num);

    void finalize();
};

#endif //VOLUMETRICLIGHT_HPP
