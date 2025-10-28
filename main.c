#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "ChipCPU.h"
#include "renderer.h"


bool load_rom(ChipCPU *cpu, const char *filename) {
    FILE *rom = fopen(filename, "rb");
    if (!rom) {
        printf("Error: Could not open ROM file: %s\n", filename);
        return false;
    }

    // Get file size
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    // Check if ROM fits in memory
    if (rom_size > (MEMORY_SIZE - PROGRAM_OFFSET)) {
        printf("Error: ROM too large to fit in memory\n");
        fclose(rom);
        return false;
    }

    // Load ROM into memory starting at PROGRAM_OFFSET (0x200)
    size_t bytes_read = fread(&cpu->memory[PROGRAM_OFFSET], 1, rom_size, rom);

    if (bytes_read != rom_size) {
        printf("Error: Failed to read entire ROM\n");
        fclose(rom);
        return false;
    }

    printf("Loaded %ld bytes into memory\n", rom_size);
    fclose(rom);
    return true;
}

// Fetch the next opcode from memory
uint16_t fetch(ChipCPU *cpu) {
    // CHIP-8 opcodes are 2 bytes, stored big-endian
    uint16_t opcode = (cpu->memory[cpu->PC] << 8) | cpu->memory[cpu->PC + 1];
    return opcode;
}

// Update timers (should be called at 60Hz)
void update_timers(ChipCPU *cpu) {
    if (cpu->delayTimer > 0) {
        cpu->delayTimer--;
    }

    if (cpu->soundTimer > 0) {
        if (cpu->soundTimer == 1) {
            printf("BEEP!\n");  // TODO: Implement actual sound
        }
        cpu->soundTimer--;
    }
}

/**
 * Handle SDL keyboard input and map to CHIP-8 keys
 *
 * CHIP-8 Keypad:        Keyboard Mapping:
 * 1 2 3 C               1 2 3 4
 * 4 5 6 D               Q W E R
 * 7 8 9 E               A S D F
 * A 0 B F               Z X C V
 */
void handle_input(ChipCPU *cpu, SDL_Event *event)
{
    // Map of SDL keys to CHIP-8 key indices
    int key = -1;

    if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
        switch (event->key.keysym.sym) {
            case SDLK_1: key = 0x1; break;
            case SDLK_2: key = 0x2; break;
            case SDLK_3: key = 0x3; break;
            case SDLK_4: key = 0xC; break;

            case SDLK_q: key = 0x4; break;
            case SDLK_w: key = 0x5; break;
            case SDLK_e: key = 0x6; break;
            case SDLK_r: key = 0xD; break;

            case SDLK_a: key = 0x7; break;
            case SDLK_s: key = 0x8; break;
            case SDLK_d: key = 0x9; break;
            case SDLK_f: key = 0xE; break;

            case SDLK_z: key = 0xA; break;
            case SDLK_x: key = 0x0; break;
            case SDLK_c: key = 0xB; break;
            case SDLK_v: key = 0xF; break;
        }

        // Update key state
        if (key != -1) {
            if (event->type == SDL_KEYDOWN) {
                cpu->keys[key] = 1;
            } else {
                cpu->keys[key] = 0;
            }
        }
    }
}

void runEmulation(ChipCPU *cpu) {
    // CPU cycle timing
    const int CYCLES_PER_SECOND = 700;  // CHIP-8 typically runs at 500-700Hz
    const int TIMER_HZ = 60;            // Timers update at 60Hz
    rndr_startupBeep();

    rndr_initialize_graphics();

    int cycles = 0;
    SDL_bool loop = SDL_TRUE;
    SDL_Event event;

    while (loop) {
        // Fetch
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                loop = SDL_FALSE;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    loop = SDL_FALSE;
                }
            }
            handle_input(cpu, &event);
        }

        uint16_t opcode = fetch(cpu);

        // Increment PC before decode (most instructions will use this)
        cpu->PC += 2;

        // Decode & Execute
        decodeOperation(opcode, cpu);

        // Update timers at 60Hz (every ~12 cycles if running at 700Hz)
        cycles++;
        if (cycles >= (CYCLES_PER_SECOND / TIMER_HZ)) {
            update_timers(cpu);
            cycles = 0;
        }

        rndr_update_screen(cpu);
        rndr_play_audio(cpu);
        // Simple delay to control emulation speed
        SDL_Delay(1);
    }
    rndr_destroy();

}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Missing Argument: ./<rom_file>\n");
        return 1;
    }

    ChipCPU cpu;
    cpuInit(&cpu);

    // Load ROM
    if (!load_rom(&cpu, argv[1])) {
        return 1;
    }
    runEmulation(&cpu);
    return 0;
}