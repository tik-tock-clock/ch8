#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t Chip8FontSet[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

typedef struct
{
    uint8_t Vregisters[16];
    uint8_t memory[4096];
    uint16_t indexRegister;
    uint16_t programCounter;

    //Timers
    uint8_t delayTimer;

    //The stack
    uint16_t stack[16];
    uint8_t stackPointer;

    //Graphics and Input
    uint8_t display[64*32];
    uint8_t keypad[16];
}Chip8;

void init(Chip8 *chip)
{
    //Reset Program Counter to 0x200
    chip->programCounter = 0x200;

    //Reset Index and Stack Pointer
    chip->indexRegister = 0;
    chip->stackPointer = 0;

    //Clear Memory, Registers, and Stack
    memset(chip->memory, 0, sizeof(chip->memory));
    memset(chip->Vregisters, 0, sizeof(chip->Vregisters));
    memset(chip->stack, 0, sizeof(chip->stack));

    //Clear Display and Keypad
    memset(chip->display, 0, sizeof(chip->display));
    memset(chip->keypad, 0, sizeof(chip->keypad));

    //Reset Timer
    chip->delayTimer = 0;

    //Load Fontset to beginning of memory
    for (int i=0; i<80; i++) {
        chip->memory[0x50 + i] = Chip8FontSet[i];
    }
}

int loadRom(const char* romPath, Chip8 *chip)
{
    //Open the File and check if the File is opened successfully
    FILE* rom = fopen(romPath, "rb");
    if (rom == NULL){
        printf("File not opened successfully!\n");
        return 0;
    }

    printf("File opened successfully\n");

    //Get File size
    fseek(rom, 0, SEEK_END);
    long romSize = ftell(rom);
    rewind(rom);

    //Allocate memory  buffer to ROM
    //Sets size of buffer to size of a char times by the size of the ROM in chars
    //Validates buffer aswell
    char* romBuffer = (char*) malloc(sizeof(char) * romSize);
    if (romBuffer == NULL){
        printf("ERROR: Failed to allocate memory for ROM!\n");
        return 0;
    }

    //Copy the rom into the buffer
    size_t romData = fread(romBuffer, sizeof(char), (size_t)romSize, rom);
    if (romData != romSize){
        printf("ERROR: Failed to read ROM!\n");
        return 0;
    }

    //Copy buffer to Chip8 memory
    if ((4096-512) > romSize){
        memcpy(chip->memory[512], romData, romSize);
    }

    fclose(rom);
    free(romBuffer);
}

