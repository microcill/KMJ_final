#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>

// ȭ�� ũ�� ����
#define SCREEN_WIDTH 656
#define SCREEN_HEIGHT 369

// ��ũ�η� ����� ���� (������ Ÿ�� ���)
#define BLOCK_ROWS 5
#define BLOCK_COLUMNS 10
#define TOTAL_BLOCKS (BLOCK_ROWS * BLOCK_COLUMNS)

// ���� ���� ���� ����
typedef enum GameScreen { TITLE, GAMEPLAY, GAMEOVER, WIN } GameScreen;

// ��� ����ü ����
typedef struct Block {
    Rectangle rect;
    bool active;      // ��� ���� ����
} Block;


int main(void)
{
    // ������ �ʱ�ȭ
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout Game");
    SetTargetFPS(60);

    // ���� ���� �ʱ�ȭ: ���� ȭ�� (TITLE)
    GameScreen currentScreen = TITLE;

    // �� ���� �ʱ�ȭ
    Vector2 ballPos = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    float ballRadius = 8.0f;
    Vector2 ballSpeed = { 200, -200 };

    // �е� ���� �ʱ�ȭ
    Rectangle paddle = { SCREEN_WIDTH / 2.0f - 50, SCREEN_HEIGHT - 20, 100, 10 };

    // ��� ũ�� �� ���� ����
    const float blockWidth = 60.0f;
    const float blockHeight = 20.0f;
    const float blockSpacing = 5.0f;
    // ������ Ÿ�� ��� TOTAL_BLOCKS ���
    Block blocks[TOTAL_BLOCKS];

    // ��� �迭 �ʱ�ȭ: ȭ�� ��ܿ��� ���� ������ �ΰ� ����
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

    // ���� ���� �ʱ�ȭ
    int score = 0;

    //��� �ʱ�ȭ
    int heart = 3;

    // ���� ����
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();



        // === ������Ʈ �κ� === //
        if (currentScreen == TITLE)
        {
            // ���� ȭ�鿡�� ENTER Ű�� ������ ���� ����
            if (IsKeyPressed(KEY_ENTER))
            {
                // ���� ����� �� ���� ����
                ballPos = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
                ballSpeed = (Vector2){ 200, -200 };
                paddle.x = SCREEN_WIDTH / 2.0f - paddle.width / 2;
                score = 0;
                heart = 3;

                // ��� �迭 �缳��
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
            // �е� �̵�: Ű����(��/��) �� ���콺 �̵����� ����
            if (IsKeyDown(KEY_RIGHT))
                paddle.x += 300 * dt;
            if (IsKeyDown(KEY_LEFT))
                paddle.x -= 300 * dt;

            // �е��� ȭ���� ����� �ʵ��� ����
            if (paddle.x < 0) paddle.x = 0;
            if (paddle.x + paddle.width > SCREEN_WIDTH)
                paddle.x = SCREEN_WIDTH - paddle.width;

            // �� �̵�
            ballPos.x += ballSpeed.x * dt;
            ballPos.y += ballSpeed.y * dt;

            // ���� �浹 ó�� (����/����)
            if ((ballPos.x - ballRadius <= 0) || (ballPos.x + ballRadius >= SCREEN_WIDTH))
                ballSpeed.x *= -1;

            // ��� �浹
            if (ballPos.y - ballRadius <= 0)
                ballSpeed.y *= -1;

            // �е�� �浹 (�ܼ� �浹 ó��: ���� ���� ���� ����)
            if (CheckCollisionCircleRec(ballPos, ballRadius, paddle))
            {
                ballSpeed.y *= -1;
                ballPos.y = paddle.y - ballRadius; // ���� �е� ���� Ƣ���� ��ġ ����
            }

            // ��ϰ� �浹 ó�� �� ���� ����
            for (int i = 0; i < TOTAL_BLOCKS; i++)
            {
                if (blocks[i].active && CheckCollisionCircleRec(ballPos, ballRadius, blocks[i].rect))
                {
                    blocks[i].active = false;
                    score += 10;
                    ballSpeed.y *= -1;  // �ܼ��ϰ� ���� ���� ���� ����
                    break;  // �� �����ӿ� �� ��ϸ� ó��
                }
            }

            // ���� ȭ�� �Ʒ��� �������� ��� �ϳ� ����
            if (ballPos.y - ballRadius > SCREEN_HEIGHT && heart != 0)
            {
                heart--;
                ballPos = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
                ballSpeed = (Vector2){ 200, 200 };
            }

            //��� 3�� �� ������� ���� ���� ó��
            if (heart == 0) {
                currentScreen = GAMEOVER;
            }

            // ��� ��� ���� �� ���� Ŭ����
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
            // ���� ����(�й� �Ǵ� �¸�) ȭ�鿡�� ENTER Ű�� ������ Ÿ��Ʋ ȭ������ ����
            if (IsKeyPressed(KEY_ENTER))
            {
                currentScreen = TITLE;
            }

            if (IsKeyPressed(KEY_Q))
            {
                break;
            }
        }

        // === �׸��� �κ� === //
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentScreen == TITLE)
        {
            // Ÿ��Ʋ ȭ��: ���� ���� �� ���� �ȳ�
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
            // ���� �÷��� ȭ�� �׸���

            // �е� �׸���
            DrawRectangleRec(paddle, BLACK);

            // �� �׸���
            DrawCircleV(ballPos, ballRadius, MAROON);

            // Ȱ�� ��� �׸���
            for (int i = 0; i < TOTAL_BLOCKS; i++)
            {
                if (blocks[i].active)
                    DrawRectangleRec(blocks[i].rect, BLUE);
            }

            // ���� ���
            char scoreText[20];
            sprintf_s(scoreText, sizeof(scoreText), "Score: %d", score);
            DrawText(scoreText, 10, 10, 20, DARKGRAY);
            DrawText(TextFormat("heart: %d", heart), 150, 10, 20, DARKGRAY);
        }
        else if (currentScreen == GAMEOVER)
        {
            // ���� ���� ȭ��
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
            // ���� Ŭ����(�¸�) ȭ��
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

    // ���� ó��
    CloseWindow();

    return 0;
}