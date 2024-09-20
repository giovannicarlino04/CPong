#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <windows.h>
#include <stdbool.h>
#include <stdio.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int PADDLE_WIDTH = 15;
const int PADDLE_HEIGHT = 100;
const int BALL_SIZE = 15;
const int PADDLE_SPEED = 10;
const int BALL_SPEED = 5;
const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS; // milliseconds per frame

typedef struct {
    int x, y, w, h;
} Paddle;

typedef struct {
    int x, y, vx, vy;
} Ball;

int PLAYER1_SCORE = 0;
int PLAYER2_SCORE = 0;

void reset_ball(Ball *ball) {
    ball->x = WINDOW_WIDTH / 2;
    ball->y = WINDOW_HEIGHT / 2;
    ball->vx = (rand() % 2 == 0 ? BALL_SPEED : -BALL_SPEED);
    ball->vy = (rand() % 2 == 0 ? BALL_SPEED : -BALL_SPEED);
}

void render_score(SDL_Renderer *renderer, TTF_Font *font) {
    char scoreText[20];
    sprintf(scoreText, "%d | %d", PLAYER1_SCORE, PLAYER2_SCORE);

    SDL_Surface *surface = TTF_RenderText_Solid(font, scoreText, (SDL_Color){255, 255, 255}); // White color
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    SDL_Rect scoreRect = { (WINDOW_WIDTH - width) / 2, 10, width, height }; // Centered at the top

    SDL_RenderCopy(renderer, texture, NULL, &scoreRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init(); // Initialize SDL_ttf
    SDL_Window *window = SDL_CreateWindow("CPong - Yet another Pong Clone", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    
    TTF_Font *font = TTF_OpenFont("data/font.otf", 24); // Load a font
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return -1;
    }

    Paddle player1 = {10, (WINDOW_HEIGHT - PADDLE_HEIGHT) / 2, PADDLE_WIDTH, PADDLE_HEIGHT};
    Paddle player2 = {WINDOW_WIDTH - 10 - PADDLE_WIDTH, (WINDOW_HEIGHT - PADDLE_HEIGHT) / 2, PADDLE_WIDTH, PADDLE_HEIGHT};
    Ball ball;
    reset_ball(&ball);

    bool running = true;
    SDL_Event event;

    while (running) {
        int frameStart = SDL_GetTicks(); // Start the frame timer

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_W] && player1.y > 0) player1.y -= PADDLE_SPEED;
        if (state[SDL_SCANCODE_S] && player1.y < WINDOW_HEIGHT - PADDLE_HEIGHT) player1.y += PADDLE_SPEED;
        if (state[SDL_SCANCODE_UP] && player2.y > 0) player2.y -= PADDLE_SPEED;
        if (state[SDL_SCANCODE_DOWN] && player2.y < WINDOW_HEIGHT - PADDLE_HEIGHT) player2.y += PADDLE_SPEED;

        ball.x += ball.vx;
        ball.y += ball.vy;

        if (ball.y <= 0 || ball.y >= WINDOW_HEIGHT - BALL_SIZE) {
            ball.vy = -ball.vy;
        }

        if ((ball.x <= player1.x + PADDLE_WIDTH && ball.y + BALL_SIZE >= player1.y && ball.y <= player1.y + PADDLE_HEIGHT) ||
            (ball.x + BALL_SIZE >= player2.x && ball.y + BALL_SIZE >= player2.y && ball.y <= player2.y + PADDLE_HEIGHT)) {
            ball.vx = -ball.vx;
        }

        // Scoring logic
        if (ball.x < 0) {
            PLAYER2_SCORE++; // Player 2 scores
            reset_ball(&ball);
        } else if (ball.x > WINDOW_WIDTH) {
            PLAYER1_SCORE++; // Player 1 scores
            reset_ball(&ball);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect player1Rect = {player1.x, player1.y, player1.w, player1.h};
        SDL_Rect player2Rect = {player2.x, player2.y, player2.w, player2.h};
        SDL_Rect ballRect = {ball.x, ball.y, BALL_SIZE, BALL_SIZE};
        SDL_RenderFillRect(renderer, &player1Rect);
        SDL_RenderFillRect(renderer, &player2Rect);
        SDL_RenderFillRect(renderer, &ballRect);
        
        render_score(renderer, font); // Render the score

        SDL_RenderPresent(renderer);

        int frameTime = SDL_GetTicks() - frameStart; // Calculate frame time
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime); // Delay to maintain FPS
        }
    }

    TTF_CloseFont(font); // Close the font
    TTF_Quit(); // Quit SDL_ttf
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
