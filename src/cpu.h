#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    uint8_t Vregisters[16];
    uint8_t memory[4096];
    uint16_t indexRegister;
    uint16_t programCounter;

    //Timers
    uint8_t delayTimer;
    uint8_t soundTimer;

    //The stack
    uint16_t stack[16];
    uint8_t stackPointer;

    //Graphics and Input
    uint8_t display[64*32];
    uint8_t keypad[16];
}Chip8;

void initChip8(Chip8* chip);
void draw(Chip8* chip, uint16_t opcode, uint8_t vx, uint8_t vy);
int loadRom(const char* romPath, Chip8 *chip);
void updateTimes(Chip8* chip);
void chip8Cycle(Chip8 *chip);