#pragma once
#include "clientNetworking.hpp"
#include "resourcePack.hpp"
#include "content.hpp"
#include "camera.hpp"

#define BLOCK_CHUNK_SIZE 16

class ClientBlocks : public Blocks, public ClientModule, EventListener<ClientPacketEvent>, EventListener<BlockChangeEvent>, EventListener<WelcomePacketEvent> {
    class RenderBlock {
    public:
        RenderBlock() : variation(rand()), state(16) {}
        int variation:8, state:8;
    };

    class BlockChunk {
        gfx::RectArray block_rects;
        bool is_created = false;
    public:
        bool isCreated() { return is_created; }
        void create(ClientBlocks* blocks, int x, int y);
        void update(ClientBlocks* blocks, int x, int y);
        void render(ClientBlocks* blocks, int x, int y);
    };
    
    void onEvent(ClientPacketEvent& event) override;
    void onEvent(BlockChangeEvent& event) override;
    void onEvent(WelcomePacketEvent& event) override;
    
    RenderBlock* render_blocks = nullptr;
    BlockChunk* block_chunks = nullptr;
    RenderBlock* getRenderBlock(int x, int y);
    
    BlockChunk* getBlockChunk(int x, int y);
    
    gfx::TextureAtlas blocks_atlas;
    gfx::Texture breaking_texture;
    
    void init() override;
    void postInit() override;
    void render() override;
    void update(float frame_length) override;
    void stop() override;
    
    bool updateOrientationSide(ClientBlocks* blocks, int x, int y, int side_x, int side_y);
    void updateOrientationDown(ClientBlocks* blocks, int x, int y);
    void updateOrientationUp(ClientBlocks* blocks, int x, int y);
    void updateOrientationLeft(ClientBlocks* blocks, int x, int y);
    void updateOrientationRight(ClientBlocks* blocks, int x, int y);
    
    ResourcePack* resource_pack;
    ClientNetworking* networking;
    Camera* camera;
    
public:
    ClientBlocks(ResourcePack* resource_pack, ClientNetworking* networking, Camera* camera) : resource_pack(resource_pack), networking(networking), camera(camera) {}
    
    const gfx::Texture& getBlocksAtlasTexture();
    gfx::RectShape getBlockRectInAtlas(BlockType* type);
    
    int getBlocksViewBeginX();
    int getBlocksViewEndX();
    int getBlocksViewBeginY();
    int getBlocksViewEndY();
    
    int getBlocksExtendedViewBeginX();
    int getBlocksExtendedViewEndX();
    int getBlocksExtendedViewBeginY();
    int getBlocksExtendedViewEndY();
    
    void updateState(int x, int y);
    void setState(int x, int y, int state);
    int getState(int x, int y);
};
