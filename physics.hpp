#ifndef PHYSICS_HPP_INCLUDED
#define PHYSICS_HPP_INCLUDED

#endif // PHYSICS_HPP_INCLUDED

#include <math.h>
#include <SDL.h>

const float pi = 3.14159265;

struct PHY_Ball{
    int ballSize;
    float speed;
    float angle;
    float x;
    float y;
    bool moving_right;
};

struct PHY_Paddle{
    int WIDTH;
    int HEIGHT;
    float x;
    float y;
};

class physics{
    private:
        int WINDOW_WIDTH;
        int WINDOW_HEIGHT;

        float GetPaddleSurfaceAngle(PHY_Ball ball, PHY_Paddle paddle){
            float y = ball.y - paddle.y + ball.ballSize;
            float l = paddle.HEIGHT + ball.ballSize;
            float angle = ((pi/(4*l)) * y) - (pi/8);
            printf("l=%f, y=%f  ", l, y);
            printf("Paddle Angle: %f\n", (360*angle)/(2*pi));
            return angle;
        }

        void CheckWallCollision(PHY_Ball* pBall, bool* flag){
            if(pBall->y<0||pBall->y+pBall->ballSize>WINDOW_HEIGHT){
                GetNewAngle(&pBall->angle, pi/2);
                *flag = true;
            }
            return;
        }

        void CheckPaddleCollision(PHY_Paddle Lpaddle, PHY_Paddle Rpaddle, PHY_Ball* ball){
            // Checks if the ball is inside of the y range of either of the paddles
            if((ball->y + ball->ballSize >= Lpaddle.y && ball->y <= Lpaddle.y + Lpaddle.HEIGHT) || (ball->y + ball->ballSize >= Rpaddle.y && ball->y <= Rpaddle.y + Rpaddle.HEIGHT)){

                if(ball->moving_right){
                    if(ball->x + ball->ballSize >= Rpaddle.x && ball->x + ball->ballSize <= Rpaddle.x + Rpaddle.WIDTH){
                        GetNewAngle(&ball->angle, -GetPaddleSurfaceAngle(*ball, Rpaddle));
                        if(ball->angle < (float)(5*pi)/8){
                            ball->angle = (5*pi)/8;
                        } if(ball->angle > (float)(11*pi)/8){
                            ball->angle = (11*pi)/8;
                        }
                        ball->moving_right = false;
                    }
                } else{
                    if(ball->x >= Lpaddle.x && ball->x < Lpaddle.x + Lpaddle.WIDTH){
                        GetNewAngle(&ball->angle, GetPaddleSurfaceAngle(*ball, Lpaddle));
                        ball->moving_right = true;
                        if(ball->angle > (float)(3*pi)/8 && ball->angle < (float)(13*pi/8)){
                            if(ball->angle > pi){
                                ball->angle = (13*pi)/8;
                            } else{
                                ball->angle = (3*pi)/8;
                            }
                            printf("Corrected angle!\n");
                        }
                        ball->moving_right = true;
                    }
                }
            }

            return;
        }

        void GetNewAngle(float* angle, float surface_angle){
            *angle = pi +(2 * surface_angle) - *angle;
            if(*angle > 2*pi){
                *angle -= 2*pi;
            }
            if(*angle < 0){
                *angle += 2*pi;
            }
            printf("New angle: %f\n", *angle);
            return;
        }


    public:
        PHY_Ball SetBallProperties(int Size, float speed, float angle, float x, float y, bool moving_right){
            PHY_Ball pBall = {Size, speed, angle, x, y, moving_right};
            return pBall;
        }

        // Checks if the ball hits a wall or a paddle
        void CheckCollision(PHY_Ball* pBall, PHY_Paddle Lpaddle, PHY_Paddle Rpaddle){
            bool wall_collision = false;

            for(int i=0; i<3; i++){
                pBall->x += (pBall->speed/(60*3)) * cosf(pBall->angle);
                pBall->y += (pBall->speed/(60*3)) * sinf(pBall->angle);

                if(!wall_collision){
                    CheckWallCollision(pBall, &wall_collision);
                }

                CheckPaddleCollision(Lpaddle, Rpaddle, pBall);
            }

            return;
        }

        void SetWindow(int w, int h){
            WINDOW_WIDTH = w;
            WINDOW_HEIGHT = h;
        }

        void movePaddle(PHY_Paddle* paddle, bool moving_up){
            if(moving_up){
                paddle->y -= float(WINDOW_HEIGHT/60);
                if(paddle->y < 0){
                    paddle->y = 0;
                }
            } else{
                paddle->y += float(WINDOW_HEIGHT/60);
                if(paddle->y > WINDOW_HEIGHT-paddle->HEIGHT){
                    paddle->y = WINDOW_HEIGHT - paddle->HEIGHT;
                }
            }

            return;
        }

        PHY_Paddle CreatePaddle(int w, int h, float x, float y){
            PHY_Paddle paddle = {w, h, x, y};

            return paddle;
        }

} PHY;