void chip8Cycle(Chip8 *chip) 
{
    //Fetch opcode from memory
    uint16_t opcode = chip->memory[chip->programCounter] << 8 | chip->memory[chip->programCounter+1];
    chip->programCounter += 2;

    //collect vx and vy from opcode
    uint16_t vx = (opcode & 0x0F00) >> 8;
    uint16_t vy = (opcode & 0x00F0) >> 4;

    switch (opcode & 0xF000)
    {
        case 0x0000:
            switch (opcode & 0x00FF)
            {
                case 0x00E0:
                    //Clear the screen
                    memset(chip->display, 0, sizeof(chip->display));
                    break;

                case 0x00EE:
                    //Return from suboutine
                    --chip->stackPointer;
                    chip->programCounter = chip->stack[chip->stackPointer];
                    break;
            }

        case 0x1000:
            //Jump to address NNN
            chip->programCounter = opcode & 0x0FFF;

        case 0x2000:
            //Execute subroutine at NNN
            chip->stackPointer++;
            chip->stack[chip->stackPointer] = chip->programCounter;
            chip->programCounter = opcode & 0xFFF;

        case 0x3000:
            //Skip next instruction if register Vx is equal to kk
            if (chip->Vregisters[vx] == (opcode & 0x00FF)){
                chip->programCounter += 2;
            }
            break;

        case 0x4000:
            //Skip next instruction if Vx is not equal to kk
            if (chip->Vregisters[vx] != (opcode & 0x00FF)){
                chip->programCounter += 2;
            }
            break;

        case 0x5000:
            //Skip next instruciton if Vx is equal to Vy
            if (chip->Vregisters[vx] == chip->Vregisters[vy]){
                chip->programCounter += 2;
            } 
            break;

        case 0x6000:
            //Set Vx to kk
            chip->Vregisters[vx] = (opcode & 0x00FF);
            break;

        case 0x7000:
            //Add kk to Vx
            chip->Vregisters[vx] += (opcode & 0x00FF);
            break;

        case 0x8000:
            switch (opcode * 0x000F)
            {
                case 0x0000:
                    //Set Vx to Vy
                    chip->Vregisters[vx] = chip->Vregisters[vy];
                    break;

                case 0x0001:
                    //Set Vx to Vx OR Vy as a bitwise operation
                    chip->Vregisters[vx] = chip->Vregisters[vx] | chip->Vregisters[vy];
                    break;

                case 0x0002:
                    //Set Vx to Vx AND Vy as a bitwise operation
                    chip->Vregisters[vx] = chip->Vregisters[vx] & chip->Vregisters[vy];
                    break;

                case 0x0003:
                    //Set Vx to Vx XOR vy as a bitwise operation
                    chip->Vregisters[vx] = chip->Vregisters[vx] ^ chip->Vregisters[vy];
                    break;

                case 0x0004:
                    //Vx and Vy are added together
                    //If result is greater than 8 bits VF is sent to 1, otherwise 0
                    //Only lowest 8 bits of result are kept in Vx
                    uint16_t result = chip->Vregisters[vx] + chip->Vregisters[vy];
                    uint8_t overflow = result > 0xFF ? 1 : 0;
                    chip->Vregisters[vx] = result & 0xFF;
                    chip->Vregisters[0xF] = overflow;
                    break;

                case 0x0005:
                    //If Vx is greater than Vy, VF is set to 1, otherwise 0
                    //Vy is subtracted from Vx, results stored in vx
                    uint16_t result = chip->Vregisters[vx] - chip->Vregisters[vy];
                    uint8_t overflow = chip->Vregisters[vx] >= chip->Vregisters[vy] ? 1 : 0;
                    chip->Vregisters[vx] = result & 0xFF;
                    chip->Vregisters[0xF] = overflow;
                    break;

                case 0x0006:
                    //Shift Vx to right by 1 bit 
                    //Store least significant bit in VF
                    uint8_t overflow = chip->Vregisters[vx] & 0x1;
                    chip->Vregisters[vx] >>= 1;
                    chip->Vregisters[0xF] = overflow;
                    break;

                case 0x0007:
                    //Vx is set to Vy - Vx
                    //If there is an overflow VF is set to 1, otherwise is set to 0
                    uint16_t result = chip->Vregisters[vy] - chip->Vregisters[vx];
                    uint8_t overflow = chip->Vregisters[vx] > 0xFF ? 1 : 0;
                    chip->Vregisters[vx] = result && 0xFF;
                    chip->Vregisters[0xF] = overflow;
                    break;

                case 0x000E:
                    //Shift Vx to left by 1
                    //Store most significant bit in VF 
                    uint8_t oveflow = chip->Vregisters[vx] >> 7;
                    chip->Vregisters[vx] <<= 1;
                    chip->Vregisters[0xF] = overflow;
                    break;
            }

        case 0x9000:
                //Skip next instruction if Vx isn't equal to Vy
                if (chip->Vregisters[vx] != chip->Vregisters[vy]){
                    chip->programCounter += 2;
                } 
                break;

        case 0xA000:
            //Sets Index register to NNN
            chip->indexRegister = (opcode & 0x0FFF);
            break;
                
        case 0xB000:
            //Jump to address NNN plus V0
            chip->programCounter = chip->Vregisters[0] + (opcode & 0x0FFF);
            break; 

        case 0xC000:
            //Set Vx to bitwise random number AND NN
            chip->Vregisters[vx] = rand() & (opcode & 0x00FF);
            break;

        case 0xD000:
            //Draw Sprite at coords; (Vx, Vy)
    }
}