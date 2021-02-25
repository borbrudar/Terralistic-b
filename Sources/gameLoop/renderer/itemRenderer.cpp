//
//  itemRenderer.cpp
//  Terralistic
//
//  Created by Jakob Zorz on 20/02/2021.
//

#define FILENAME itemRenderer
#define NAMESPACE renderer
#include "essential.hpp"

#include "renderer.hpp"
#include "itemEngine.hpp"
#include "events.hpp"
#include "init.hpp"
#include "blockEngine.hpp"
#include "playerHandler.hpp"

std::vector<renderer::uniqueRenderItem> unique_render_items;

renderer::uniqueRenderItem& renderer::getUniqueRenderItem(unsigned short id) {
    return unique_render_items[id];
}

void renderer::renderItems() {
    for(itemEngine::item& i : itemEngine::items)
        swl::render(unique_render_items[i.getItemId()].texture, {short(i.x / 100 - playerHandler::view_x + swl::window_width / 2), short(i.y / 100 - playerHandler::view_y + swl::window_height / 2), BLOCK_WIDTH, BLOCK_WIDTH});
}

INIT_SCRIPT
    INIT_ASSERT(itemEngine::unique_items.size());
    for(itemEngine::uniqueItem& i : itemEngine::unique_items) {
        unique_render_items.emplace_back();
        unique_render_items.back().texture = i.name == "nothing" ? nullptr : swl::loadTextureFromFile("texturePack/items/" + i.name + ".png");
        unique_render_items.back().text_texture.loadFromText(i.name, {255, 255, 255});
        unique_render_items.back().text_texture.scale = 2;
        unique_render_items.back().text_texture.free_texture = false;
    }
INIT_SCRIPT_END
