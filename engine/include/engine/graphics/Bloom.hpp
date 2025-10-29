#pragma once
#include <engine/resources/Shader.hpp>

namespace engine::graphics {

class Bloom {
public:
    Bloom();

    ~Bloom();

    void init(int screenWidth, int screenHeight,
              engine::resources::Shader *shaderBlur,
              engine::resources::Shader *shaderFinal);

    void begin_frame();

    void end_frame();

    void render();

    void resize(int newWidth, int newHeight);

    void update_resize();

private:
    void renderQuad();

    unsigned int hdrFBO = 0;
    unsigned int colorBuffers[2] = {};
    unsigned int rboDepth = 0;
    unsigned int pingpongFBO[2] = {};
    unsigned int pingpongColorBuffers[2] = {};

    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;

    int width_ = 0;
    int height_ = 0;

    engine::resources::Shader *shaderBlur_ = nullptr;
    engine::resources::Shader *shaderFinal_ = nullptr;

};

}
