#include <engine/graphics/Bloom.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

using namespace engine::graphics;

Bloom::Bloom() = default;

Bloom::~Bloom() {
    CHECKED_GL_CALL(glDeleteFramebuffers, 1, &hdrFBO);
    CHECKED_GL_CALL(glDeleteTextures, 2, colorBuffers);
    CHECKED_GL_CALL(glDeleteRenderbuffers, 1, &rboDepth);

    for (int i = 0; i < 2; ++i) {
        CHECKED_GL_CALL(glDeleteFramebuffers, 1, &pingpongFBO[i]);
        CHECKED_GL_CALL(glDeleteTextures, 1, &pingpongColorBuffers[i]);
    }

    if (quadVAO)
        CHECKED_GL_CALL(glDeleteVertexArrays, 1, &quadVAO);
    if (quadVBO)
        CHECKED_GL_CALL(glDeleteBuffers, 1, &quadVBO);
}

void Bloom::init(int screenWidth, int screenHeight,
                 engine::resources::Shader *shaderBlur,
                 engine::resources::Shader *shaderFinal) {
    width_ = screenWidth;
    height_ = screenHeight;
    shaderBlur_ = shaderBlur;
    shaderFinal_ = shaderFinal;

    CHECKED_GL_CALL(glGenFramebuffers, 1, &hdrFBO);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, hdrFBO);

    CHECKED_GL_CALL(glGenTextures, 2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, colorBuffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }

    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    CHECKED_GL_CALL(glDrawBuffers, 2, attachments);

    CHECKED_GL_CALL(glGenRenderbuffers, 1, &rboDepth);
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, rboDepth);
    CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);


    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

    CHECKED_GL_CALL(glGenFramebuffers, 2, pingpongFBO);
    CHECKED_GL_CALL(glGenTextures, 2, pingpongColorBuffers);
    for (unsigned int i = 0; i < 2; i++) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, pingpongFBO[i]);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, pingpongColorBuffers[i]);
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffers[i], 0);

        if (CHECKED_GL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { spdlog::error("Pingpong FBO is not complete", i); }
    }

    shaderBlur_->use();
    shaderBlur_->set_int("image", 0);

    shaderFinal_->use();
    shaderFinal_->set_int("scene", 0);
    shaderFinal_->set_int("bloomBlur", 1);
    shaderFinal_->set_float("exposure", 1.0f);
}

void Bloom::renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
                -1.0f, 1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 1.0f, 0.0f,
        };

        CHECKED_GL_CALL(glGenVertexArrays, 1, &quadVAO);
        CHECKED_GL_CALL(glGenBuffers, 1, &quadVBO);

        CHECKED_GL_CALL(glBindVertexArray, quadVAO);
        CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, quadVBO);
        CHECKED_GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        CHECKED_GL_CALL(glEnableVertexAttribArray, 0);
        CHECKED_GL_CALL(glVertexAttribPointer, 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);

        CHECKED_GL_CALL(glEnableVertexAttribArray, 1);
        CHECKED_GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

        CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
        CHECKED_GL_CALL(glBindVertexArray, 0);
    }

    CHECKED_GL_CALL(glBindVertexArray, quadVAO);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
    CHECKED_GL_CALL(glBindVertexArray, 0);
}

void Bloom::render() {
    bool horizontal = true, first_iteration = true;
    int amount = 10;

    shaderBlur_->use();
    for (int i = 0; i < amount; i++) {
        CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        shaderBlur_->set_bool("horizontal", horizontal);
        CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
        CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorBuffers[!horizontal]);
        renderQuad();
        horizontal = !horizontal;
        if (first_iteration) first_iteration = false;
    }

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderFinal_->use();
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, colorBuffers[0]);
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE1);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);
    renderQuad();
}

void Bloom::resize(int newWidth, int newHeight) {

    if (newWidth == width_ && newHeight == height_) return;

    width_ = newWidth;
    height_ = newHeight;

    CHECKED_GL_CALL(glDeleteFramebuffers, 1, &hdrFBO);
    CHECKED_GL_CALL(glDeleteTextures, 2, colorBuffers);
    CHECKED_GL_CALL(glDeleteRenderbuffers, 1, &rboDepth);
    CHECKED_GL_CALL(glDeleteFramebuffers, 2, pingpongFBO);
    CHECKED_GL_CALL(glDeleteTextures, 2, pingpongColorBuffers);

    init(newWidth, newHeight, shaderBlur_, shaderFinal_);
}

void Bloom::begin_frame() {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, hdrFBO);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Bloom::end_frame() {
    CHECKED_GL_CALL(glDisable, GL_DEPTH_TEST);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    render();
    CHECKED_GL_CALL(glEnable, GL_DEPTH_TEST);
}

void Bloom::update_resize() {
    auto platform = engine::platform::PlatformController::get<engine::platform::PlatformController>();
    int currentWidth = platform->window()->width();
    int currentHeight = platform->window()->height();

    if (currentWidth != width_ || currentHeight != height_) { if (shaderBlur_ && shaderFinal_) { resize(currentWidth, currentHeight); } }
}
