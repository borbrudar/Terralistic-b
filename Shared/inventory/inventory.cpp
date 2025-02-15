#include <algorithm>
#include "inventory.hpp"
#include <cstring>
#include "player.hpp"

int Inventory::countItems(int item_id) {
    int result = 0;
    for(int i = 0; i < 20; i++){
        if(getItem(i).type->id == item_id)
            result += getItem(i).stack;
    }
    return result;
}

void Inventory::setItem(int pos, ItemStack item) {
    if(item_counts.empty())
        throw Exception("item_counts is empty");
    item_counts[(int)getItem(pos).type->id] -= getItem(pos).stack;
    item_counts[(int)item.type->id] += item.stack;
    ItemStack& item_stack = pos == -1 ? mouse_item : inventory_arr[pos];
    item_stack = item;
    updateAvailableRecipes();
    
    InventoryItemChangeEvent event(pos);
    item_change_event.call(event);
}

int Inventory::increaseStack(int pos, int stack) {
    int stack_to_be = getItem(pos).stack + stack, result;
    if(stack_to_be > getItem(pos).type->max_stack)
        stack_to_be = getItem(pos).type->max_stack;
    result = stack_to_be - getItem(pos).stack;
    setItem(pos, ItemStack(getItem(pos).type, stack_to_be));
    return result;
}

int Inventory::decreaseStack(int pos, int stack) {
    if(stack >= getItem(pos).stack) {
        int prev_stack = getItem(pos).stack;
        setItem(pos, ItemStack(&items->nothing, 0));
        return prev_stack;
    } else {
        setItem(pos, ItemStack(getItem(pos).type, getItem(pos).stack - stack));
        return stack;
    }
}

void Inventory::fromSerial(const std::vector<char>& serial) {
    const char* iter = &serial[0];
    for(auto & i : inventory_arr) {
        i.type = items->getItemTypeById((int)*iter++);
        i.stack = 0;
        memcpy(&i.stack, iter, sizeof(short));
        iter += sizeof(short);
        item_counts[(int)i.type->id] += i.stack;
    }
    updateAvailableRecipes();
}

std::vector<char> Inventory::toSerial() const {
    std::vector<char> serial;
    for(auto i : inventory_arr) {
        serial.push_back((char)i.type->id);
        serial.insert(serial.end(), {0, 0});
        memcpy(&serial[serial.size() - 2], &i.stack, sizeof(short));
    }
    return serial;
}

Inventory::Inventory(Items* items, Recipes* recipes) : items(items), recipes(recipes), mouse_item(&items->nothing, 0) {
    item_counts.resize(items->getNumItemTypes());
    for(int i = 0; i < items->getNumItemTypes(); i++)
        item_counts[i] = 0;
    
    for(auto & i : inventory_arr)
        i.type = &items->nothing;
}

Inventory::Inventory(const Inventory& inventory) {
    *this = inventory;
}

int Inventory::addItem(ItemType* id, int quantity) {
    if(quantity < 0)
        throw Exception("Item quantity cannot be negative.");
    for(int i = 0; i < INVENTORY_SIZE; i++)
        if(getItem(i).type == id) {
            quantity -= increaseStack(i, quantity);
            if(!quantity)
                return i;
        }
    for(int i = 0; i < INVENTORY_SIZE; i++)
        if(getItem(i).type == &items->nothing) {
            setItem(i, ItemStack(id, getItem(i).stack));
            quantity -= increaseStack(i, quantity);
            if(!quantity)
                return i;
        }
    return -1;
}

int Inventory::removeItem(ItemType* id, int quantity) {
    if(quantity < 0)
        throw Exception("Item quantity cannot be negative.");
    for(int i = 0; i < INVENTORY_SIZE; i++)
        if(inventory_arr[i].type == id) {
            quantity -= decreaseStack(i, quantity);
            if(!quantity)
                return i;
        }
    if(mouse_item.type == id) {
        quantity -= decreaseStack(-1, quantity);
        if(!quantity)
            return -1;
    }
    return -1;
}

ItemStack Inventory::getItem(int pos) {
    if(pos < -1 || pos >= INVENTORY_SIZE)
        throw Exception("Inventory position does not exist");
    if(pos == -1)
        return mouse_item;
    return inventory_arr[pos];
}

ItemStack Inventory::getSelectedSlot() {
    return getItem(selected_slot);
}

void Inventory::swapWithMouseItem(int pos) {
    if(pos < 0 || pos >= INVENTORY_SIZE)
        throw Exception("Inventory position does not exist");
    ItemStack temp = mouse_item;
    mouse_item = inventory_arr[pos];
    inventory_arr[pos] = temp;
}

bool Inventory::canCraftRecipe(const Recipe* recipe) {
    if(std::all_of(recipe->ingredients.begin(), recipe->ingredients.end(), [this](auto ingredient){
        if(item_counts.empty())
            throw Exception("item_counts is empty");
        return countItems((int)ingredient.first->id) >= ingredient.second;
    })) {
        if (recipe->crafting_block == nullptr) {
            return true;
        } else if (player != nullptr && blocks != nullptr) {
            for (int i = player->getX() / (BLOCK_WIDTH * 2) - 3;
                 i < player->getX() / (BLOCK_WIDTH * 2) + 3; i++) {
                for (int j = player->getY() / (BLOCK_WIDTH * 2) - 3;
                     j < player->getY() / (BLOCK_WIDTH * 2) + 3; j++) {
                    if (i >= 0 && j >= 0 && i < blocks->getWidth() && j < blocks->getHeight() &&
                        blocks->getBlockType(i, j) == recipe->crafting_block) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

const std::vector<const Recipe*>& Inventory::getAvailableRecipes() {
    return available_recipes;
}

void Inventory::updateAvailableRecipes() {
    for(int i = 0; i < available_recipes.size(); i++){
        if(!canCraftRecipe(available_recipes[i]))
            available_recipes.erase(available_recipes.begin() + i);
    }

    for(auto i : recipes->getAllRecipes())
        if(canCraftRecipe(i) && !std::count(available_recipes.begin(), available_recipes.end(), i))
            available_recipes.emplace_back(i);
}

void Recipes::registerARecipe(Recipe* recipe) {
    recipes.push_back(recipe);
}

const std::vector<Recipe*>& Recipes::getAllRecipes() {
    return recipes;
}

void Inventory::operator=(const Inventory &inventory) {
    items = inventory.items;
    recipes = inventory.recipes;
    player = inventory.player;
    blocks = inventory.blocks;
    mouse_item = inventory.mouse_item;
    item_counts = inventory.item_counts;
    available_recipes = inventory.available_recipes;
    for(int i = 0; i < INVENTORY_SIZE; i++)
        inventory_arr[i] = inventory.inventory_arr[i];
    selected_slot = inventory.selected_slot;
}
