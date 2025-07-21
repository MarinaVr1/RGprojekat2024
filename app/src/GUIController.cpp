#include <GUIController.hpp>
#include <imgui.h>
#include <algorithm>
#include <engine/platform/PlatformController.hpp>
#include <glm/gtc/type_ptr.inl>

app::GUIController::GUIController() {
    jellyfishLights[0].position = glm::vec3(7.0f, 0.0f, -2.0f);
    jellyfishLights[1].position = glm::vec3(3.0f, 1.5f, -8.0f);
    jellyfishLights[2].position = glm::vec3(-7.0f, 2.0f, 2.0f);
    jellyfishLights[3].position = glm::vec3(1.0f, 0.5f, 3.0f);
    dirLightColor = glm::vec3(0.2f, 0.4f, 0.6f);
}

void app::GUIController::initialize() { set_enable(false); }

void app::GUIController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed) { set_enable(!is_enabled()); }
}

void app::GUIController::draw() {
    static int selected_tab = 0;
    const char *tabs[] = {"Lights", "Camera", "Bloom", "Parallax"};

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->begin_gui();

    ImGui::Begin("Scene Controls");

    ImGui::BeginChild("Tabs", ImVec2(120, 0), true);
    for (int i = 0; i < IM_ARRAYSIZE(tabs); ++i) { if (ImGui::Selectable(tabs[i], selected_tab == i)) { selected_tab = i; } }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("TabContent", ImVec2(0, 0), false);

    if (selected_tab == 0) {
        ImGui::Text("Jellyfish Lights");
        ImGui::Separator();
        for (int i = 0; i < 4; ++i) {
            std::string label = "Jellyfish " + std::to_string(i + 1);
            ImGui::Text("%s", label.c_str());
            ImGui::DragFloat3(("Position##" + std::to_string(i)).c_str(), &jellyfishLights[i].position.x, 0.1f);
            if (jellyfishLights[i].position.y < 0.0f) jellyfishLights[i].position.y = 0.0f;
            ImGui::DragFloat(("Constant##" + std::to_string(i)).c_str(), &jellyfishLights[i].constant, 0.01f, 0.0f, 2.0f);
            ImGui::DragFloat(("Linear##" + std::to_string(i)).c_str(), &jellyfishLights[i].linear, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat(("Quadratic##" + std::to_string(i)).c_str(), &jellyfishLights[i].quadratic, 0.01f, 0.0f, 1.0f);
            ImGui::Separator();
        }

        ImGui::Text("Directional Light");
        ImGui::ColorEdit3("Color", glm::value_ptr(dirLightColor));
        dirLightColor.r = std::clamp(dirLightColor.r, 0.0f, 0.4f);
        dirLightColor.g = std::clamp(dirLightColor.g, 0.2f, 0.8f);
        dirLightColor.b = std::clamp(dirLightColor.b, 0.4f, 1.0f);
    }

    if (selected_tab == 1) {
        ImGui::Text("Camera Controls");
        ImGui::Separator();

        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        glm::vec3 pos = graphics->camera()->Position;
        float yaw = graphics->camera()->Yaw;
        float pitch = graphics->camera()->Pitch;

        if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.1f)) { graphics->camera()->set_position(pos); }
        if (ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f)) { graphics->camera()->set_yaw(yaw); }
        if (ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f)) { graphics->camera()->set_pitch(pitch); }
    }

    if (selected_tab == 2) {
        ImGui::Text("Bloom Settings");
        ImGui::Separator();
        ImGui::SliderFloat("Emissive Strength", &emissiveStrength, 0.0f, 10.0f);
    }

    if (selected_tab == 3) {
        ImGui::Text("Parallax Mapping");
        ImGui::Separator();
        ImGui::SliderFloat("Height Scale", &heightScale, 0.0f, 0.2f);
        ImGui::SliderFloat("Tiling Factor", &tilingFactor, 0.1f, 10.0f);
    }

    ImGui::EndChild();
    ImGui::End();
    graphics->end_gui();
}

//
// Created by marina on 7/21/2025.
//
