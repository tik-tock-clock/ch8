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

int loadRom(const char* romPath)
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
    size_t data = fread(romBuffer, sizeof(char), (size_t)romSize, rom);
    if (data != romSize){
        printf("ERROR: Failed to read ROM!\n");
        return 0;
    }
}

int main(int argc, const char* argv[])
{
    return 0;
}