#ifndef resourcePack_hpp
#define resourcePack_hpp

#include "properties.hpp"
#include "graphics.hpp"

class ResourcePack {
    gfx::Image item_textures[(int)ItemType::NUM_ITEMS], item_text_textures[(int)ItemType::NUM_ITEMS], breaking_texture, player_texture, background, block_texture_atlas, liquid_texture_atlas, item_texture_atlas;
    gfx::RectShape block_texture_rectangles[(int)BlockType::NUM_BLOCKS];
    gfx::RectShape liquid_texture_rectangles[(int)LiquidType::NUM_LIQUIDS];
    //gfx::RectShape item_texture_rectangles[(int)BlockType::NUM_BLOCKS];
public:
    void load(const std::string& path);
    void loadBlocks(const std::string& path);
    void loadLiquids(const std::string& path);
    //void loadItems(std::string path);

    const gfx::Image& getBlockTexture();
    const gfx::Image& getItemTexture(ItemType type);
    const gfx::Image& getItemTextTexture(ItemType type);
    const gfx::Image& getLiquidTexture();
    const gfx::Image& getBreakingTexture();
    const gfx::Image& getPlayerTexture();
    const gfx::Image& getBackground();
    const gfx::RectShape& getTextureRectangle(BlockType type);
    const gfx::RectShape& getTextureRectangle(LiquidType type);
    const gfx::RectShape& getTextureRectangle(ItemType type);

};

#endif
