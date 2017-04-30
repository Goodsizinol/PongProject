#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include "physics.hpp"

// Initializes the program and creates the window and the window surface.
bool init();

TTF_Font* gameFont = NULL;
SDL_Texture* fontTexture = NULL;

int ball_size;

enum mainMenuSelction{
    START_GAME,
    OPTIONS,
    EXIT
};

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

SDL_Window* window;
SDL_Renderer* renderer;

int paddle_width;
int paddle_height;

struct controls{
    bool P1_up;
    bool P1_down;
    bool P2_up;
    bool P2_down;
    bool restart;
}controlsBools;

bool init(){
    bool error = false;

    // Initializes SDL
    if(SDL_Init(SDL_INIT_EVERYTHING)<0){
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        error = true;
    } else{

        // Creates the game window
        window = SDL_CreateWindow("Pong: The Movie", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == NULL){
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            error = true;
        } else{

            // Creates the renderer
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
            if(renderer == NULL){
                printf("Could not create the renderer! SDL Error: %s\n", SDL_GetError());
                error = true;
            } if(TTF_Init() == -1){
                printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                error = true;
            }
        }
    }

    return error;
}

bool loadFont(int fontSize){
    bool error = false;
    gameFont = TTF_OpenFont("Pixeled.ttf", 20);

    return error;
}

void close(){
    TTF_CloseFont(gameFont);
    gameFont = NULL;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;

    TTF_Quit();
    SDL_Quit();

    return;
}

class object{
public:
    SDL_Rect r;
    SDL_Color objectColor;

    float true_x;
    float true_y;

    void offset(float x, float y){
        r.x = x;
        r.y = y;
        true_x = x;
        true_y = y;

        return;
    }

    void color(int r, int b, int g){
        objectColor.r = r;
        objectColor.b = b;
        objectColor.g = g;

        return;
    }

    void shape(int w, int h){
        r.w = w;
        r.h = h;
    }

    void applyShape(){
        SDL_SetRenderDrawColor(renderer, objectColor.r, objectColor.b, objectColor.g, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &r);
    }

    void debug(){
        printf("true x = %d  ", r.x);
        printf("true y = %d  ", r.y);
        printf("color = %d %d %d  ", objectColor.r, objectColor.b, objectColor.g);
        printf("w = %d  ", r.w);
        printf("h = %d\n", r.h);
    }
};

class gameStuff{
    public:
        void keyStateHandler(){
            const Uint8* state = SDL_GetKeyboardState(NULL);

            // P1 key states
            if(state[SDL_SCANCODE_W]){
                if(!state[SDL_SCANCODE_S]){
                    controlsBools.P1_up = true;
                }
            } else{
                controlsBools.P1_up = false;
            }
            if(state[SDL_SCANCODE_S]){
                if(!state[SDL_SCANCODE_W]){
                    controlsBools.P1_down = true;
                }
            } else{
                controlsBools.P1_down = false;
            }

            // P2 key states
            if(state[SDL_SCANCODE_UP]){
                if(!state[SDL_SCANCODE_DOWN]){
                    controlsBools.P2_up = true;
                }
            } else{
                controlsBools.P2_up = false;
            }
            if(state[SDL_SCANCODE_DOWN]){
                if(!state[SDL_SCANCODE_UP]){
                    controlsBools.P2_down = true;
                }
            } else{
                controlsBools.P2_down = false;
            }

            // Restarts the game with "R"
            if(state[SDL_SCANCODE_R]){
                controlsBools.restart = true;
            } else{
                controlsBools.restart = false;
            }

            return;
        }

        void resetGame(PHY_Paddle* LPaddle, PHY_Paddle* RPaddle, PHY_Ball* gBall){
            LPaddle->x = paddle_width*3;
            LPaddle->y = (WINDOW_HEIGHT/2)-(paddle_height/2);
            RPaddle->x = WINDOW_WIDTH - (paddle_width*4);
            RPaddle->y = (WINDOW_HEIGHT/2)-(paddle_height/2);

            *gBall = PHY.SetBallProperties(ball_size, 700, 0, 0, 0, true);
            gBall->x = (WINDOW_WIDTH/2)-(gBall->ballSize/2);
            gBall->y = (WINDOW_HEIGHT/2)-(gBall->ballSize/2);

        }

