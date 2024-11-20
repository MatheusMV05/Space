#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "timer.h"
#include <stdbool.h>

#define SCREEN_WIDTH 800     
#define SCREEN_HEIGHT 600   
#define PLAYER_SPEED 5       
#define BULLET_SPEED 10      
#define INVADER_ROWS 5       
#define INVADER_COLS 10      
#define INVADER_SPEED 2      
#define MAX_NAME_LENGTH 20   
#define MAX_ALIEN_BULLETS 50 


const char *PLAYER_SPRITE = "A";       
const char *INVADER_SPRITE = "M";      
const char *BULLET_SPRITE = "|";       
const char *ALIEN_BULLET_SPRITE = "v"; 


int playerX, playerY;                        
int bulletX = -1, bulletY = -1;              
int alienBulletX = -1, alienBulletY = -1;    
int score = 0;                               
int invaders[INVADER_ROWS][INVADER_COLS];    
int invaderPosX[INVADER_ROWS][INVADER_COLS]; 
int invaderPosY[INVADER_ROWS][INVADER_COLS]; 
int gameState = 0;                           
char playerName[MAX_NAME_LENGTH + 1] = {0};  
int nameIndex = 0;                           
int nameEntered = 0;                         


void initGame(); 
void resetGame(); 
void saveScore(); 


void drawPlayer();      
void drawInvaders();    
void drawBullet();      
void drawAlienBullet(); 
void displayScore();    
void gameOver();        
void highScoreScreen(); 


void shootBullet();       
void updateBullet();      
void updateAlienBullet(); 
void updateInvaders();   
void alienShoot();        
void checkCollisions();   


typedef struct AlienBullet
{
    int x;
    int y;
    int active; 
} AlienBullet;


AlienBullet alienBullets[MAX_ALIEN_BULLETS];


typedef struct
{
    char name[MAX_NAME_LENGTH + 1];
    int score;
} PlayerScore;


void sortScores(PlayerScore scores[], int count)
{
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            if (scores[j].score > scores[i].score)
            {
                // Troca os elementos
                PlayerScore temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }
}

