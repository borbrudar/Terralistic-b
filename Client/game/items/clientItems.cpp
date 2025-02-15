#include "clientItems.hpp"
#include "readOpa.hpp"

void ClientItems::init() {
    networking->packet_event.addListener(this);
    item_creation_event.addListener(this);
    entities->entity_deletion_event.addListener(this);
}

void ClientItems::loadTextures() {
    std::vector<gfx::Surface> item_surfaces(getNumItemTypes() - 1);

    for(int i = 1; i < getNumItemTypes(); i++)
        item_surfaces[i - 1] = readOpa(resource_pack->getFile("/items/" + getItemTypeById(i)->name + ".opa"));

    items_atlas.create(item_surfaces);
}

void ClientItems::stop() {
    networking->packet_event.removeListener(this);
    item_creation_event.removeListener(this);
    entities->entity_deletion_event.removeListener(this);
}

void ClientItems::onEvent(ItemCreationEvent& event) {
    item_count++;
    item_count_changed = true;
}

void ClientItems::onEvent(EntityDeletionEvent& event) {
    if(event.entity->type == EntityType::ITEM) {
        item_count--;
        item_count_changed = true;
    }
}

void ClientItems::render() {
    int curr_item_count = item_count;
    if(item_count_changed) {
        item_rects.resize(item_count);
        item_count_changed = false;
    }
    
    int item_index = 0;
    for(int i = 0; i < entities->getEntities().size() && item_index < curr_item_count; i++) {
        if(entities->getEntities()[i]->type == EntityType::ITEM) {
            Item* item = (Item*)entities->getEntities()[i];
            gfx::RectShape rect = getItemRectInAtlas(item->getType());
            if(item->entity_item_count == 1)
                item_rects.setTextureCoords(item_index, {rect.x, rect.y, rect.w / 2, rect.h});
            else
                item_rects.setTextureCoords(item_index, {rect.x + rect.w / 2, rect.y, rect.w / 2, rect.h});

            int item_x = item->getX() - camera->getX() + gfx::getWindowWidth() / 2;
            int item_y = item->getY() - camera->getY() + gfx::getWindowHeight() / 2;
            item_rects.setRect(item_index, {item_x + BLOCK_WIDTH - rect.w / 2, item_y - rect.h * 2 + 16, rect.w, rect.h * 2});
            item_rects.setColor(item_index, {255, 255, 255});
            item_index++;
        }
    }
    
    if(item_index)
        item_rects.render(&getItemsAtlasTexture());
}

void ClientItems::onEvent(ClientPacketEvent& event) {
    switch(event.packet_type) {
        case ServerPacketType::ITEM_CREATION: {
            int x, y;
            int id;
            int type;
            event.packet >> x >> y >> id >> type;
            
            Item* item = new Item(getItemTypeById(type), x, y, 1, id);
            entities->registerEntity(item);
            
            ItemCreationEvent item_event(item);
            item_creation_event.call(item_event);
            
            break;
        }
        case ServerPacketType::ITEM_COUNT_CHANGE:{
            int id, entity_item_count;
            event.packet >> id >> entity_item_count;
            Item* item = (Item*)entities->getEntityById(id);
            item->entity_item_count = entity_item_count;
        }
        default:;
    }
}

const gfx::Texture& ClientItems::getItemsAtlasTexture() {
    return items_atlas.getTexture();
}

gfx::RectShape ClientItems::getItemRectInAtlas(ItemType* type) {
    return items_atlas.getRect(type->id - 1);
}
