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

// Funções de inicialização
void initGame();
void resetGame();

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
    // Inicialização do Raylib e da janela
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Invasores do espaço");
    SetTargetFPS(60);

    // Inicialização das funcionalidades de tela, timer e teclado
    screenInit(1);
    timerInit(1);
    keyboardInit();

    initGame();

    // Loop principal do jogo
    while (!WindowShouldClose())
    {
        // Atualização do jogo com timer
        if (timerTimeOver())
        {
            screenClear();

            // Controle do jogador com o teclado usando Raylib
            if (IsKeyDown(KEY_A))
                playerX -= PLAYER_SPEED;
            if (IsKeyDown(KEY_D))
                playerX += PLAYER_SPEED;
            if (IsKeyPressed(KEY_W))
                shootBullet();

            // Limites da tela para o jogador
            if (playerX < 0) playerX = 0;
            if (playerX > SCREEN_WIDTH - 20) playerX = SCREEN_WIDTH - 20;

            // Atirar aleatoriamente
            if (alienBulletY < 0 && (rand() % 50) == 0)
            {
                alienShoot(); // Atira aleatoriamente
            }

            // Atualizações de jogo
            updateBullet();
            updateAlienBullet();
            updateInvaders();
            checkCollisions();

            // Desenha os elementos do jogo
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

    // Limpeza e encerramento
    keyboardDestroy();
    timerDestroy();
    screenDestroy();
    CloseWindow();
    return 0;
}

void initGame()
{
    // Configurações iniciais
    playerX = SCREEN_WIDTH / 2;
    playerY = SCREEN_HEIGHT - 50;
    bulletX = bulletY = -1;
    alienBulletX = alienBulletY = -1;
    score = 0;

    // Configuração dos invasores
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
        }
    }

    // Verifica bordas e altera direção
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
    // Tenta encontrar um invasor aleatório para atirar
    for (int i = INVADER_ROWS - 1; i >= 0; i--)
    {
        for (int j = rand() % INVADER_COLS; j < INVADER_COLS; j++)
        {
            if (invaders[i][j] == 1)
            {
                alienBulletX = invaderPosX[i][j];
                alienBulletY = invaderPosY[i][j] + 20; // Define a posição do tiro do alien
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
        gameOver();
        resetGame();
    }
}

void resetGame()
{
    score = 0;
    bulletX = bulletY = -1;
    alienBulletX = alienBulletY = -1;
    initGame();
}
