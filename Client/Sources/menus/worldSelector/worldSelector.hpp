#ifndef worldSelector_hpp
#define worldSelector_hpp

#include "graphics.hpp"

class WorldSelector : public gfx::Scene {
    struct world_to_select {
        std::string name;
        explicit world_to_select(std::string name) : name(std::move(name)) {}
        gfx::Button button, delete_button;
        void render(int position);
        int button_y{};
    };

    gfx::Sprite title;
    gfx::Button back_button, new_button;
    std::vector<std::string> worlds_names;
    std::vector<world_to_select> worlds;
    int scroll_limit, position;
    bool shift_pressed = false;
    
    void init() override;
    void onKeyDown(gfx::Key key) override;
    void onKeyUp(gfx::Key key) override;
    void render() override;
    void onMouseScroll(int distance) override;
    void refresh();
};

#endif