        bool CheckScore(PHY_Ball pBall){
            bool score = false;
            if(pBall.x > WINDOW_WIDTH || pBall.x < WINDOW_WIDTH - pBall.ballSize){
                score = true;
            }

            return score;
        }
}game;

void setObjectToPHY_Ball(PHY_Ball gBall, object* oBall){
    oBall->offset(gBall.x, gBall.y);
    oBall->shape(gBall.ballSize, gBall.ballSize);

    return;
}

void setObjectToPHY_Paddle(PHY_Paddle paddle, object* oPaddle){
    oPaddle->offset(paddle.x, paddle.y);
    oPaddle->shape(paddle.WIDTH, paddle.HEIGHT);

    return;
}

int main(int argc, char* argv[]){

    if(init()){
        printf("Error in initialization!\n");
    } else{

        PHY.SetWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

        ball_size = WINDOW_WIDTH/45;
        paddle_height = WINDOW_HEIGHT/7;
        paddle_width = WINDOW_WIDTH/40;

        bool quit = false;
        SDL_Event e;

        // Creates and sets the background properties
        object background;
        background.color(0, 0, 0);
        background.offset(0, 0);
        background.shape(WINDOW_WIDTH, WINDOW_HEIGHT);

        // Creates the ball for physics.hpp
        PHY_Ball ball;
        ball = PHY.SetBallProperties(ball_size, 700, 0, 0, 0, true);
        ball.x = (WINDOW_WIDTH/2)-(ball.ballSize/2);
        ball.y = (WINDOW_HEIGHT/2)-(ball.ballSize/2);

        // Creates the ball and sets it's properties to that of it in physics.hpp
        object pong_ball;
        pong_ball.color(255, 255, 255);
        setObjectToPHY_Ball(ball, &pong_ball);

        // Creates the paddles and sets it's properties
        object left_paddle;
        left_paddle.color(255, 255, 255);
        PHY_Paddle leftPHY_Paddle = PHY.CreatePaddle(paddle_width, paddle_height, paddle_width*3, (WINDOW_HEIGHT/2) - (paddle_height/2));
        setObjectToPHY_Paddle(leftPHY_Paddle, &left_paddle);

        object right_paddle;
        right_paddle.color(255, 255, 255);
        PHY_Paddle rightPHY_Paddle = PHY.CreatePaddle(paddle_width, paddle_height, WINDOW_WIDTH - (paddle_width*3) - paddle_width, (WINDOW_HEIGHT/2) - (paddle_height/2));
        setObjectToPHY_Paddle(rightPHY_Paddle, &right_paddle);

        while(!quit){

            background.applyShape();
            pong_ball.applyShape();

            while(SDL_PollEvent(&e) != 0){
                if(e.type == SDL_QUIT){
                    quit = true;
                } else{
                    game.keyStateHandler();
                }
            }

            pong_ball.applyShape();
            left_paddle.applyShape();
            right_paddle.applyShape();
            left_paddle.debug();
            right_paddle.debug();
            pong_ball.debug();


            if(controlsBools.P1_up || controlsBools.P1_down){
                PHY.movePaddle(&leftPHY_Paddle, controlsBools.P1_up);
                setObjectToPHY_Paddle(leftPHY_Paddle, &left_paddle);
            }
            if(controlsBools.P2_up || controlsBools.P2_down){
                PHY.movePaddle(&rightPHY_Paddle, controlsBools.P2_up);
                setObjectToPHY_Paddle(rightPHY_Paddle, &right_paddle);
            }
            if(controlsBools.restart){
                game.resetGame(&leftPHY_Paddle, &rightPHY_Paddle, &ball);
                setObjectToPHY_Paddle(leftPHY_Paddle, &left_paddle);
                setObjectToPHY_Paddle(rightPHY_Paddle, &right_paddle);
            }

            PHY.CheckCollision(&ball, leftPHY_Paddle, rightPHY_Paddle);
            setObjectToPHY_Ball(ball, &pong_ball);

            SDL_RenderPresent(renderer);
        }
    }

    close();

    return 0;
}
