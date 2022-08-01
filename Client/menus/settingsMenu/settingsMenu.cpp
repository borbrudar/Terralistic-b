#include "settingsMenu.hpp"

void SettingsMenu::init() {
    for(auto i : settings->getSettings()) {
        switch(i->type) {
            case SettingType::CHOICE_SETTING: {
                ChoiceSetting* choice_setting = (ChoiceSetting*)i;
                RenderChoiceSetting* render_choice_setting = new RenderChoiceSetting(choice_setting);
                render_settings.push_back(render_choice_setting);
                break;
            }
            case SettingType::BOOLEAN_SETTING: {
                BooleanSetting* boolean_setting = (BooleanSetting*)i;
                RenderBooleanSetting* render_boolean_setting = new RenderBooleanSetting(boolean_setting);
                render_settings.push_back(render_boolean_setting);
                break;
            }
        }
        
        if(render_settings.back()->getWidth() > required_width)
            required_width = render_settings.back()->getWidth();
    }
    
    back_button.loadFromText("Back");
    back_button.scale = 3;
    back_button.orientation = gfx::BOTTOM;
    back_button.y = -SPACING;
}

void SettingsMenu::stop() {
    for(auto & render_setting : render_settings)
        delete render_setting;
}

bool SettingsMenu::onKeyUp(gfx::Key key) {
    if(key == gfx::Key::MOUSE_LEFT) {
        if(back_button.isHovered(getMouseX(), getMouseY()))
            returnFromScene();
        for(auto & render_setting : render_settings)
            render_setting->onMouseButtonUp(getMouseX(), getMouseY());
        return true;
    }
    return false;
}

void SettingsMenu::render() {
    background->setBackWidth(required_width + 6 * SPACING);
    background->renderBack();
    
    int y = 0;
    for(auto & render_setting : render_settings) {
        int width = required_width, height = render_setting->getHeight();
        y += SPACING;
        
        gfx::Color c = BLACK;
        c.a = TRANSPARENCY;
        gfx::RectShape(gfx::getWindowWidth() / 2 - width / 2, y, width, height).render(c);
        
        render_setting->render(y, required_width, getMouseX(), getMouseY());
        
        y += height;
    }
    
    
    back_button.render(getMouseX(), getMouseY());
}

RenderChoiceSetting::RenderChoiceSetting(ChoiceSetting* setting) : setting(setting) {
    choice_text.loadFromText(setting->ident);
    choice_text.scale = 3;
    choice_text.orientation = gfx::TOP;
    select_rect.fill_color = DARK_GREY;
    select_rect.smooth_factor = 2;
    select_rect.orientation = gfx::TOP;
    for(const auto & choice : setting->choices) {
        gfx::Button* button = new gfx::Button;
        button->loadFromText(choice);
        button->scale = 2;
        button->orientation = gfx::TOP;
        button->margin = 5;
        choice_buttons.push_back(button);
    }
}

void RenderChoiceSetting::render(int y, int width, int mouse_x, int mouse_y) {
    int x = width / 2 - SPACING;
    for(int i = (int)choice_buttons.size() - 1; i >= 0; i--) {
        gfx::Button* button = choice_buttons[i];
        x -= button->getWidth() / 2;
        button->x = x;
        x -= button->getWidth() / 2;
        button->y = y + getHeight() / 2 - button->getHeight() / 2;
    }
    
    gfx::Button* selected_button = choice_buttons[setting->getSelectedChoice()];
    select_rect.setX(selected_button->x);
    select_rect.setY(selected_button->y);
    select_rect.setWidth(selected_button->getWidth());
    select_rect.setHeight(selected_button->getHeight());
    select_rect.render();
    
    choice_text.y = y + getHeight() / 2 - choice_text.getHeight() / 2;
    choice_text.x = -width / 2 + choice_text.getWidth() / 2 + SPACING;
    
    choice_text.render();
    
    for(int i = 0; i < (int)choice_buttons.size(); i++)
        choice_buttons[i]->render(mouse_x, mouse_y);
}

int RenderChoiceSetting::getHeight() {
    int height = 0;
    height = std::max(height, choice_text.getHeight() + 2 * SPACING);
    return height;
}

int RenderChoiceSetting::getWidth() {
    int width = choice_text.getWidth() + 3 * SPACING;
    for(auto & choice_button : choice_buttons)
        width += choice_button->getWidth();
    return width;
}

void RenderChoiceSetting::onMouseButtonUp(int x, int y) {
    for(int i = 0; i < choice_buttons.size(); i++)
        if(choice_buttons[i]->isHovered(x, y))
            setting->setSelectedChoice(i);
}

RenderBooleanSetting::RenderBooleanSetting(BooleanSetting* setting) : setting(setting) {
    text.loadFromText(setting->ident);
    text.scale = 3;
    text.orientation = gfx::TOP;
    
    toggle_button.scale = 2;
    toggle_button.orientation = gfx::TOP;
    toggle_button.margin = 5;
    
    updateButtonText();
}

void RenderBooleanSetting::render(int y, int width, int mouse_x, int mouse_y) {
    text.y = y + getHeight() / 2 - text.getHeight() / 2;
    text.x = -width / 2 + text.getWidth() / 2 + SPACING;
    
    toggle_button.y = y + getHeight() / 2 - toggle_button.getHeight() / 2;
    toggle_button.x = width / 2 - toggle_button.getWidth() / 2 - SPACING;
    toggle_button.render(mouse_x, mouse_y);
    
    text.render();
}

void RenderBooleanSetting::updateButtonText() {
    if(setting->getValue()) {
        toggle_button.def_color = {44, 159, 44};
        toggle_button.hover_color = {44, 199, 44};
        toggle_button.loadFromText("On");
    } else {
        toggle_button.def_color = GFX_DEFAULT_BUTTON_COLOR;
        toggle_button.hover_color = GFX_DEFAULT_HOVERED_BUTTON_COLOR;
        toggle_button.loadFromText("Off");
    }
}

int RenderBooleanSetting::getHeight() {
    int height = 0;
    height = std::max(height, text.getHeight() + 2 * SPACING);
    height = std::max(height, toggle_button.getHeight() + 2 * SPACING);
    return height;
}

int RenderBooleanSetting::getWidth() {
    int width = text.getWidth() + 3 * SPACING;
    return width;
}

void RenderBooleanSetting::onMouseButtonUp(int x, int y) {
    if(toggle_button.isHovered(x, y)) {
        setting->setValue(!setting->getValue());
        updateButtonText();
    }
}