int main(void)
{
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Invasores do espaço");
    
    timerInit(1000 / 60); 

    
    initGame();

    
    while (!WindowShouldClose())
    {
        
        if (IsKeyPressed(KEY_ESCAPE))
        {
            if (gameState == 2 && nameEntered)
            {
                saveScore(); 
            }
            break; 
        }

        if (gameState == 0) 
        {
            
            BeginDrawing();
            ClearBackground(BLACK);

            
            int titleWidth = MeasureText("Invasores do Espaco", 40);
            DrawText("Invasores do Espaco", (SCREEN_WIDTH - titleWidth) / 2, SCREEN_HEIGHT / 2 - 100, 40, PURPLE);

            
            int enterWidth = MeasureText("Pressione ENTER para comecar", 20);
            DrawText("Pressione ENTER para comecar", (SCREEN_WIDTH - enterWidth) / 2, SCREEN_HEIGHT / 2, 20, WHITE);

            
            int highScoreWidth = MeasureText("Pressione H para high scores", 20);
            DrawText("Pressione H para high scores", (SCREEN_WIDTH - highScoreWidth) / 2, SCREEN_HEIGHT / 2 + 40, 20, WHITE);

            
            int escWidth = MeasureText("Pressione ESC para sair", 20);
            DrawText("Pressione ESC para sair", (SCREEN_WIDTH - escWidth) / 2, SCREEN_HEIGHT / 2 + 80, 20, WHITE);
            EndDrawing();

            
            if (IsKeyPressed(KEY_ENTER))
            {
                gameState = 1;
            }
            else if (IsKeyPressed(KEY_H))
            {
                gameState = 3;
            }
        }
        else if (gameState == 1) 
        {
            if (timerTimeOver()) 
            {
                screenClear(); 

                
                if (IsKeyDown(KEY_A))
                {
                    playerX -= PLAYER_SPEED;
                }
                if (IsKeyDown(KEY_D))
                {
                    playerX += PLAYER_SPEED;
                }
                if (IsKeyPressed(KEY_W))
                {
                    shootBullet();
                } 

                
                if (playerX < 0) {
                    playerX = 0;
                }
                if (playerX > SCREEN_WIDTH - 20) {
                    playerX = SCREEN_WIDTH - 20;
                }

                
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
        else if (gameState == 2) 
        {
            if (!nameEntered) 
            {
                BeginDrawing();
                ClearBackground(BLACK);

                
                int gameOverWidth = MeasureText("Game Over!", 40);
                DrawText("Game Over!", (SCREEN_WIDTH - gameOverWidth) / 2, SCREEN_HEIGHT / 2 - 100, 40, RED);

                
                int enterNameWidth = MeasureText("Digite seu nome:", 20);
                DrawText("Digite seu nome:", (SCREEN_WIDTH - enterNameWidth) / 2, SCREEN_HEIGHT / 2 - 50, 20, WHITE);

                
                int playerNameWidth = MeasureText(playerName, 20);
                DrawText(playerName, (SCREEN_WIDTH - playerNameWidth) / 2, SCREEN_HEIGHT / 2, 20, WHITE);

                
                int confirmWidth = MeasureText("Pressione ENTER para confirmar", 20);
                DrawText("Pressione ENTER para confirmar", (SCREEN_WIDTH - confirmWidth) / 2, SCREEN_HEIGHT / 2 + 40, 20, WHITE);
                EndDrawing();

                
                int key = GetCharPressed();
                if (key >= 32 && key <= 125 && nameIndex < MAX_NAME_LENGTH) 
                {
                    playerName[nameIndex++] = (char)key;
                    playerName[nameIndex] = '\0'; 
                }
                if (IsKeyPressed(KEY_BACKSPACE) && nameIndex > 0) 
                {
                    playerName[--nameIndex] = '\0';
                }
                if (IsKeyPressed(KEY_ENTER) && nameIndex > 0) 
                {
                    nameEntered = 1;
                    saveScore(); 
                }
            }
            else
            {
                
                BeginDrawing();
                ClearBackground(BLACK);
                gameOver();

                
                int restartTextWidth = MeasureText("Pressione R para reiniciar", 20);
                DrawText("Pressione R para reiniciar", (SCREEN_WIDTH - restartTextWidth) / 2, SCREEN_HEIGHT / 2 + 40, 20, WHITE);

                
                int menuTextWidth = MeasureText("Pressione M para voltar ao menu", 20);
                DrawText("Pressione M para voltar ao menu", (SCREEN_WIDTH - menuTextWidth) / 2, SCREEN_HEIGHT / 2 + 80, 20, WHITE);

                
                int sairTextWidth = MeasureText("Pressione ESC para sair", 20);
                DrawText("Pressione ESC para sair", (SCREEN_WIDTH - sairTextWidth) / 2, SCREEN_HEIGHT / 2 + 120, 20, WHITE);
                EndDrawing();

                
                if (IsKeyPressed(KEY_R))
                {
                    resetGame();
                    gameState = 1;
                }
                else if (IsKeyPressed(KEY_M)) 
                {
                    resetGame();
                    gameState = 0;
                    nameIndex = 0;
                    nameEntered = 0;
                    playerName[0] = '\0'; 
                }
            }
        }
        else if (gameState == 3) 
        {
            highScoreScreen();
        }
    }

    

    timerDestroy();
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
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++)
    {
        if (alienBullets[i].active)
        {
            DrawText(ALIEN_BULLET_SPRITE, alienBullets[i].x, alienBullets[i].y, 20, WHITE);
        }
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


void highScoreScreen()
{
    FILE *file = fopen("score.txt", "r");
    if (file != NULL)
    {
        char line[100];
        PlayerScore scores[100]; 
        int count = 0;

        
        while (fgets(line, sizeof(line), file) && count < 100)
        {
            sscanf(line, "Nome: %20[^,], Score: %d", scores[count].name, &scores[count].score);
            count++;
        }
        fclose(file);

        
        sortScores(scores, count);

        
        BeginDrawing();
        ClearBackground(BLACK);

        int titleWidth = MeasureText("High Scores", 40);
        DrawText("High Scores", (SCREEN_WIDTH - titleWidth) / 2, 50, 40, PURPLE);

        int yOffset = 100;
        for (int i = 0; i < count && i < 5; i++)
        { 
            char scoreText[100];
            sprintf(scoreText, "%d. %s - %d", i + 1, scores[i].name, scores[i].score);
            DrawText(scoreText, 100, yOffset, 20, WHITE);
            yOffset += 30;
        }

        int menuTextWidth = MeasureText("Pressione M para voltar ao menu", 20);
        DrawText("Pressione M para voltar ao menu", (SCREEN_WIDTH - menuTextWidth) / 2, SCREEN_HEIGHT - 100, 20, WHITE);
        EndDrawing();

        if (IsKeyPressed(KEY_M))
        {
            gameState = 0;
        }
    }
    else
    {
        printf("Erro ao abrir o arquivo de score.\n");
    }
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
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++)
    {
        if (alienBullets[i].active)
        {
            alienBullets[i].y += 5; 
            if (alienBullets[i].y > SCREEN_HEIGHT)
            {
                alienBullets[i].active = 0; 
            }
        }
    }
}


void updateInvaders()
{
    static int direction = INVADER_SPEED; 
    static int stepDown = 0;              

    int invadersLeft = 0; 

    
    for (int i = 0; i < INVADER_ROWS; i++)
    {
        for (int j = 0; j < INVADER_COLS; j++)
        {
            if (invaders[i][j] == 1) 
            {
                invadersLeft = 1;
                

                invaderPosX[i][j] += direction;
                

                if (invaderPosY[i][j] > playerY - 20)
                
                {
                    gameState = 2;
                   
                    return;
                }
            }
        }
    }

    if (!invadersLeft)
    {
        gameState = 3;
        
    }

    
    int leftMostInvader = SCREEN_WIDTH;
    int rightMostInvader = 0;

    for (int i = 0; i < INVADER_ROWS; i++)
    {
        for (int j = 0; j < INVADER_COLS; j++)
        {
            if (invaders[i][j] == 1)
            {
                if (invaderPosX[i][j] < leftMostInvader)
                {
                    leftMostInvader = invaderPosX[i][j];
                }
                if (invaderPosX[i][j] > rightMostInvader)
                {
                    rightMostInvader = invaderPosX[i][j];
                }
            }
        }
    }

    if (leftMostInvader <= 0 || rightMostInvader >= SCREEN_WIDTH - 20)
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
    
    for (int i = 0; i < INVADER_ROWS; i++)
    {
        for (int j = 0; j < INVADER_COLS; j++)
        {
            if (invaders[i][j] == 1)
            {                                     
                int index = i * INVADER_COLS + j; 
                if (alienBullets[index].active == 0 && (rand() % 30) < 2)
                {                                                   
                    alienBullets[index].x = invaderPosX[i][j] + 20; 
                    alienBullets[index].y = invaderPosY[i][j] + 20; 
                    alienBullets[index].active = 1;                 
                }
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
                if (bulletX >= invaderPosX[i][j] && bulletX <= invaderPosX[i][j] + 20 && bulletY >= invaderPosY[i][j] && bulletY <= invaderPosY[i][j] + 20)
                {                           
                    invaders[i][j] = 0;     
                    bulletX = bulletY = -1; 
                    score += 10;            
                }
            }
        }
    }

    
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++)
    {
        if (alienBullets[i].active)
        { 
            if (alienBullets[i].x > playerX && alienBullets[i].x < playerX + 20 && alienBullets[i].y > playerY && alienBullets[i].y < playerY + 20)
            {                  
                gameState = 2; 
            }
        }
    }
}


void resetGame()
{
    score = 0;                        
    bulletX = bulletY = -1;           
    alienBulletX = alienBulletY = -1; 
    initGame();                       
}
