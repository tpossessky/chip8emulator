//
// Created by Tristan Possessky on 10/24/25.
//
#include <stdint.h>

#ifndef CHIP8_CHIPCPU_H
#define CHIP8_CHIPCPU_H

#define MEMORY_SIZE 4096
#define V_REGISTER_COUNT 16
#define STACK_DEPTH 16
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define DISPLAY_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT)
#define DISPLAY_SCALE 10
#define KEY_COUNT 16
#define FONT_OFFSET 0x50
#define PROGRAM_OFFSET 0x200


#define FONT_ARRAY_SIZE 80

typedef struct ChipCPU {
    uint8_t memory[MEMORY_SIZE];
    uint16_t PC;
    uint16_t I;
    uint8_t V[V_REGISTER_COUNT];
    uint16_t stack[STACK_DEPTH];
    uint8_t stackPointer;
    uint8_t soundTimer;
    uint8_t delayTimer;
    uint8_t display[DISPLAY_SIZE];
    uint8_t keys[KEY_COUNT];
    uint8_t drawFlag;  // Set to 1 when display should be redrawn
} ChipCPU;

void cpuInit(ChipCPU* cpu);
void decodeOperation(uint16_t opcode, ChipCPU* cpu);
void load_font(ChipCPU* cpu);

#endif //CHIP8_CHIPCPU_H
