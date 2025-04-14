#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>

// 화면 크기 설정
#define SCREEN_WIDTH 656
#define SCREEN_HEIGHT 369

// 매크로로 상수값 정의 (컴파일 타임 상수)
#define BLOCK_ROWS 5
#define BLOCK_COLUMNS 10
#define TOTAL_BLOCKS (BLOCK_ROWS * BLOCK_COLUMNS)

// 게임 진행 상태 정의
typedef enum GameScreen { TITLE, GAMEPLAY, GAMEOVER, WIN } GameScreen;

// 블록 구조체 정의
typedef struct Block {
    Rectangle rect;
    bool active;      // 블록 존재 여부
} Block;


int main(void)
{
    // 윈도우 초기화
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout Game");
    SetTargetFPS(60);

    // 게임 상태 초기화: 시작 화면 (TITLE)
    GameScreen currentScreen = TITLE;

    // 공 변수 초기화
    Vector2 ballPos = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    float ballRadius = 8.0f;
    Vector2 ballSpeed = { 200, -200 };

    // 패들 변수 초기화
    Rectangle paddle = { SCREEN_WIDTH / 2.0f - 50, SCREEN_HEIGHT - 20, 100, 10 };

    // 블록 크기 및 간격 설정
    const float blockWidth = 60.0f;
    const float blockHeight = 20.0f;
    const float blockSpacing = 5.0f;
    // 컴파일 타임 상수 TOTAL_BLOCKS 사용
    Block blocks[TOTAL_BLOCKS];

    // 블록 배열 초기화: 화면 상단에서 일정 간격을 두고 생성
    int index = 0;
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLUMNS; col++) {
            float x = blockSpacing + col * (blockWidth + blockSpacing);
            float y = 50 + blockSpacing + row * (blockHeight + blockSpacing);
            blocks[index].rect = (Rectangle){ x, y, blockWidth, blockHeight };
            blocks[index].active = true;
            index++;
        }
    }

    // 점수 변수 초기화
    int score = 0;

    //목숨 초기화
    int heart = 3;

    // 게임 루프
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();



        // === 업데이트 부분 === //
        if (currentScreen == TITLE)
        {
            // 시작 화면에서 ENTER 키를 누르면 게임 시작
            if (IsKeyPressed(KEY_ENTER))
            {
                // 게임 재시작 시 변수 리셋
                ballPos = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
                ballSpeed = (Vector2){ 200, -200 };
                paddle.x = SCREEN_WIDTH / 2.0f - paddle.width / 2;
                score = 0;
                heart = 3;

                // 블록 배열 재설정
                index = 0;
                for (int row = 0; row < BLOCK_ROWS; row++)
                {
                    for (int col = 0; col < BLOCK_COLUMNS; col++)
                    {
                        blocks[index].active = true;
                        float x = blockSpacing + col * (blockWidth + blockSpacing);
                        float y = 50 + blockSpacing + row * (blockHeight + blockSpacing);
                        blocks[index].rect = (Rectangle){ x, y, blockWidth, blockHeight };
                        index++;
                    }
                }
                currentScreen = GAMEPLAY;
            }
        }
        else if (currentScreen == GAMEPLAY)
        {
            // 패들 이동: 키보드(좌/우) 및 마우스 이동으로 제어
            if (IsKeyDown(KEY_RIGHT))
                paddle.x += 300 * dt;
            if (IsKeyDown(KEY_LEFT))
                paddle.x -= 300 * dt;

            // 패들이 화면을 벗어나지 않도록 제한
            if (paddle.x < 0) paddle.x = 0;
            if (paddle.x + paddle.width > SCREEN_WIDTH)
                paddle.x = SCREEN_WIDTH - paddle.width;

            // 공 이동
            ballPos.x += ballSpeed.x * dt;
            ballPos.y += ballSpeed.y * dt;

            // 벽과 충돌 처리 (좌측/우측)
            if ((ballPos.x - ballRadius <= 0) || (ballPos.x + ballRadius >= SCREEN_WIDTH))
                ballSpeed.x *= -1;

            // 상단 충돌
            if (ballPos.y - ballRadius <= 0)
                ballSpeed.y *= -1;

            // 패들과 충돌 (단순 충돌 처리: 공의 수직 방향 반전)
            if (CheckCollisionCircleRec(ballPos, ballRadius, paddle))
            {
                ballSpeed.y *= -1;
                ballPos.y = paddle.y - ballRadius; // 공이 패들 위로 튀도록 위치 조정
            }

            // 블록과 충돌 처리 및 점수 증가
            for (int i = 0; i < TOTAL_BLOCKS; i++)
            {
                if (blocks[i].active && CheckCollisionCircleRec(ballPos, ballRadius, blocks[i].rect))
                {
                    blocks[i].active = false;
                    score += 10;
                    ballSpeed.y *= -1;  // 단순하게 공의 수직 방향 반전
                    break;  // 한 프레임에 한 블록만 처리
                }
            }

            // 공이 화면 아래로 떨어지면 목숨 하나 오버
            if (ballPos.y - ballRadius > SCREEN_HEIGHT && heart != 0)
            {
                heart--;
                ballPos = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
                ballSpeed = (Vector2){ 200, 200 };
            }

            //목숨 3개 다 사라지면 게임 오버 처리
            if (heart == 0) {
                currentScreen = GAMEOVER;
            }

            // 모든 블록 제거 시 게임 클리어
            bool allCleared = true;
            for (int i = 0; i < TOTAL_BLOCKS; i++)
            {
                if (blocks[i].active)
                {
                    allCleared = false;
                    break;
                }
            }
            if (allCleared)
            {
                currentScreen = WIN;
            }
        }
        else if (currentScreen == GAMEOVER || currentScreen == WIN)
        {
            // 게임 종료(패배 또는 승리) 화면에서 ENTER 키를 누르면 타이틀 화면으로 복귀
            if (IsKeyPressed(KEY_ENTER))
            {
                currentScreen = TITLE;
            }

            if (IsKeyPressed(KEY_Q))
            {
                break;
            }
        }

        // === 그리기 부분 === //
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentScreen == TITLE)
        {
            // 타이틀 화면: 게임 제목 및 시작 안내
            const char* titleText = "BREAKOUT";
            int titleFontSize = 40;
            int titleWidth = MeasureText(titleText, titleFontSize);
            DrawText(titleText, SCREEN_WIDTH / 2 - titleWidth / 2, SCREEN_HEIGHT / 2 - 50, titleFontSize, BLACK);

            const char* startText = "Press ENTER to start";
            int startFontSize = 20;
            int startWidth = MeasureText(startText, startFontSize);
            DrawText(startText, SCREEN_WIDTH / 2 - startWidth / 2, SCREEN_HEIGHT / 2 + 10, startFontSize, GRAY);
        }
        else if (currentScreen == GAMEPLAY)
        {
            // 게임 플레이 화면 그리기

            // 패들 그리기
            DrawRectangleRec(paddle, BLACK);

            // 공 그리기
            DrawCircleV(ballPos, ballRadius, MAROON);

            // 활성 블록 그리기
            for (int i = 0; i < TOTAL_BLOCKS; i++)
            {
                if (blocks[i].active)
                    DrawRectangleRec(blocks[i].rect, BLUE);
            }

            // 점수 출력
            char scoreText[20];
            sprintf_s(scoreText, sizeof(scoreText), "Score: %d", score);
            DrawText(scoreText, 10, 10, 20, DARKGRAY);
            DrawText(TextFormat("heart: %d", heart), 150, 10, 20, DARKGRAY);
        }
        else if (currentScreen == GAMEOVER)
        {
            // 게임 오버 화면
            const char* overText = "GAME OVER";
            int fontSize = 40;
            int textWidth = MeasureText(overText, fontSize);
            DrawText(overText, SCREEN_WIDTH / 2 - textWidth / 2, SCREEN_HEIGHT / 2 - 50, fontSize, RED);

            char finalScore[30];
            sprintf_s(finalScore, sizeof(finalScore), "Final Score: %d", score);
            int scoreTextWidth = MeasureText(finalScore, 20);
            DrawText(finalScore, SCREEN_WIDTH / 2 - scoreTextWidth / 2, SCREEN_HEIGHT / 2, 20, DARKGRAY);

            const char* returnText = "Press ENTER to return";
            int returnTextWidth = MeasureText(returnText, 20);
            DrawText(returnText, SCREEN_WIDTH / 2 - returnTextWidth / 2, SCREEN_HEIGHT / 2 + 40, 20, GRAY);

            const char* quitText = "Press Q to quit";
            int quitTextWidth = MeasureText(quitText, 20);
            DrawText(quitText, SCREEN_WIDTH / 2 - quitTextWidth / 2, SCREEN_HEIGHT / 2 + 80, 20, GRAY);
        }
        else if (currentScreen == WIN)
        {
            // 게임 클리어(승리) 화면
            const char* winText = "YOU WIN!";
            int fontSize = 40;
            int textWidth = MeasureText(winText, fontSize);
            DrawText(winText, SCREEN_WIDTH / 2 - textWidth / 2, SCREEN_HEIGHT / 2 - 50, fontSize, GREEN);

            char finalScore[30];
            sprintf_s(finalScore, sizeof(finalScore), "Final Score: %d", score);
            int scoreTextWidth = MeasureText(finalScore, 20);
            DrawText(finalScore, SCREEN_WIDTH / 2 - scoreTextWidth / 2, SCREEN_HEIGHT / 2, 20, DARKGRAY);

            const char* restartText = "Press ENTER to play again";
            int restartTextWidth = MeasureText(restartText, 20);
            DrawText(restartText, SCREEN_WIDTH / 2 - restartTextWidth / 2, SCREEN_HEIGHT / 2 + 40, 20, GRAY);

            const char* quitText = "Press Q to quit";
            int quitTextWidth = MeasureText(quitText, 20);
            DrawText(quitText, SCREEN_WIDTH / 2 - quitTextWidth / 2, SCREEN_HEIGHT / 2 + 80, 20, GRAY);
        }

        EndDrawing();
    }

    // 종료 처리
    CloseWindow();

    return 0;
}