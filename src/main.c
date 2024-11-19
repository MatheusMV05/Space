#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "timer.h"
#include "keyboard.h"
#include <stdbool.h>

#define SCREEN_WIDTH 800     // Largura da tela do jogo
#define SCREEN_HEIGHT 600    // Altura da tela do jogo
#define PLAYER_SPEED 5       // Velocidade do jogador
#define BULLET_SPEED 10      // Velocidade da bala
#define INVADER_ROWS 5       // Número de linhas de invasores
#define INVADER_COLS 10      // Número de colunas de invasores
#define INVADER_SPEED 2      // Velocidade dos invasores
#define MAX_NAME_LENGTH 20   // Comprimento máximo do nome do jogador
#define MAX_ALIEN_BULLETS 50 // Limite de balas de alien que podem existir

// Estruturas de sprites
const char *PLAYER_SPRITE = "A";       // Sprite do jogador
const char *INVADER_SPRITE = "M";      // Sprite do invasor
const char *BULLET_SPRITE = "|";       // Sprite da bala do jogador
const char *ALIEN_BULLET_SPRITE = "v"; // Sprite da bala do invasor

// Variáveis globais do jogo
int playerX, playerY;                        // Posição do jogador
int bulletX = -1, bulletY = -1;              // Posição da bala do jogador
int alienBulletX = -1, alienBulletY = -1;    // Posição da bala do invasor
int score = 0;                               // Pontuação do jogador
int invaders[INVADER_ROWS][INVADER_COLS];    // Matriz de invasores
int invaderPosX[INVADER_ROWS][INVADER_COLS]; // Posições X dos invasores
int invaderPosY[INVADER_ROWS][INVADER_COLS]; // Posições Y dos invasores
int gameState = 0;                           // 0 = menu, 1 = jogando, 2 = game over, 3 = highscore
char playerName[MAX_NAME_LENGTH + 1] = {0};  // Nome do jogador
int nameIndex = 0;                           // Índice para controle de caracteres do nome
int nameEntered = 0;                         // Flag para verificar se o nome foi inserido

// Funções de inicialização
void initGame();  // Inicializa o jogo
void resetGame(); // Reseta o jogo
void saveScore(); // Salva a pontuação

// Funções de desenho
void drawPlayer();      // Desenha o jogador
void drawInvaders();    // Desenha os invasores
void drawBullet();      // Desenha a bala do jogador
void drawAlienBullet(); // Desenha a bala do invasor
void displayScore();    // Exibe a pontuação
void gameOver();        // Exibe a tela de game over
void highScoreScreen(); // Exibe a tela de high score

// Funções de movimentação e controle
void movePlayer();        // Move o jogador
void shootBullet();       // Dispara uma bala do jogador
void updateBullet();      // Atualiza a posição da bala do jogador
void updateAlienBullet(); // Atualiza a posição da bala do invasor
void updateInvaders();    // Atualiza a posição dos invasores
void alienShoot();        // Dispara uma bala do invasor
void checkCollisions();   // Verifica colisões

// Estrutura para armazenar a bala de um alien
typedef struct AlienBullet
{
    int x;
    int y;
    int active; // 0 para inativa, 1 para ativa
} AlienBullet;

// Vetor para armazenar as balas dos aliens
AlienBullet alienBullets[MAX_ALIEN_BULLETS];

// Estrutura para armazenar um score
typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    int score;
} PlayerScore;

