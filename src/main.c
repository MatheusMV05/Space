#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "timer.h"
#include "keyboard.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_SPEED 5
#define BULLET_SPEED 10
#define INVADER_ROWS 5
#define INVADER_COLS 10
#define INVADER_SPEED 2
#define MAX_NAME_LENGTH 20

// Estruturas de sprites
const char *PLAYER_SPRITE = "A";
const char *INVADER_SPRITE = "M";
const char *BULLET_SPRITE = "|";
const char *ALIEN_BULLET_SPRITE = "v";

// Variáveis globais do jogo
int playerX, playerY;
int bulletX = -1, bulletY = -1;
int alienBulletX = -1, alienBulletY = -1;
int score = 0;
int invaders[INVADER_ROWS][INVADER_COLS];
int invaderPosX[INVADER_ROWS][INVADER_COLS];
int invaderPosY[INVADER_ROWS][INVADER_COLS];
int gameState = 0; // 0 = menu, 1 = jogando, 2 = game over
char playerName[MAX_NAME_LENGTH + 1] = {0}; // Nome do jogador
int nameIndex = 0; // Índice para controle de caracteres
int nameEntered = 0; // Flag para verificar se o nome foi inserido

// Funções de inicialização
void initGame();
void resetGame();
void saveScore();

// Funções de desenho
void drawPlayer();
void drawInvaders();
void drawBullet();
void drawAlienBullet();
void displayScore();
void gameOver();

// Funções de movimentação e controle
void movePlayer();
void shootBullet();
void updateBullet();
void updateAlienBullet();
void updateInvaders();
void alienShoot();
void checkCollisions();

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Invasores do espaço");
    SetTargetFPS(60);

    screenInit(1);
    timerInit(1);
    keyboardInit();

    initGame();

    while (!WindowShouldClose())
    {
        // Verifica se o jogador quer sair do jogo
        if (IsKeyPressed(KEY_ESCAPE)) 
        {
            if (gameState == 2 && nameEntered)
            {
                saveScore();
            }
            break;
        }

        if (gameState == 0) // Menu inicial
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Invasores do Espaco", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 100, 40, WHITE);
            DrawText("Pressione ENTER para comecar", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, 20, WHITE);
            DrawText("Pressione ESC para sair", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 40, 20, WHITE);
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) 
            {
                gameState = 1;
            }
        }
        else if (gameState == 1) // Jogo em andamento
        {
            if (timerTimeOver())
            {
                screenClear();

                if (IsKeyDown(KEY_A))
                    playerX -= PLAYER_SPEED;
                if (IsKeyDown(KEY_D))
                    playerX += PLAYER_SPEED;
                if (IsKeyPressed(KEY_W))
                    shootBullet();

                if (playerX < 0) playerX = 0;
                if (playerX > SCREEN_WIDTH - 20) playerX = SCREEN_WIDTH - 20;

                if (alienBulletY < 0 && (rand() % 50) == 0)
                {
                    alienShoot();
                }

                updateBullet();
                updateAlienBullet();
                updateInvaders();
                checkCollisions();

                BeginDrawing();
                ClearBackground(BLACK);
                drawPlayer();
                drawInvaders();
                drawBullet();
                drawAlienBullet();
                displayScore();
                EndDrawing();

                screenUpdate();
            }
        }
        else if (gameState == 2) // Game over
        {
            if (!nameEntered) // Solicita o nome do jogador no game over
            {
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("Game Over!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100, 40, RED);
                DrawText("Digite seu nome:", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 20, WHITE);
                DrawText(playerName, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 20, WHITE);
                DrawText("Pressione ENTER para confirmar", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 40, 20, WHITE);
                EndDrawing();

                int key = GetCharPressed();
                if (key >= 32 && key <= 125 && nameIndex < MAX_NAME_LENGTH) // Caracteres imprimíveis
                {
                    playerName[nameIndex++] = (char)key;
                    playerName[nameIndex] = '\0'; // Null-terminator
                }
                if (IsKeyPressed(KEY_BACKSPACE) && nameIndex > 0) // Remover o último caractere
                {
                    playerName[--nameIndex] = '\0';
                }
                if (IsKeyPressed(KEY_ENTER) && nameIndex > 0) // Confirma o nome
                {
                    nameEntered = 1;
                    saveScore(); // Salva o score ao final do jogo
                }
            }
            else
            {
                BeginDrawing();
                ClearBackground(BLACK);
                gameOver();
                DrawText("Pressione R para reiniciar", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 40, 20, WHITE);
                DrawText("Pressione M para voltar ao menu", SCREEN_WIDTH / 2 - 170, SCREEN_HEIGHT / 2 + 80, 20, WHITE);
                DrawText("Pressione ESC para sair", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 120, 20, WHITE);
                EndDrawing();

                if (IsKeyPressed(KEY_R))
                {
                    resetGame();
                    gameState = 1;
                }
                else if (IsKeyPressed(KEY_M)) // Volta ao menu
                {
                    resetGame();
                    gameState = 0;
                    nameIndex = 0;
                    nameEntered = 0;
                    playerName[0] = '\0'; // Limpa o nome do jogador
                }
            }
        }
    }

    // Limpeza e encerramento
    keyboardDestroy();
    timerDestroy();
    screenDestroy();
    CloseWindow();
    return 0;
}

