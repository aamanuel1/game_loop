#include <stdio.h>
#include <SDL2/SDL.h>
#include "./constants.h"

//That's a lot of globals.
int game_is_running = FALSE;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int last_frame_time = 0;

//Struct of the game ball.
//generalized struct for both paddles and ball
struct game_object{
    float x;
    float y;
    float width;
    float height;
    float velocity_x;
    float velocity_y;
} ball, paddle;

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
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE){
                game_is_running = FALSE;
                break;
            }
            // Set paddle velocity based on left/right arrow keys
            if(event.key.keysym.sym == SDLK_LEFT){
                paddle.velocity_x = -400;
            }
            if(event.key.keysym.sym == SDLK_RIGHT){
                paddle.velocity_x = 400;
            }
            //Forgot to stop the switch from falling through and keeping the program from not moving.
            break;
        case SDL_KEYUP:
            if(event.key.keysym.sym == SDLK_LEFT){
                paddle.velocity_x = 0;
            }
            if(event.key.keysym.sym == SDLK_RIGHT){
                paddle.velocity_x = 0;
            }
            break;
    }
}

void setup(){
    ball.x = 20;
    ball.y = 20;
    ball.width = 15;
    ball.height = 15;
    //initialize velocity x and y
    ball.velocity_x = 300;
    ball.velocity_y = 300;
    //initialize paddle position at the bottom of the screen.
    paddle.width = 100;
    paddle.height = 20;
    paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
    paddle.y = WINDOW_HEIGHT - 40;
    paddle.velocity_x = 0;
    paddle.velocity_y = 0;
}

void update(){
    //fix the timestep to be 30fps.
    //wastes time until the frame target time
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time); // Empty while to lock execution.

    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
        SDL_Delay(time_to_wait);
    }
    //get delta time factor converted to seconds for update
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;  //float division

    //how many frames since init.
    last_frame_time = SDL_GetTicks();

    ball.x += ball.velocity_x * delta_time;
    ball.y += ball.velocity_y * delta_time;
    
    // Update paddle position based on its velocity
    // Use delta time and the ball and paddle velocity to get the x and y pos.
    paddle.x += paddle.velocity_x * delta_time;
    paddle.y += paddle.velocity_y * delta_time;

    // Check for ball collision with the walls
    // Check if x is less than 0 or greater than the window width. if it is then reverse direction
    if(ball.x <= 0 || ball.x + ball.width >= WINDOW_WIDTH){
        ball.velocity_x = -ball.velocity_x;
    }
    // Check if y is less than 0 (the top). If it is then reverse direction
    if(ball.y < 0){
        ball.velocity_y = -ball.velocity_y;
    }

    // Check for ball collision with the paddle
    // Check if ball y and ball height is greater than or equal to paddle y
    // Check if ball x is to the left of the paddle, or right of the paddle
    if((ball.y + ball.height >= paddle.y) && (ball.x <= paddle.x + paddle.width) && (ball.x + ball.width >= paddle.x)){
        ball.velocity_y = -ball.velocity_y;
    }

    // Prevent paddle from moving outside the boundaries of the window
    // If the paddle x is 0 (left side) or window width (right side). then velocity is 0
    if(paddle.x <= 0){
        paddle.x = 0;
    }
    // Forgot to bound to the right side.
    if(paddle.x >= WINDOW_WIDTH - paddle.width){
        paddle.x = WINDOW_WIDTH - paddle.width;
    }

    // Check for game over when ball hits the bottom part of the screen
    // If the ball y and ball height (top of ball) is greater than window height then send the ball back
    // to the middle top.
    if(ball.y + ball.height > WINDOW_HEIGHT){
        ball.x = WINDOW_WIDTH / 2;
        ball.y = 0;
    }
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

    // Draw a rectangle for the paddle object
    SDL_Rect paddle_rect = {
        (int)paddle.x,
        (int)paddle.y,
        (int)paddle.width,
        (int)paddle.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle_rect);

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

    destroy_window();

    return 0;


}