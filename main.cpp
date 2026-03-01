#include <cstdint>
#include <cstdio>
#include <vector>

#include <SDL3/SDL.h>
#include "chip8.h"

static constexpr int SCREEN_WIDTH = 64;
static constexpr int SCREEN_HEIGHT = 32;
static constexpr int SCALE = 10;

void keyboardUp(chip8 &myChip8, SDL_Keycode key)
{
    if (key == '1') myChip8.key[0x1] = 0;
    else if (key == '2') myChip8.key[0x2] = 0;
    else if (key == '3') myChip8.key[0x3] = 0;
    else if (key == '4') myChip8.key[0xC] = 0;
    else if (key == 'q') myChip8.key[0x4] = 0;
    else if (key == 'w') myChip8.key[0x5] = 0;
    else if (key == 'e') myChip8.key[0x6] = 0;
    else if (key == 'r') myChip8.key[0xD] = 0;
    else if (key == 'a') myChip8.key[0x7] = 0;
    else if (key == 's') myChip8.key[0x8] = 0;
    else if (key == 'd') myChip8.key[0x9] = 0;
    else if (key == 'f') myChip8.key[0xE] = 0;
    else if (key == 'z') myChip8.key[0xA] = 0;
    else if (key == 'x') myChip8.key[0x0] = 0;
    else if (key == 'c') myChip8.key[0xB] = 0;
    else if (key == 'v') myChip8.key[0xF] = 0;
}

void keyboardDown(chip8 &myChip8, SDL_Keycode key)
{
    // Keyboard mapping:
    // 1 2 3 4
    // Q W E R
    // A S D F
    // Z X C V
    //
    // CHIP-8 keypad:
    // 1 2 3 C
    // 4 5 6 D
    // 7 8 9 E
    // A 0 B F

    if (key == 27) // esc
        exit(0);

    if (key == '1')
        myChip8.key[0x1] = 1;
    else if (key == '2')
        myChip8.key[0x2] = 1;
    else if (key == '3')
        myChip8.key[0x3] = 1;
    else if (key == '4')
        myChip8.key[0xC] = 1;

    else if (key == 'q')
        myChip8.key[0x4] = 1;
    else if (key == 'w')
        myChip8.key[0x5] = 1;
    else if (key == 'e')
        myChip8.key[0x6] = 1;
    else if (key == 'r')
        myChip8.key[0xD] = 1;

    else if (key == 'a')
        myChip8.key[0x7] = 1;
    else if (key == 's')
        myChip8.key[0x8] = 1;
    else if (key == 'd')
        myChip8.key[0x9] = 1;
    else if (key == 'f')
        myChip8.key[0xE] = 1;

    else if (key == 'z')
        myChip8.key[0xA] = 1;
    else if (key == 'x')
        myChip8.key[0x0] = 1;
    else if (key == 'c')
        myChip8.key[0xB] = 1;
    else if (key == 'v')
        myChip8.key[0xF] = 1;

    // printf("Press key %c\n", key);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::printf("Usage: chip8_sdl3 <chip8_rom>\n");
        return 1;
    }

    chip8 c8;
    if (!c8.loadApplication(argv[1]))
        return 1;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::printf("SDL_Init failed: %s\n", SDL_GetError());
        std::getchar();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "CHIP-8 (SDL3)",
        SCREEN_WIDTH * SCALE,
        SCREEN_HEIGHT * SCALE,
        SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        std::printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Nearest-neighbor scaling so pixels stay crisp.
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // Create a 64x32 streaming texture we can update each frame.
    SDL_Texture *tex = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);
    if (!tex)
    {
        std::printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Make scaling crisp (no blur).
    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);

    std::vector<std::uint32_t> pixels(SCREEN_WIDTH * SCREEN_HEIGHT, 0xFF000000u);

    bool running = true;

    // Timing: render at ~60Hz, run multiple CPU cycles per frame.
    const std::uint64_t perfFreq = SDL_GetPerformanceFrequency();
    std::uint64_t lastCounter = SDL_GetPerformanceCounter();
    double accumulator = 0.0;
    const double frameStep = 1.0 / 60.0;

    // Adjust this based on how fast/accurate your core feels.
    // Many emulators run ~500-1000 cycles/sec; 60fps => ~8-16 cycles/frame.
    const int cyclesPerFrame = 10;

    while (running)
    {
        // --- Events ---
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                if (e.key.key == SDLK_ESCAPE)
                    running = false;
                keyboardDown(c8, e.key.key);
                break;

            case SDL_EVENT_KEY_UP:
                keyboardUp(c8, e.key.key);
                break;

            default:
                break;
            }
        }

        // --- Fixed-ish step at ~60Hz ---
        std::uint64_t now = SDL_GetPerformanceCounter();
        double dt = static_cast<double>(now - lastCounter) / static_cast<double>(perfFreq);
        lastCounter = now;
        accumulator += dt;

        while (accumulator >= frameStep)
        {
            for (int i = 0; i < cyclesPerFrame; ++i)
                c8.emulateCycle();

            // Timers tick at 60Hz, once per frame — not once per cycle
            if (c8.delay_timer > 0) --c8.delay_timer;
            if (c8.sound_timer > 0) --c8.sound_timer;

            accumulator -= frameStep;
        }

        // --- Render only when the core says the framebuffer changed ---
        if (c8.drawFlag)
        {
            // Convert gfx[] (0/1) into RGBA pixels.
            for (int y = 0; y < SCREEN_HEIGHT; ++y)
            {
                for (int x = 0; x < SCREEN_WIDTH; ++x)
                {
                    const int idx = y * SCREEN_WIDTH + x;
                    pixels[idx] = (c8.gfx[idx] ? 0xFFFFFFFFu : 0xFF000000u);
                }
            }

            if (!SDL_UpdateTexture(tex, nullptr, pixels.data(), SCREEN_WIDTH * (int)sizeof(std::uint32_t)))
            {
                std::printf("SDL_UpdateTexture failed: %s\n", SDL_GetError());
            }
        }

        // Draw scaled to window size
        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        SDL_FRect dst;
        dst.x = 0.0f;
        dst.y = 0.0f;
        dst.w = (float)w;
        dst.h = (float)h;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, tex, nullptr, &dst);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
