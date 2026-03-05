#include <SDL2/SDL.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error initializing SDL: %s\n", SDL_GetError());
    }

    SDL_Renderer* renderer;
    SDL_Window* window = SDL_CreateWindow("chip8 emu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, 0);
    SDL_RenderSetLogicalSize(renderer, 64, 32);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    int close = 0;

    while(!close)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:

                    close = 1;
                    printf("Exited successfully!\n");
                    break;

            }
        }
    }

    return 0;
}