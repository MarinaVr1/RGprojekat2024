#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;

    bool loop() override;

    void draw_busStop();

    void draw_jellyfish();

    void draw_submarine();

    void draw_gary();

    void draw_sand();

    void begin_draw() override;

    void update_camera();

    void update() override;

    void draw() override;

    void draw_skybox();

    void end_draw() override;

    bool submarineActive = false;

    bool submarineVisible = false;

    float submarineTimer = 0.0f;

    int submarineCounter = 0;

    bool submarineFromLeft = false;

    bool garyVisible = true;

public:
    std::string_view name() const override { return "app::MainController"; }

};
}

#endif //MAINCONTROLLER_HPP