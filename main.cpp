#include "game.h"

const Uint8* keyboard_state = NULL;

void derror (const char* err_name) {
    printf ("ERROR: %s failed: %s\n", err_name, SDL_GetError());
    exit(1);
}

SDL_Renderer* renderer = NULL;
TTF_Font* game_font = NULL;

std::vector<SDL_Texture*> game_images;

const char *image_files[] = {
    "map.png"
};

bool load_image_file_index (unsigned int index) {
    SDL_Surface* image = IMG_Load(image_files[index]);
    if (image == NULL) {
        return false;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
    if (texture == NULL) {
        SDL_FreeSurface(image);
        return false;
    }
    game_images.push_back(texture);
    return true;
}

int main () {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        derror("SDL_Init");
    }

    if (TTF_Init() != 0) {
        derror("TTF_Init");
    }

    printf ("Loading font 04b-03.ttf ...\n");
    game_font = TTF_OpenFont("04b-03.ttf", 24);

    printf ("Getting SDL keyboard states ...\n");
    keyboard_state = SDL_GetKeyboardState(NULL);

    World* world = new World({});

    // Window creation
    SDL_Window* window = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    printf ("Loading images ...\n");
    for (int i=0; i<SIZE(image_files); i++) {
        printf ("%s ...\t\t", image_files[i]);
        if (load_image_file_index(i)) printf("OK\n");
        else printf("FAIL (%s)\n", SDL_GetError());
    }

    Uint64 timeNow, timeLast = 0;
    double deltaTime = 0;

    timeNow = SDL_GetPerformanceCounter();
    DrawableText* dtxt = new DrawableText("Testing SDL text (TTF)", 0, 0, {255, 255, 255, 255});
    DrawableImage* gamemap = new DrawableImage(0, 0, 0);

    SDL_Point mouse_coords;
    SDL_Point click_point;
    int o_x = gamemap->x, o_y = gamemap->y; // original

    bool moving_map = false;

    bool close = false;
    SDL_Event event;
    while (!close) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            close = true;
        }
        else if (event.type == SDL_MOUSEMOTION) {
            mouse_coords.x = event.motion.x;
            mouse_coords.y = event.motion.y;
            if (moving_map) {
                gamemap->x = o_x + (mouse_coords.x - click_point.x);
                gamemap->y = o_y + (mouse_coords.y - click_point.y);
            }
        }
        world->checkMouseOver(&mouse_coords);
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (!world->checkMouseClick(&mouse_coords, event.button.button) && event.button.button == SDL_BUTTON_LEFT) {
                click_point = mouse_coords;
                moving_map = true;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT && moving_map) {
                moving_map = false;
                o_x = gamemap->x;
                o_y = gamemap->y;
            }
        }

        // delta
        timeLast = timeNow;
        timeNow = SDL_GetPerformanceCounter();
        deltaTime = (double)((timeNow - timeLast)*1000 / (double)SDL_GetPerformanceFrequency());

        SDL_RenderClear (renderer);
        //printf("Drawing world ...\n");
        world->drawWorld();
        //printf("Drawing text ...\n");
        gamemap->draw();
        dtxt->draw();
        //printf("Done!\n");
        SDL_RenderPresent (renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}