// Função para ordenar os scores
void sortScores(PlayerScore scores[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[j].score > scores[i].score) {
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
    // Inicializa a janela do jogo
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Invasores do espaço");
    // Inicializa as bibliotecas de tela, timer e teclado
    timerInit(1000 / 60); //60 fps
    

    // Inicializa o jogo
    initGame();

    // Loop principal do jogo
    while (!WindowShouldClose())
    {
        // Verifica se o jogador quer sair do jogo
        if (IsKeyPressed(KEY_ESCAPE))
        {
            if (gameState == 2 && nameEntered)
            {
                saveScore(); // Salva a pontuação se o jogo acabou
            }
            break; // Sai do loop do jogo
        }

        if (gameState == 0) // Menu inicial
        {
            // Desenha o menu
            BeginDrawing();
            ClearBackground(BLACK);

            // Centralize o texto
            int titleWidth = MeasureText("Invasores do Espaco", 40);
            DrawText("Invasores do Espaco", (SCREEN_WIDTH - titleWidth) / 2, SCREEN_HEIGHT / 2 - 100, 40, PURPLE);

            // Centralize o texto
            int enterWidth = MeasureText("Pressione ENTER para comecar", 20);
            DrawText("Pressione ENTER para comecar", (SCREEN_WIDTH - enterWidth) / 2, SCREEN_HEIGHT / 2, 20, WHITE);

            // Centralize o texto
            int highScoreWidth = MeasureText("Pressione H para high scores", 20);
            DrawText("Pressione H para high scores", (SCREEN_WIDTH - highScoreWidth) / 2, SCREEN_HEIGHT / 2 + 40, 20, WHITE);

            // Centralize o texto
            int escWidth = MeasureText("Pressione ESC para sair", 20);
            DrawText("Pressione ESC para sair", (SCREEN_WIDTH - escWidth) / 2, SCREEN_HEIGHT / 2 + 80, 20, WHITE);
            EndDrawing();

            // Inicia o jogo se ENTER for pressionado
            if (IsKeyPressed(KEY_ENTER))
            {
                gameState = 1;
            }
            else if (IsKeyPressed(KEY_H))
            {
                gameState = 3;
            }
        }
        else if (gameState == 1) // Jogo em andamento
        {
            if (timerTimeOver()) // Verifica se o tempo do timer acabou
            {
                screenClear(); // Limpa a tela

                // Movimentação do jogador
                if (IsKeyDown(KEY_A))
                    playerX -= PLAYER_SPEED;
                if (IsKeyDown(KEY_D))
                    playerX += PLAYER_SPEED;
                if (IsKeyPressed(KEY_W))
                    shootBullet(); // Dispara uma bala

                // Restringe o movimento do jogador aos limites da tela
                if (playerX < 0)
                    playerX = 0;
                if (playerX > SCREEN_WIDTH - 20)
                    playerX = SCREEN_WIDTH - 20;

                // Disparo do invasor aleatoriamente
                if (alienBulletY < 0 && (rand() % 50) == 0)
                {
                    alienShoot();
                }

                // Atualiza as posições dos elementos do jogo
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

                screenUpdate(); // Atualiza a tela
            }
        }
        else if (gameState == 2) // Game over
        {
            if (!nameEntered) // Solicita o nome do jogador no game over
            {
                BeginDrawing();
                ClearBackground(BLACK);

                // Centralize o texto
                int gameOverWidth = MeasureText("Game Over!", 40);
                DrawText("Game Over!", (SCREEN_WIDTH - gameOverWidth) / 2, SCREEN_HEIGHT / 2 - 100, 40, RED);

                // Centralize o texto
                int enterNameWidth = MeasureText("Digite seu nome:", 20);
                DrawText("Digite seu nome:", (SCREEN_WIDTH - enterNameWidth) / 2, SCREEN_HEIGHT / 2 - 50, 20, WHITE);

                // Centralize o texto
                int playerNameWidth = MeasureText(playerName, 20);
                DrawText(playerName, (SCREEN_WIDTH - playerNameWidth) / 2, SCREEN_HEIGHT / 2, 20, WHITE);

                // Centralize o texto
                int confirmWidth = MeasureText("Pressione ENTER para confirmar", 20);
                DrawText("Pressione ENTER para confirmar", (SCREEN_WIDTH - confirmWidth) / 2, SCREEN_HEIGHT / 2 + 40, 20, WHITE);
                EndDrawing();

                // Captura o nome do jogador
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
                // Exibe as opções de game over
                BeginDrawing();
                ClearBackground(BLACK);
                gameOver();

                // Centralize o texto
                int restartTextWidth = MeasureText("Pressione R para reiniciar", 20);
                DrawText("Pressione R para reiniciar", (SCREEN_WIDTH - restartTextWidth) / 2, SCREEN_HEIGHT / 2 + 40, 20, WHITE);

                // Centralize o texto
                int menuTextWidth = MeasureText("Pressione M para voltar ao menu", 20);
                DrawText("Pressione M para voltar ao menu", (SCREEN_WIDTH - menuTextWidth) / 2, SCREEN_HEIGHT / 2 + 80, 20, WHITE);

                // Centralize o texto
                int sairTextWidth = MeasureText("Pressione ESC para sair", 20);
                DrawText("Pressione ESC para sair", (SCREEN_WIDTH - sairTextWidth) / 2, SCREEN_HEIGHT / 2 + 120, 20, WHITE);
                EndDrawing();

                // Reinicia ou volta ao menu
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
        else if (gameState == 3) // Tela de high score
        {
            highScoreScreen();
        }
    }

    // Limpeza e encerramento
   
    timerDestroy();
    CloseWindow();
    return 0;
}


// Salva a pontuação do jogador em um arquivo
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

// Inicializa as variáveis do jogo
void initGame()
{
    playerX = SCREEN_WIDTH / 2;       // Posição inicial do jogador
    playerY = SCREEN_HEIGHT - 50;     // Posição Y do jogador
    bulletX = bulletY = -1;           // Posição inicial da bala do jogador
    alienBulletX = alienBulletY = -1; // Posição inicial da bala do invasor
    score = 0;                        // Inicializa o score

    // Inicializa as posições dos invasores
    for (int i = 0; i < INVADER_ROWS; i++)
    {
        for (int j = 0; j < INVADER_COLS; j++)
        {
            invaders[i][j] = 1;              // Marca o invasor como ativo
            invaderPosX[i][j] = 50 + j * 60; // Define a posição X do invasor
            invaderPosY[i][j] = 50 + i * 40; // Define a posição Y do invasor
        }
    }
}

// Desenha o jogador na tela
void drawPlayer()
{
    DrawText(PLAYER_SPRITE, playerX, playerY, 20, WHITE);
}

// Desenha os invasores na tela
void drawInvaders()
{
    for (int i = 0; i < INVADER_ROWS; i++)
    {
        for (int j = 0; j < INVADER_COLS; j++)
        {
            if (invaders[i][j] == 1) // Verifica se o invasor está ativo
            {
                DrawText(INVADER_SPRITE, invaderPosX[i][j], invaderPosY[i][j], 20, WHITE);
            }
        }
    }
}

// Desenha a bala do jogador na tela
void drawBullet()
{
    if (bulletY > 0)
    {
        DrawText(BULLET_SPRITE, bulletX, bulletY, 20, WHITE);
    }
}

// Desenha a bala do invasor na tela
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

// Exibe a pontuação do jogador
void displayScore()
{
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    DrawText(scoreText, 10, 10, 20, GREEN);
}

// Exibe a tela de game over
void gameOver()
{
    char message[50];
    sprintf(message, "Game Over! Score: %d", score);
    DrawText(message, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 20, RED);
}

// Exibe a tela de high score
void highScoreScreen() {
    FILE *file = fopen("score.txt", "r");
    if (file != NULL) {
        char line[100];
        PlayerScore scores[100]; // Suponha no máximo 100 scores
        int count = 0;

        // Lê as pontuações do arquivo
        while (fgets(line, sizeof(line), file) && count < 100) {
            sscanf(line, "Nome: %20[^,], Score: %d", scores[count].name, &scores[count].score);
            count++;
        }
        fclose(file);

        // Ordena as pontuações
        sortScores(scores, count);

        // Exibe os scores na tela
        BeginDrawing();
        ClearBackground(BLACK);

        int titleWidth = MeasureText("High Scores", 40);
        DrawText("High Scores", (SCREEN_WIDTH - titleWidth) / 2, 50, 40, PURPLE);

        int yOffset = 100;
        for (int i = 0; i < count && i < 5; i++) { // Exibe apenas os 5 melhores scores
            char scoreText[100];
            sprintf(scoreText, "%d. %s - %d", i + 1, scores[i].name, scores[i].score);
            DrawText(scoreText, 100, yOffset, 20, WHITE);
            yOffset += 30;
        }

        int menuTextWidth = MeasureText("Pressione M para voltar ao menu", 20);
        DrawText("Pressione M para voltar ao menu", (SCREEN_WIDTH - menuTextWidth) / 2, SCREEN_HEIGHT - 100, 20, WHITE);
        EndDrawing();

        if (IsKeyPressed(KEY_M)) {
            gameState = 0;
        }
    } else {
        printf("Erro ao abrir o arquivo de score.\n");
    }
}

// Dispara uma bala do jogador
void shootBullet()
{
    if (bulletY < 0) // Se não há uma bala ativa
    {
        bulletX = playerX + 10; // Define a posição X da bala
        bulletY = playerY - 10; // Define a posição Y da bala
    }
}

// Atualiza a posição da bala do jogador
void updateBullet()
{
    if (bulletY > 0) // Se a bala está ativa
    {
        bulletY -= BULLET_SPEED; // Move a bala para cima
    }
    else // Se a bala saiu da tela
    {
        bulletX = -1;
        bulletY = -1;
    }
}

// Atualiza a posição da bala do invasor
void updateAlienBullet()
{
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++)
    {
        if (alienBullets[i].active)
        {
            alienBullets[i].y += 5; // Velocidade da bala dos aliens
            if (alienBullets[i].y > SCREEN_HEIGHT)
            {
                alienBullets[i].active = 0; // Desativa a bala quando sai da tela
            }
        }
    }
}

// Atualiza a posição dos invasores
void updateInvaders()
{
    static int direction = INVADER_SPEED; // Direção inicial dos invasores
    static int stepDown = 0;              // Flag para mover os invasores para baixo

    int invadersLeft = 0; // Variable to track if there are any invaders left

    // Atualiza a posição dos invasores
    for (int i = 0; i < INVADER_ROWS; i++)
    {
        for (int j = 0; j < INVADER_COLS; j++)
        {
            if (invaders[i][j] == 1) // Verifica se o invasor está ativo
            {
                invadersLeft = 1;
                // Se pelo menos um invasior ainda estiver ativo

                invaderPosX[i][j] += direction;
                // Move o invasor na direção atual

                if (invaderPosY[i][j] > playerY - 20)
                // Verifica se os invasores chegaram perto do jogador
                {
                    gameState = 2;
                    // Game over se os invasores chegarem no jogador
                    return;
                }
            }
        }
    }

    if (!invadersLeft)
    {
        gameState = 3;
        // Transition to winner screen if all invaders are destroyed
    }

    // Verifica se os invasores precisam mudar de direção
    int leftMostInvader = SCREEN_WIDTH;
    int rightMostInvader = 0;

    for (int i = 0; i < INVADER_ROWS; i++) {
        for (int j = 0; j < INVADER_COLS; j++) {
            if (invaders[i][j] == 1) {
                if (invaderPosX[i][j] < leftMostInvader) {
                    leftMostInvader = invaderPosX[i][j];
                }
                if (invaderPosX[i][j] > rightMostInvader) {
                    rightMostInvader = invaderPosX[i][j];
                }
            }
        }
    }

    if (leftMostInvader <= 0 || rightMostInvader >= SCREEN_WIDTH - 20)
    {
        direction = -direction; // Inverte a direção dos invasores
        stepDown = 1;           // Sinaliza para mover os invasores para baixo
    }

    // Move os invasores para baixo se necessário
    if (stepDown)
    {
        for (int i = 0; i < INVADER_ROWS; i++)
        {
            for (int j = 0; j < INVADER_COLS; j++)
            {
                invaderPosY[i][j] += 20; // Move o invasor para baixo
            }
        }
        stepDown = 0; // Reseta a flag
    }
}

// Dispara uma bala de um invasor
void alienShoot()
{
    // Tente disparar de qualquer alien aleatoriamente
    for (int i = 0; i < INVADER_ROWS; i++)
    {
        for (int j = 0; j < INVADER_COLS; j++)
        {
            if (invaders[i][j] == 1)
            {                                     // Verifique se o alien está vivo
                int index = i * INVADER_COLS + j; // Index para a bala do alien
                if (alienBullets[index].active == 0 && (rand() % 100) < 2)
                {                                                   // Tiro aleatório
                    alienBullets[index].x = invaderPosX[i][j] + 10; // Posição X
                    alienBullets[index].y = invaderPosY[i][j] + 20; // Posição Y
                    alienBullets[index].active = 1;                 // Ativa a bala
                }
            }
        }
    }
}

// Verifica colisões entre balas e invasores/jogador
void checkCollisions()
{
    // Verifica colisões entre a bala do jogador e os invasores
    for (int i = 0; i < INVADER_ROWS; i++)
    {
        for (int j = 0; j < INVADER_COLS; j++)
        {
            if (invaders[i][j] == 1)
            { // Se o invasor está ativo
                if (bulletX >= invaderPosX[i][j] && bulletX <= invaderPosX[i][j] + 10 &&
                    bulletY >= invaderPosY[i][j] && bulletY <= invaderPosY[i][j] + 10)
                {                           // Se há colisão
                    invaders[i][j] = 0;     // Desativa o invasor
                    bulletX = bulletY = -1; // Reseta a bala do jogador
                    score += 10;            // Aumenta a pontuação
                }
            }
        }
    }

    // Verifica colisões entre os tiros dos invasores e o jogador
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++)
    {
        if (alienBullets[i].active)
        { // Se a bala do invasor está ativa
            if (alienBullets[i].x > playerX && alienBullets[i].x < playerX + 20 &&
                alienBullets[i].y > playerY && alienBullets[i].y < playerY + 20)
            {                  // Se há colisão
                gameState = 2; // Game Over
            }
        }
    }
}

// Reseta o jogo para o estado inicial
void resetGame()
{
    score = 0;                        // Reseta a pontuação
    bulletX = bulletY = -1;           // Reseta a bala do jogador
    alienBulletX = alienBulletY = -1; // Reseta a bala do invasor
    initGame();                       // Inicializa o jogo
}
