#include "engine/graphics/VolumetricLight.hpp"
#include <glad/glad.h>
#include <engine/graphics/OpenGL.hpp>

#include <spdlog/spdlog.h>

void VolumetricLight::init(unsigned int buffer_width, unsigned int buffer_height) {
    // hdr_FBO
    CHECKED_GL_CALL(glGenFramebuffers, 1, &m_hdr_FBO);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_hdr_FBO);

    CHECKED_GL_CALL(glGenTextures, 1, &m_color_buffer);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_color_buffer);
    CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, buffer_width, buffer_height, 0, GL_RGBA, GL_FLOAT,
                    nullptr);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                    m_color_buffer, 0);

    CHECKED_GL_CALL(glGenRenderbuffers, 1, &m_depth_stencil_buffer);
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, m_depth_stencil_buffer);
    CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH_COMPONENT, buffer_width, buffer_height);
    CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                    m_depth_stencil_buffer);

    if (CHECKED_GL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) spdlog::error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void VolumetricLight::bind_hdr_fbo() { CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_hdr_FBO); }

void VolumetricLight::clear_color_depth_buffers() { CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void VolumetricLight::finalize() {
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_color_buffer);
}
