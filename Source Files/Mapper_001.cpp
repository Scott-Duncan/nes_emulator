#include "Mapper_001.h"


Mapper_001::Mapper_001(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
    vRAMStatic.resize(32 * 1024);
}

Mapper_001::~Mapper_001()
{

}


bool Mapper_001::cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data)
{
    if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        mapped_addr = 0xFFFFFFFF;

        data = vRAMStatic[addr & 0x1FFFF];

        return true;
    }

    if (addr >= 0x8000)
    {
        if (nControlRegister & 0b01000)
        {
            // 16k mode
            if (addr >= 0x8000 && addr <= 0xBFFF)
            {
                mapped_addr = nPRGBankSelect16Lo * 0x4000 + (addr & 0x3FFF);
                return true;
            }

            if (addr >= 0xC000 && addr <= 0xFFFF)
            {
                mapped_addr = nPRGBankSelect16Hi * 0x4000 + (addr & 0x3FFF);
                return true;
            }
        }
        else 
        {
            mapped_addr = nPRGBankSelect32 * 0x8000 + (addr & 0x7FFF);
        }
    }

    return false;
}

bool Mapper_001::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data)
{
    if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        mapped_addr = 0xFFFFFFFF;

        vRAMStatic[addr & 0x1FFFF] = data; 

        return true;
    }

    if (addr >= 0x8000)
    {
        if (data & 0x80)
        {
            nLoadRegister = 0x00;
            nLoadRegisterCount = 0;
            nControlRegister = nControlRegister | 0x0C;
        }
        else 
        {
            nLoadRegister >>= 1;
            nLoadRegister |= (data & 0x01) << 4;
            nLoadRegisterCount++;

            if (nLoadRegisterCount == 5)
            {
                uint8_t nTargetRegister = (addr >> 13) & 0x03;

                if (nTargetRegister == 0)
                {
                    nControlRegister = nLoadRegister & 0x1F;

                    switch(nControlRegister & 0x03)

                    switch (nControlRegister & 0x03)
                    {
                        case 0: mirrormode = ONESCREEN_LO; break;
                        case 1: mirrormode = ONESCREEN_HI; break;
                        case 2: mirrormode = VERTICAL;     break;
                        case 3: mirrormode = HORIZONTAL;   break;

                    }
                }
                else if (nTargetRegister == 1)
                {
                    if (nControlRegister & 0b10000)
                    {
                        nCHRBankSelect4Lo = nLoadRegister & 0x1F;
                    }
                    else
                    {
                        nCHRBankSelect8 = nLoadRegister & 0x1E;
                    }
                }
                else if (nTargetRegister == 2)
                {
                    if (nControlRegister & 0b10000)
                    {
                        nCHRBankSelec4HI = nLoadRegister & 0x1F;
                    }
                }
                else if (nTargetRegister == 3)
                {
                    uint8_t nPRGMode = (nControlRegister >> 2) & 0x03;

                    if (nPRGMode == 0 || nPRGMode == 1)
                    {
                        nPRGBankSelect32 = (nLoadRegister & 0x0E) >> 1;
                    }
                    else if (nPRGMode == 2)
                    {
                        nPRGBankSelect16Lo = 0;

                        nPRGBankSelect16Hi = nLoadRegister & 0x0F;
                    }
                    else if (nPRGMode == 3)
                    {
                        nPRGBankSelect16Lo = nLoadRegister & 0x0F;

                        nPRGBankSelect16Hi = nPRGBanks -1;
                    }
                }

                nLoadRegister = 0x00;
                nLoadRegisterCount = 0;
            }
        }
        
    }
    return false;
}

