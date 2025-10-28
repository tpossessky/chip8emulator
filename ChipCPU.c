#include <memory.h>
#include <printf.h>
#include <stdlib.h>
#include <time.h>
#include "ChipCPU.h"
//
// Created by Tristan Possessky on 10/24/25.
//

uint8_t font_sprites[FONT_ARRAY_SIZE] = {
        0xF0,0x90,0x90,0x90,0xF0, // 0
        0x20,0x60,0x20,0x20,0x70, // 1
        0xF0,0x10,0xF0,0x80,0xF0, // 2
        0xF0,0x10,0xF0,0x10,0xF0, // 3
        0x90,0x90,0xF0,0x10,0x10, // 4
        0xF0,0x80,0xF0,0x10,0xF0, // 5
        0xF0,0x80,0xF0,0x90,0xF0, // 6
        0xF0,0x10,0x20,0x40,0x40, // 7
        0xF0,0x90,0xF0,0x90,0xF0, // 8
        0xF0,0x90,0xF0,0x10,0xF0, // 9
        0xF0,0x90,0xF0,0x90,0x90, // A
        0xE0,0x90,0xE0,0x90,0xE0, // B
        0xF0,0x80,0x80,0x80,0xF0, // C
        0xE0,0x90,0x90,0x90,0xE0, // D
        0xF0,0x80,0xF0,0x80,0xF0, // E
        0xF0,0x80,0xF0,0x80,0x80  // F
};


