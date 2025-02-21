#ifndef VOLUMETRICLIGHT_HPP
#define VOLUMETRICLIGHT_HPP

class VolumetricLight {
    unsigned int m_hdr_FBO;

    unsigned int m_color_buffer;

    unsigned int m_depth_stencil_buffer;

public:
    void init(unsigned int screen_width, unsigned int screen_height);

    void bind_hdr_fbo();

    void clear_color_depth_buffers();

    void finalize();
};

#endif //VOLUMETRICLIGHT_HPP
