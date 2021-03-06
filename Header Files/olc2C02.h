#pragma once
#include <cstdint>
#include <memory>
#include "Cartridge.h"

#include "E:\Workspace\Libraries\olcPixelGameEngine\olcPixelGameEngine.h"
class olc2C02
{
public:
    olc2C02();
    ~olc2C02();

private:
    uint8_t     tblName[2][1024]; // VRAM Name table
    uint8_t     tblPattern[2][4096];
    uint8_t     tblPalette[32]; // RAM that stores pallet information

private:
    olc::Pixel palScreen[0x40];
    olc::Sprite sprScreen           = olc::Sprite(256, 240);
    olc::Sprite sprNameTable[2]     = { olc::Sprite(256, 240), olc::Sprite(256, 240) };
    olc::Sprite sprPatternTable[2]  = { olc::Sprite(128, 128), olc::Sprite(128, 128) };

public:    
    // Debugging Utilities
    olc::Sprite& GetScreen();
    olc::Sprite& GetNameTable(uint8_t i);
    olc::Sprite& GetPatternTable(uint8_t i, uint8_t palette);

    olc::Pixel& GetColourFromPaletteRam(uint8_t palette, uint8_t pixel);

    bool frame_complete = false;

public:
    // Communications with Main Bus
    uint8_t cpuRead(uint16_t addr, bool rdonly = false);
    void    cpuWrite(uint16_t addr, uint8_t data);

    // Communications with PPU Bus
    uint8_t  ppuRead(uint16_t addr, bool rdonly = false);
    void     ppuWrite(uint16_t addr, uint8_t data);

    olc::Pixel& GetColourFromPaletteRam(uint8_t palette, uint8_t pixel);
public:
    // Interface
    void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge);
    void clock();
    void reset();
    bool nmi = false;

private:
    // The Cartridge
    std::shared_ptr<Cartridge> cart;

    union
    {
        struct 
        {
            uint8_t unused : 5;
            uint8_t sprite_overflow : 1;
            uint8_t sprite_zero_hit : 1;
            uint8_t vertical_blank : 1;
        };

        uint8_t reg;
    } status;

    union
    {
        struct
        {
            uint8_t grayscale : 1 ;
            uint8_t render_background_left : 1;
            uint8_t render_sprites_left : 1;
            uint8_t render_background : 1;
            uint8_t render_sprites : 1;
            uint8_t enhance_red : 1;
            uint8_t enhance_green : 1;
            uint8_t enhance_blue : 1;
        };

        uint8_t reg;
    } mask;

    union PPUCTRL
    {
        struct 
        {
            uint8_t nametable_x : 1; 
            uint8_t nametable_y : 1; 
            uint8_t increment_mode : 1;
            uint8_t pattern_sprite : 1;
            uint8_t pattern_background : 1;
            uint8_t sprite_size : 1;
            uint8_t slave_mode : 1; // unused 
            uint8_t enable_nmi : 1;
        };

        uint8_t reg;
    } control;

    uint8_t address_latch = 0x00;
    uint8_t ppu_data_buffer = 0x00;
    uint16_t ppu_address = 0x0000;

private: 
    union loopy_register
    {
        struct 
        {
            
            uint16_t coarse_x : 5;
            uint16_t coarse_y : 5;
            uint16_t nametable_x : 1;
            uint16_t nametable_y : 1;
            uint16_t fine_y : 3;
            uint16_t unused : 1;
        };

        uint16_t reg = 0x0000;
    }; 

    loopy_register vram_addr;
    loopy_register tram_addr;

    uint8_t fine_x = 0x00; // Pixel offset horizontally

    // Internal communications
    uint8_t address_latch = 0x00;
    uint8_t ppu_data_buffer = 0x00;

    // Pixel "dot" position information
    int16_t scanline = 0;
    int16_t cycle = 0;

    // Backgorund rendering
    uint8_t bg_next_tile_id = 0x00;
    uint8_t bg_next_tile_attrib = 0x00;
    uint8_t bg_next_tile_lsb = 0x00;
    uint8_t bg_next_tile_msb = 0x00;
    uint16_t bg_shifter_pattern_lo = 0x0000;
    uint16_t bg_shifter_pattern_hi = 0x0000;
    uint16_t bg_shifter_attrib_lo = 0x0000;
    uint16_t bg_shifter_attrib_hi = 0x0000;

// OAM memory for the PPU
// Store locations of the next fram to be drawn
    struct sObjectAttributeEntry
    {
        uint8_t y; // Y position of the spirte 
        uint8_t id; // ID of tile from pattern memory
        uint8_t attribute; // Flags define how sprite should be rendered 
        uint8_t x; // X position of sprite
    } OAM[64];


    sObjectAttributeEntry spriteScanline[8];
    uint8_t sprite_count;
    uint8_t sprite_shifter_pattern_lo[8];
    uint8_t sprite_shifter_pattern_hi[8];

    // Sprite Zero Collision Flags
    bool bSpriteZeroHitPossible = false;
    bool bSpriteZeroHitBeingRendered = false;

public: 
    uint8_t* pOAM = (uint8_t*)OAM;

    uint8_t oam_addr = 0x00; // Stores address when CPU communicates with OAM via PPU registers
};