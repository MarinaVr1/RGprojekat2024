//
// Created by marina on 7/21/2025.
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>

namespace app {
struct PointLightSettings {
    glm::vec3 position;
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

class GUIController : public engine::core::Controller {
public:
    GUIController();

    std::array<PointLightSettings, 4> jellyfishLights;

    glm::vec3 dirLightColor;

    float emissiveStrength = 5.0f;
    float tilingFactor = 10.0f;
    float heightScale = 0.1f;

    std::string_view name() const override { return "app::GUIController"; }

private:
    void initialize() override;

    void poll_events() override;

    void draw() override;
};
}

#endif //GUICONTROLLER_HPP