void saveScore()
{
    FILE *file = fopen("score.txt", "a");
    if (file != NULL)
    {
        fprintf(file, "Nome: %s, Score: %d\n", playerName, score);
        fclose(file);
    }
    else
    {
        printf("Erro ao abrir o arquivo de score.\n");
    }
}

void initGame()
{
    playerX = SCREEN_WIDTH / 2;
    playerY = SCREEN_HEIGHT - 50;
    bulletX = bulletY = -1;
    alienBulletX = alienBulletY = -1;
    score = 0;

    for (int i = 0; i < INVADER_ROWS; i++)
    {
        for (int j = 0; j < INVADER_COLS; j++)
        {
            invaders[i][j] = 1;
            invaderPosX[i][j] = 50 + j * 60;
            invaderPosY[i][j] = 50 + i * 40;
        }
    }
}

void drawPlayer()
{
    DrawText(PLAYER_SPRITE, playerX, playerY, 20, WHITE);
}

void drawInvaders()
{
    for (int i = 0; i < INVADER_ROWS; i++)
    {
        for (int j = 0; j < INVADER_COLS; j++)
        {
            if (invaders[i][j] == 1)
            {
                DrawText(INVADER_SPRITE, invaderPosX[i][j], invaderPosY[i][j], 20, WHITE);
            }
        }
    }
}

void drawBullet()
{
    if (bulletY > 0)
    {
        DrawText(BULLET_SPRITE, bulletX, bulletY, 20, WHITE);
    }
}

void drawAlienBullet()
{
    if (alienBulletY > 0)
    {
        DrawText(ALIEN_BULLET_SPRITE, alienBulletX, alienBulletY, 20, WHITE);
    }
}

void displayScore()
{
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    DrawText(scoreText, 10, 10, 20, GREEN);
}

void gameOver()
{
    char message[50];
    sprintf(message, "Game Over! Score: %d", score);
    DrawText(message, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 20, RED);
}

void shootBullet()
{
    if (bulletY < 0)
    {
        bulletX = playerX + 10;
        bulletY = playerY - 10;
    }
}

void updateBullet()
{
    if (bulletY > 0)
    {
        bulletY -= BULLET_SPEED;
    }
    else
    {
        bulletX = -1;
        bulletY = -1;
    }
}

void updateAlienBullet()
{
    if (alienBulletY > 0)
    {
        alienBulletY += BULLET_SPEED;
    }
    else
    {
        alienBulletX = -1;
        alienBulletY = -1;
    }
}

void updateInvaders()
{
    static int direction = INVADER_SPEED;
    static int stepDown = 0;

    for (int i = 0; i < INVADER_ROWS; i++)
    {
        for (int j = 0; j < INVADER_COLS; j++)
        {
            invaderPosX[i][j] += direction;
            if (invaderPosY[i][j] > playerY - 20)
            {
                gameState = 2;
                return;
            }
        }
    }

    if (invaderPosX[0][0] <= 0 || invaderPosX[0][INVADER_COLS - 1] >= SCREEN_WIDTH - 20)
    {
        direction = -direction;
        stepDown = 1;
    }

    if (stepDown)
    {
        for (int i = 0; i < INVADER_ROWS; i++)
        {
            for (int j = 0; j < INVADER_COLS; j++)
            {
                invaderPosY[i][j] += 20;
            }
        }
        stepDown = 0;
    }
}

void alienShoot()
{
    for (int i = INVADER_ROWS - 1; i >= 0; i--)
    {
        for (int j = rand() % INVADER_COLS; j < INVADER_COLS; j++)
        {
            if (invaders[i][j] == 1)
            {
                alienBulletX = invaderPosX[i][j];
                alienBulletY = invaderPosY[i][j] + 20;
                return;
            }
        }
    }
}

void checkCollisions()
{
    for (int i = 0; i < INVADER_ROWS; i++)
    {
        for (int j = 0; j < INVADER_COLS; j++)
        {
            if (invaders[i][j] == 1)
            {
                if (bulletX >= invaderPosX[i][j] && bulletX <= invaderPosX[i][j] + 10 &&
                    bulletY >= invaderPosY[i][j] && bulletY <= invaderPosY[i][j] + 10)
                {
                    invaders[i][j] = 0;
                    bulletX = bulletY = -1;
                    score += 10;
                }
            }
        }
    }

    if (alienBulletY > playerY && alienBulletY < playerY + 10 && alienBulletX > playerX && alienBulletX < playerX + 20)
    {
        gameState = 2;
    }
}

void resetGame()
{
    score = 0;
    bulletX = bulletY = -1;
    alienBulletX = alienBulletY = -1;
    initGame();
}
