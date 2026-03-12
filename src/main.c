#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>
#include "src/cpu.h"

static Chip8* chip;
static SDL_Surface* chip8Buffer = NULL;
static SDL_Surface* rgbaBuffer = NULL;
static SDL_Renderer* renderer;
static SDL_Texture* texture = NULL;
 
uint32_t totalCycles;

const static uint8_t KEYMAP[16] = {
    SDLK_x, // 0
    SDLK_1, // 1
    SDLK_2, // 2
    SDLK_3, // 3
    SDLK_q, // 4
    SDLK_w, // 5
    SDLK_e, // 6
    SDLK_a, // 7
    SDLK_s, // 8
    SDLK_d, // 9
    SDLK_z, // A
    SDLK_c, // B
    SDLK_4, // C
    SDLK_r, // D
    SDLK_f, // E
    SDLK_v  // F
};

void drawChip8Display(SDL_Surface *surface, Chip8 *chip)
{
    SDL_LockSurface(surface);

    uint32_t *pixels = (uint32_t *)surface->pixels;
    for (int i=0; i<(64*32); i++){
        pixels[i] = chip->display[i] == 0 ? 0 : 0xFFFFFFFF;
    }
    SDL_UnlockSurface(surface);
}

void update()
{
    drawChip8Display(chip8Buffer, chip);
    SDL_BlitScaled(chip8Buffer, NULL, rgbaBuffer->pixels, rgbaBuffer->pitch);
    SDL_UpdateTexture(texture, NULL, rgbaBuffer->pixels, rgbaBuffer->pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void emulateCycle()
{
    chip8Cycle(chip);
    totalCycles++;

    if (totalCycles % 9 == 0){
        updateTimes(chip);
    }

    sleep(1/550);
}

int main(int argc, char* argv[])
{
    chip = (Chip8 *)malloc(sizeof(Chip8));
    initChip8(chip);

    loadRom(argv[0], chip);

    SDL_Window *window = NULL;
    SDL_Surface *screenSurface = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        fprintf(stderr, "ERROR: Couldn't initialise SDL2: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow(
        "Chip 8 emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 320,
        SDL_WINDOW_SHOWN
    );

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (window == NULL){
        fprintf(stderr, "ERROR: Couldn't create window: %s\n", SDL_GetError());
        return 1;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 640, 320);
    screenSurface = SDL_GetWindowSurface(window);

    chip8Buffer = SDL_CreateRGBSurface(0, 64, 32, 32, 0, 0, 0, 0);
    rgbaBuffer = SDL_CreateRGBSurface(0, 640, 320, 32, 0, 0, 0, 0);

    while(1)
    {
        SDL_Event event;

        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    printf("Exited successfully!\n");
                    break;

                case SDL_KEYDOWN:
                    for (int i=0; i<16; i++){
                        if (event.key.keysym.sym == KEYMAP[i]){
                            chip->keypad[i] = 1;
                        }
                    }

                case SDL_KEYUP:
                    for (int i=0; i<16; i++){
                        if (event.key.keysym.sym == KEYMAP[i]){
                            chip->keypad[i] = 0;
                        }
                    }
            }
        }

        emulateCycle();
        update();
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}