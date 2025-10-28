//
// Created by Tristan Possessky on 10/27/25.
//

#ifndef CHIP8_RENDERER_H
#define CHIP8_RENDERER_H

#include "ChipCPU.h"

void rndr_play_audio(ChipCPU *cpu);
void rndr_startupBeep();
void rndr_destroy();
void rndr_initialize_graphics();
void rndr_update_screen(const ChipCPU *cpu);

#endif //CHIP8_RENDERER_H
