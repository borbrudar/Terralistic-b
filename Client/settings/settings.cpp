#include "settings.hpp"

Settings::~Settings() {
    if(!settings.empty())
        std::cout << "Warning: settings not empty when destructed!" << std::endl;
}

const std::vector<Setting*>& Settings::getSettings() {
    return settings;
}

void Settings::addSetting(Setting* setting) {
    settings.push_back(setting);
    if(config_file.keyExists(setting->ident))
        setting->loadFromStr(config_file.getStr(setting->ident));
    setting->setting_change_event.addListener(this);
}

void Settings::removeSetting(Setting* setting) {
    setting->setting_change_event.removeListener(this);
    auto pos = std::find(settings.begin(), settings.end(), setting);
    if(pos == settings.end())
        throw Exception("Removed non-existing setting.");
    settings.erase(pos);
    config_file.setStr(setting->ident, setting->exportToStr());
}

std::string ChoiceSetting::exportToStr() {
    return std::to_string(getSelectedChoice());
}

void ChoiceSetting::loadFromStr(std::string value) {
    setSelectedChoice(std::stoi(value));
}

int ChoiceSetting::getSelectedChoice() const {
    return selected_choice;
}

void ChoiceSetting::setSelectedChoice(int choice) {
    selected_choice = choice;
    
    SettingChangeEvent event;
    setting_change_event.call(event);
}

std::string BooleanSetting::exportToStr() {
    return std::to_string(getValue());
}

void BooleanSetting::loadFromStr(std::string value_) {
    setValue(std::stoi(value_));
}

bool BooleanSetting::getValue() const {
    return value;
}

void BooleanSetting::setValue(bool new_value) {
    value = new_value;
    
    SettingChangeEvent event;
    setting_change_event.call(event);
}

std::string SliderSetting::exportToStr() {
    return std::to_string(getSelectedChoice());
}

void SliderSetting::loadFromStr(std::string value) {
    setSelectedChoice(std::stoi(value));
}

int SliderSetting::getSelectedChoice() const {
    return selected_choice;
}

int SliderSetting::getSliderValue() const {
    return min + (selected_choice - (int)choices.size()) * step;
}

void SliderSetting::setSelectedChoice(int choice) {
    selected_choice = choice;
    
    SettingChangeEvent event;
    setting_change_event.call(event);
}

void Settings::reloadSettings() {
    is_loading = true;
    config_file.reloadFromDisk();
    for(auto & setting : settings)
        if(config_file.keyExists(setting->ident))
            setting->loadFromStr(config_file.getStr(setting->ident));
    is_loading = false;
}

#define SETTINGS_RELOAD_INTERVAL 5000

void Settings::update() {
    if(timer.getTimeElapsed() > SETTINGS_RELOAD_INTERVAL) {
        timer.reset();
        reloadSettings();
    }
}

void Settings::onEvent(SettingChangeEvent& event) {
    if(!is_loading) {
        for(auto & setting : settings)
            config_file.setStr(setting->ident, setting->exportToStr());
        config_file.saveConfig();
    }
}