void skipInstruction(ChipCPU* cpu){
    cpu->PC += 2;
}
void repeatInstruction(ChipCPU* cpu){
    cpu->PC -= 2;
}

 void decodeOperation(uint16_t opcode, ChipCPU* cpu){
     uint8_t indexX;
     uint8_t indexY;
     uint16_t val;
     //2 byte opcode, take first 4 bits to find operation
     switch (opcode & 0xF000) {
         case 0x0000:
             switch (opcode & 0x00FF) {
                 case 0x00E0:
                     cpu->drawFlag = 1;
                     printf("Clear Screen\n");
                     break;
                 case 0x00EE:
                     //Return from Subroutine
                     cpu->stackPointer--;
                     cpu->PC = cpu->stack[cpu->stackPointer];
                     break;
                 default:
                     printf("Invalid opcode!");
                     break;
             }
             break;
         case 0x1000:
             //1NNN Jump to address NNN
             cpu->PC = opcode & 0x0FFF;
             break;
         case 0x2000:
             // 2NNN: Call subroutine at NNN
             cpu->stack[cpu->stackPointer] = cpu->PC;
             cpu->stackPointer++;
             cpu->PC = opcode & 0x0FFF;
             break;
         case 0x3000:
             //3XNN Skip the following instruction if the value of register VX equals NN
             val = (opcode & 0x00FF);
             indexX = (opcode & 0x0F00) >> 8;
             if(cpu->V[indexX] == val){
                 skipInstruction(cpu);
             }
             break;
         case 0x4000:
             //4XNN Skip the following instruction if the value of register VX is not equal to NN
             val = (opcode & 0x00FF);
             indexX = (opcode & 0x0F00) >> 8;
             if(cpu->V[indexX] != val){
                 skipInstruction(cpu);
             }
             break;
         case 0x5000:
             //5XY0 Skip the following instruction if the value of register VX = VY
             indexX = (opcode & 0x0F00) >> 8;
             indexY = (opcode & 0x00F0) >> 4;
             if(cpu->V[indexX] == cpu->V[indexY]){
                 skipInstruction(cpu);
             }
             break;
         case 0x6000:
             //6XNN Store number NN in register VX
             val = (opcode & 0x00FF);
             indexX = (opcode & 0x0F00) >> 8;
             cpu->V[indexX] = val;
             break;
         case 0x7000:
             //7XNN Add the value NN to register VX
             val = (opcode & 0x00FF);
             indexX = (opcode & 0x0F00) >> 8;
             cpu->V[indexX] = cpu->V[indexX] + val;
             break;
         case 0x8000:
             indexX = (opcode & 0x0F00) >> 8;
             indexY = (opcode & 0x00F0) >> 4;
             switch (opcode & 0x000F) {
                 //8XY0 Store the value of register VY in register VX
                 case 0x0: cpu->V[indexX] = cpu->V[indexY]; break;
                     //8XY1	Set VX to VX OR VY
                 case 0x1: cpu->V[indexX] |= cpu->V[indexY]; break;
                     //8XY2 Set VX to VX AND VY
                 case 0x2: cpu->V[indexX] &= cpu->V[indexY]; break;
                     //8XY3 Set VX to VX XOR VY
                 case 0x3: cpu->V[indexX] ^= cpu->V[indexY]; break;
                     //8XY4 Add the value of register VY to register VX
                     //         Set VF to 01 if a carry occurs
                     //         Set VF to 00 if a carry does not occur
                 case 0x4: {
                     uint16_t sum = cpu->V[indexX] + cpu->V[indexY];
                     cpu->V[0xF] = (sum > 255);
                     cpu->V[indexX] = sum & 0xFF;
                     break;
                 }
                     //8XY5 Subtract the value of register VY from register VX
                     //         Set VF to 00 if a borrow occurs
                     //         Set VF to 01 if a borrow does not occur
                 case 0x5: {
                     cpu->V[0xF] = (cpu->V[indexX] >= cpu->V[indexY]) ? 1 : 0;
                     cpu->V[indexX] = (cpu->V[indexX] - cpu->V[indexY]) & 0xFF;
                     break;
                 }
                     //8XY6 Store the value of register VY shifted right one bit in register VX
                     //         Set register VF to the least significant bit prior to the shift
                     //         VY is unchanged
                 case 0x6:
                     cpu->V[0xF] = cpu->V[indexX] & 0x1;
                     cpu->V[indexX] >>= 1;
                     break;
                     //8XY7 Set register VX to the value of VY minus VX
                     //         Set VF to 00 if a borrow occurs
                     //         Set VF to 01 if a borrow does not occur
                 case 0x7: {
                     indexX = (opcode & 0x0F00) >> 8;
                     indexY = (opcode & 0x00F0) >> 4;

                     cpu->V[0xF] = (cpu->V[indexY] >= cpu->V[indexX]) ? 1 : 0;
                     cpu->V[indexX] = cpu->V[indexY] - cpu->V[indexX];
                     break;
                 }
                     //8XYE Store the value of register VY shifted left one bit in register VX
                     //         Set register VF to the most significant bit prior to the shift
                     //         VY is unchanged
                 case 0xE:
                     cpu->V[0xF] = (cpu->V[indexX] & 0x80) >> 7;
                     cpu->V[indexX] <<= 1;
                     break;
             }
             break;
         case 0x9000:
             // 9XY0 Skip next instruction if VX != VY
             indexX = (opcode & 0x0F00) >> 8;
             indexY = (opcode & 0x00F0) >> 4;
             if (cpu->V[indexX] != cpu->V[indexY]) {
                 skipInstruction(cpu);
             }
             break;
         case 0xA000:
             //ANNN Store memory address NNN in register I
             cpu->I = (opcode & 0x0FFF);
             break;
         case 0xB000:
             //BNNN	Jump to address NNN + V0
             cpu->PC = cpu->V[0] + (opcode & 0x0FFF);
             break;
         case 0xC000:
             // CXNN: Set VX = random byte AND NN
             indexX = (opcode & 0x0F00) >> 8;
             uint8_t mask  = opcode & 0x00FF;
             uint8_t randNum = rand() % 256;
             cpu->V[indexX] = randNum & mask;
             break;
         case 0xD000:
             cpu->drawFlag = 1;
             indexX = (opcode & 0x0F00) >> 8;
             indexY = (opcode & 0x00F0) >> 4;
             val = (opcode & 0x000F);  // N = height of sprite in pixels

             uint8_t x_pos = cpu->V[indexX] % DISPLAY_WIDTH;   // Wrap x position
             uint8_t y_pos = cpu->V[indexY] % DISPLAY_HEIGHT;  // Wrap y position

             cpu->V[0xF] = 0;  // Reset collision flag

             // Loop through each row of the sprite (N rows)
             for (int row = 0; row < val; row++) {
                 uint8_t sprite_byte = cpu->memory[cpu->I + row];

                 // Loop through each bit/pixel in this row (8 pixels per byte)
                 for (int col = 0; col < 8; col++) {
                     // Check if this bit is set in the sprite
                     if ((sprite_byte & (0x80 >> col)) != 0) {
                         // Calculate screen position
                         int screen_x = (x_pos + col) % DISPLAY_WIDTH;
                         int screen_y = (y_pos + row) % DISPLAY_HEIGHT;

                         // Calculate index in display array (assuming 1D array)
                         int pixel_index = screen_y * DISPLAY_WIDTH + screen_x;

                         // Check if pixel is already on (collision detection)
                         if (cpu->display[pixel_index] == 1) {
                             cpu->V[0xF] = 1;  // Set collision flag
                         }

                         // XOR the pixel
                         cpu->display[pixel_index] ^= 1;
                     }
                 }
             }
             break;
         case 0xE000:
             //EX9E Skip the following instruction if the key corresponding to the hex value
             //         currently stored in register VX is pressed
             //EXA1 Skip the following instruction if the key corresponding to the hex value
             //         currently stored in register VX is not pressed
             indexX = (opcode & 0x0F00) >> 8;
             switch (opcode & 0x00FF) {
                 case 0x9E:
                     if (cpu->keys[cpu->V[indexX]]) {
                         skipInstruction(cpu);
                     }
                     break;
                 case 0xA1:
                     if (!cpu->keys[cpu->V[indexX]]) {
                         skipInstruction(cpu);
                     }
                     break;
             }
             break;
         case 0xF000:
             indexX = (opcode & 0x0F00) >> 8;
             val = (opcode & 0x00FF);

             switch (val) {
                 //FX07 Store the current value of the delay timer in register VX
                 case 0x07:
                     cpu->V[indexX] = cpu->delayTimer;
                     break;
                 //FX0A	Wait for a keypress and store the result in register VX
                 case 0x0A:
                     // Wait for keypress — block until one is pressed
                     for (int i = 0; i < 16; i++) {
                         if (cpu->keys[i]) {
                             cpu->V[indexX] = i;
                             return;
                         }
                     }
                     repeatInstruction(cpu);
                     break;
                 //FX15	Set the delay timer to the value of register VX
                 case 0x15:
                     cpu->delayTimer = cpu->V[indexX];
                     break;
                 //FX1E Add the value stored in register VX to register I
                 case 0x1E:
                     cpu->I += cpu->V[indexX];
                     break;
                 //FX29 Set I to the memory address of the sprite data corresponding to the hexadecimal digit
                 //         stored in register VX
                 case 0x29:
                     cpu->I = cpu->V[indexX] * 5;

                     break;
                 //FX33 Store the binary-coded decimal equivalent of the value stored in register VX at
                 //         addresses I, I + 1, and I + 2
                 case 0x33:
                     cpu->memory[cpu->I]     = cpu->V[indexX] / 100;
                     cpu->memory[cpu->I + 1] = (cpu->V[indexX] / 10) % 10;
                     cpu->memory[cpu->I + 2] = cpu->V[indexX] % 10;
                     break;
                 //FX55 Store the values of registers V0 to VX inclusive in memory starting at address I
                 //         I = I + X + 1 after operation
                 case 0x55:
                     for (int i = 0; i <= indexX; i++)
                         cpu->memory[cpu->I + i] = cpu->V[i];
                     cpu->I += indexX + 1;
                     break;
                 //FX65 Fill registers V0 to VX inclusive with the values stored in memory starting at address I
                 //         I is set to I + X + 1 after operation
                 case 0x65:
                     for (int i = 0; i <= indexX; i++)
                         cpu->V[i] = cpu->memory[cpu->I + i];
                     cpu->I += indexX + 1;
                     break;
                 default:
                     printf("Unsupported opco");
                     break;
             }
             break;
     }
 }



void load_font(ChipCPU* cpu)
{
    printf("Loading Integrated Fonts...\n");

    for(int i = 0; i < FONT_ARRAY_SIZE; i++){
        cpu->memory[FONT_OFFSET + i] = font_sprites[i];
    }
    printf("Loaded Fonts");
}


void cpuInit(ChipCPU* cpu)
{
    memset(cpu, 0, sizeof(ChipCPU));
    cpu->PC = PROGRAM_OFFSET;
    printf("Initialize CPU\n");
    srand(time(NULL));

    load_font(cpu);
}