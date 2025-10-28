#include <printf.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include "ChipCPU.h"
#include <SDL_audio.h>

SDL_Window *_window;
SDL_Renderer *_renderer;

Mix_Chunk *_tone = NULL;


const int AMPLITUDE = 20000;
const int FREQUENCY = 44100;
Sint16 *toneSamples;

void generateTone() {
    const int duration = 100; // ms
    const int sample_count = FREQUENCY * duration / 1000;

    toneSamples = malloc(sizeof(Sint16) * sample_count);
    if (!toneSamples) return;

    for (int i = 0; i < sample_count; i++) {
        double t = (double)i / FREQUENCY;
        toneSamples[i] = (sin(2.0 * M_PI * 440.0 * t) >= 0) ? AMPLITUDE : -AMPLITUDE;
    }

    _tone = Mix_QuickLoad_RAW((Uint8 *)toneSamples, sample_count * sizeof(Sint16));
    // do NOT free toneSamples yet; SDL_mixer uses it
}
void rndr_startupBeep() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! %s\n", SDL_GetError());
        return;
    }

    if (Mix_OpenAudio(FREQUENCY, MIX_DEFAULT_FORMAT, 1, 4096) != 0) { // mono
        printf("SDL_mixer could not initialize! %s\n", Mix_GetError());
        return;
    }

    generateTone();

    if (!_tone) {
        printf("Tone not generated\n");
        return;
    }

    // Play the tone
    Mix_PlayChannel(0, _tone, 0);

    // Wait long enough for it to play
    SDL_Delay(400);

    // Clean up
    Mix_HaltChannel(0);
    Mix_CloseAudio();
    SDL_Quit();
    if (toneSamples) free(toneSamples);
}
/**
 * Initialise SDL2 and output some useful display info
 */
void initSDL()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("[Error] SDL Init : %s \n", SDL_GetError());
    } else {
        printf("SDL INITIALISED\n");
        SDL_DisplayMode dm;
        SDL_GetCurrentDisplayMode(0, &dm);
        SDL_SetWindowBordered(_window, SDL_FALSE);
        printf("Display mode is %dx%dpx @ %dhz\n", dm.w, dm.h, dm.refresh_rate);
    }
    if (Mix_OpenAudio(FREQUENCY, MIX_DEFAULT_FORMAT, 1, 4096) != 0) {
        printf("[Error] Error Initialising Audio : %s\n", SDL_GetError());
    } else {
        generateTone();
        printf("Audio Initialised\n");
    }
}

/**
 * Initialise an SDL Window and Renderer
 *
 * This uses SDL_CreateWindowAndRenderer. They can alternatively be created separately. See SDL2 Docs
 */
void init_window_and_renderer()
{
    _window = SDL_CreateWindow("",0,0,DISPLAY_WIDTH * DISPLAY_SCALE,DISPLAY_HEIGHT * DISPLAY_SCALE, SDL_WINDOW_SHOWN);
    _renderer = SDL_CreateRenderer(_window,-1, SDL_RENDERER_ACCELERATED);
}



/**
 * Play a sample audio file
 */
void rndr_play_audio(ChipCPU *cpu)
{
    if (cpu->soundTimer > 0 && _tone != NULL) {
        if (!Mix_Playing(-1)) { // -1 = first available channel
            Mix_PlayChannel(-1, _tone, 0);
        }
    }
}



/**
 * Update the screen based on Chip8 display buffer
 *
 * @param cpu Pointer to the ChipCPU struct
 * @param scale Scale factor for pixels (e.g., 10 means each chip8 pixel = 10x10 SDL pixels)
 */
void rndr_update_screen(ChipCPU *cpu)
{
    // Only update if drawFlag is set
    if (!cpu->drawFlag) {
        return;
    }

    // Clear the screen with black
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);

    // Set draw color to white for "on" pixels
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);

    // Chip8 display is 64x32 pixels
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            // Check if pixel is on
            if (cpu->display[y * 64 + x]) {
                SDL_Rect rect = {
                        x * DISPLAY_SCALE,      // x position
                        y * DISPLAY_SCALE,      // y position
                        DISPLAY_SCALE,          // width
                        DISPLAY_SCALE           // height
                };
                SDL_RenderFillRect(_renderer, &rect);
            }
        }
    }

    // Present the rendered frame
    SDL_RenderPresent(_renderer);

    // Reset the draw flag
    cpu->drawFlag = 0;
}

void rndr_destroy()
{

    if (toneSamples)
        free(toneSamples);

    Mix_CloseAudio();

    // Window
    SDL_DestroyWindow(_window);
    // SDL
    SDL_Quit();
    exit(0);
}

void rndr_initialize_graphics(){
    initSDL();
    init_window_and_renderer();
}