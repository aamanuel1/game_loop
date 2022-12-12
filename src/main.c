#include <stdio.h>
#include <SDL2/SDL.h>
#include "./constants.h"

//That's a lot of globals.
int game_is_running = FALSE;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int last_frame_time = 0;

//Struct of the game ball.
struct ball{
    float x;
    float y;
    float width;
    float height;
} ball;

int initialize_window(void){
    if(SDL_Init(SDL_INIT_EVERYTHING)){
        fprintf(stderr, "Error initializing SDL.\n");
        return FALSE;
    }
    //Centre the window
    window = SDL_CreateWindow(
        NULL, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        WINDOW_WIDTH, 
        WINDOW_HEIGHT, 
        SDL_WINDOW_BORDERLESS
        );
        if(!window){
            fprintf(stderr, "Error creating SDL Window.\n");
            return FALSE;
        }
        //Render the window by attaching the window to renderer with default stuff.
        renderer = SDL_CreateRenderer(window, -1, 0);
        if(!renderer){
            fprintf(stderr, "Error creating SDL Renderer.\n");
            return FALSE;
        }

    return TRUE;
}

void process_input(){
    //Poll the event with reference to raw struct
    SDL_Event event;
    SDL_PollEvent(&event);
    
    //What type of event is it?
    switch(event.type){
        case SDL_QUIT:  //Click x button.
            game_is_running = FALSE;
            break;
        case SDL_KEYDOWN:   //press esc key.
            if(event.key.keysym.sym == SDLK_ESCAPE)
            game_is_running = FALSE;
            break;
        
    }

}

void setup(){
    ball.x = 20;
    ball.y = 20;
    ball.width = 15;
    ball.height = 15;
}

void update(){
    //fix the timestep to be 30fps.
    //wastes time until the frame target time
    while(!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME)); // Empty while to lock execution.

    //get delta time factor converted to seconds for update
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;  //float division

    //how many frames since init.
    last_frame_time = SDL_GetTicks();

    ball.x += 70 * delta_time;
    ball.y += 50 * delta_time;
}

void render(){
    //RGB opaque
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //Draw rectangle
    SDL_Rect ball_rect = {
        //cast to int.
        (int)ball.x, 
        (int)ball.y, 
        (int)ball.width, 
        (int)ball.height};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    //start drawing game objects.
    //do a buffer swap.
    SDL_RenderPresent(renderer);

}

void destroy_window(){
    //Destroy in reverse order of the init.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(){
    //Setup
    int game_is_running = initialize_window();

    setup();

    //loop forever
    while(game_is_running){
        //The holy trinity of the game loop.
        process_input();
        update();
        render();
    }
    return 0;

    //This window will self destruct.
    destroy_window();
}