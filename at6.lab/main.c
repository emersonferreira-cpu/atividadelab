
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define LARGURA_JANELA  800
#define ALTURA_JANELA   600
#define RAIO_JOGADOR    20.0f
#define MAX_ENTIDADES   30
#define TOTAL_INIMIGOS  5
#define TOTAL_ITENS     6
#define TAM_NOME        16          /* 15 caracteres + '\0' */
#define ARQUIVO_PLACAR  "placar.txt"
#define ARQUIVO_SAVE    "save.bin"

typedef enum {
    ENTIDADE_JOGADOR,
    ENTIDADE_INIMIGO,
    ENTIDADE_ITEM
} TipoEntidade;

typedef union {
    int dano;
    int valor;
} ExtraEntidade;

typedef struct {
    TipoEntidade  tipo;
    Vector2       pos;
    float         raio;
    int           vida;
    Color         cor;
    ExtraEntidade extra;
} Entidade;

Entidade *vetorEntidades[MAX_ENTIDADES];
int totalEntidades = 0;

Entidade *criarEntidade(TipoEntidade tipo, Vector2 pos) {
    Entidade *e = (Entidade *)malloc(sizeof(Entidade));
    if (e == NULL) return NULL;

    e->tipo  = tipo;
    e->pos   = pos;
    e->raio  = (tipo == ENTIDADE_JOGADOR) ? RAIO_JOGADOR
             : (tipo == ENTIDADE_INIMIGO) ? 15.0f : 8.0f;

    switch (tipo) {
        case ENTIDADE_JOGADOR:
            e->vida = 100;
            e->cor  = BLUE;
            break;
        case ENTIDADE_INIMIGO:
            e->vida       = 40;
            e->cor        = MAROON;
            e->extra.dano = GetRandomValue(5, 15);
            break;
        case ENTIDADE_ITEM:
            e->vida        = 1;
            e->cor         = GOLD;
            e->extra.valor = GetRandomValue(5, 20);
            break;
    }
    return e;
}

void adicionarEntidade(Entidade *e) {
    if (e == NULL || totalEntidades >= MAX_ENTIDADES) return;
    vetorEntidades[totalEntidades] = e;
    totalEntidades++;
}

void removerEntidade(int indice) {
    if (indice < 0 || indice >= totalEntidades) return;
    free(vetorEntidades[indice]);
    vetorEntidades[indice] = vetorEntidades[totalEntidades - 1];
    totalEntidades--;
}

void liberarTodasEntidades(void) {
    for (int i = 0; i < totalEntidades; i++) free(vetorEntidades[i]);
    totalEntidades = 0;
}

bool colidiu(Entidade *a, Entidade *b) {
    float dx = a->pos.x - b->pos.x;
    float dy = a->pos.y - b->pos.y;
    float distancia = sqrtf(dx * dx + dy * dy);
    return distancia <= (a->raio + b->raio);
}

void desenharEntidade(Entidade *e) {
    DrawCircleV(e->pos, e->raio, e->cor);
    if (e->tipo == ENTIDADE_INIMIGO) {
        DrawText(TextFormat("%d", e->vida), e->pos.x - 8, e->pos.y - 26, 14, BLACK);
    }
}

/* ---- Exercício 1: pede o nome do jogador na tela inicial ----
 * Devolve false se a janela foi fechada durante a digitação.
 * Espaços são ignorados, para que o fscanf("%15s") leia o nome inteiro. */
bool pedirNomeJogador(char *nome) {
    int tamanho = 0;
    nome[0] = '\0';

    while (!WindowShouldClose()) {
        int tecla = GetCharPressed();
        while (tecla > 0) {
            if (tecla > 32 && tecla < 127 && tamanho < TAM_NOME - 1) {
                nome[tamanho++] = (char)tecla;
                nome[tamanho] = '\0';
            }
            tecla = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && tamanho > 0) {
            tamanho--;
            nome[tamanho] = '\0';
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (tamanho == 0) TextCopy(nome, "Jogador"); /* nome padrão */
            return true;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Digite seu nome (sem espacos) e tecle ENTER:", 150, 220, 22, DARKGRAY);
            DrawRectangle(250, 270, 300, 40, LIGHTGRAY);
            DrawRectangleLines(250, 270, 300, 40, DARKGRAY);
            DrawText(nome, 260, 278, 24, BLACK);
            DrawText(TextFormat("%d/%d", tamanho, TAM_NOME - 1), 250, 318, 16, GRAY);
        EndDrawing();
    }
    return false;
}

/* ---- arquivo de TEXTO: histórico "nome pontuacao" (fprintf/fscanf) ---- */
void salvarPlacarTexto(const char *nome, int pontuacao) {
    FILE *arquivo = fopen(ARQUIVO_PLACAR, "a"); // "a": anexa ao final, modo texto
    if (arquivo == NULL) return;

    fprintf(arquivo, "%s %d\n", nome, pontuacao); // Exercício 1: nome + pontuação
    fclose(arquivo);
}

/* lê o arquivo de texto par a par (nome, valor); devolve a maior pontuação
 * e copia para nomeMelhor o nome de quem a fez */
int lerMelhorPontuacao(char *nomeMelhor) {
    nomeMelhor[0] = '\0';

    FILE *arquivo = fopen(ARQUIVO_PLACAR, "r"); // "r": leitura, modo texto
    if (arquivo == NULL) return 0;

    char nomeLido[TAM_NOME];
    int melhor = 0, valor = 0;
    while (fscanf(arquivo, "%15s %d", nomeLido, &valor) == 2) {
        if (valor > melhor) {
            melhor = valor;
            TextCopy(nomeMelhor, nomeLido);
        }
    }
    fclose(arquivo);
    return melhor;
}

/* ---- arquivo BINÁRIO: estado completo do jogo (fwrite/fread) ---- */
bool salvarJogoBinario(void) {
    FILE *arquivo = fopen(ARQUIVO_SAVE, "wb"); // "wb": escrita, modo binário
    if (arquivo == NULL) return false;

    fwrite(&totalEntidades, sizeof(int), 1, arquivo);
    for (int i = 0; i < totalEntidades; i++) {
        fwrite(vetorEntidades[i], sizeof(Entidade), 1, arquivo); // grava a struct inteira, byte a byte
    }

    fclose(arquivo);
    return true;
}

/* recria o vetor de ponteiros a partir dos bytes gravados no arquivo binário */
bool carregarJogoBinario(void) {
    FILE *arquivo = fopen(ARQUIVO_SAVE, "rb"); // "rb": leitura, modo binário
    if (arquivo == NULL) return false;

    int totalSalvo = 0;
    if (fread(&totalSalvo, sizeof(int), 1, arquivo) != 1) {
        fclose(arquivo);
        return false;
    }

    liberarTodasEntidades();
    for (int i = 0; i < totalSalvo; i++) {
        Entidade *e = (Entidade *)malloc(sizeof(Entidade));
        if (e == NULL) break;
        if (fread(e, sizeof(Entidade), 1, arquivo) != 1) {
            free(e);
            break;
        }
        adicionarEntidade(e);
    }

    fclose(arquivo);
    return true;
}

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Atividade 6 - Manipulacao de Arquivos (texto e binario)");
    SetTargetFPS(60);

    /* Exercício 1: nome pedido antes de iniciar a partida */
    char nomeJogador[TAM_NOME];
    if (!pedirNomeJogador(nomeJogador)) {
        CloseWindow();
        return 0;
    }

    Entidade *jogador = criarEntidade(ENTIDADE_JOGADOR,
                                      (Vector2){ LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f });
    adicionarEntidade(jogador);

    for (int i = 0; i < TOTAL_INIMIGOS; i++) {
        Vector2 pos = { GetRandomValue(30, LARGURA_JANELA - 30), GetRandomValue(30, ALTURA_JANELA - 30) };
        adicionarEntidade(criarEntidade(ENTIDADE_INIMIGO, pos));
    }
    for (int i = 0; i < TOTAL_ITENS; i++) {
        Vector2 pos = { GetRandomValue(30, LARGURA_JANELA - 30), GetRandomValue(30, ALTURA_JANELA - 30) };
        adicionarEntidade(criarEntidade(ENTIDADE_ITEM, pos));
    }

    int pontuacao = 0;
    char nomeRecordista[TAM_NOME];
    int melhorPontuacao = lerMelhorPontuacao(nomeRecordista); // recorde salvo em placar.txt, se existir
    char mensagem[64] = "";
    float tempoMensagem = 0.0f;

    while (!WindowShouldClose()) {

        float vel = 250.0f * GetFrameTime();
        if (IsKeyDown(KEY_RIGHT)) jogador->pos.x += vel;
        if (IsKeyDown(KEY_LEFT))  jogador->pos.x -= vel;
        if (IsKeyDown(KEY_UP))    jogador->pos.y -= vel;
        if (IsKeyDown(KEY_DOWN))  jogador->pos.y += vel;

        for (int i = 1; i < totalEntidades; i++) {
            Entidade *e = vetorEntidades[i];
            if (!colidiu(jogador, e)) continue;

            if (e->tipo == ENTIDADE_ITEM) {
                pontuacao += e->extra.valor;
                removerEntidade(i);
                i--;
            } else if (e->tipo == ENTIDADE_INIMIGO) {
                jogador->vida -= e->extra.dano;
                if (jogador->vida < 0) jogador->vida = 0;
            }
        }

        if (IsKeyPressed(KEY_F5)) { // salva nome + pontuação no arquivo de texto
            salvarPlacarTexto(nomeJogador, pontuacao);
            if (pontuacao > melhorPontuacao) {
                melhorPontuacao = pontuacao;
                TextCopy(nomeRecordista, nomeJogador);
            }
            TextCopy(mensagem, "Placar salvo em placar.txt!");
            tempoMensagem = 2.0f;
        }

        if (IsKeyPressed(KEY_F6)) { // salva o estado do jogo no arquivo binário
            bool ok = salvarJogoBinario();
            TextCopy(mensagem, ok ? "Jogo salvo em save.bin!" : "Erro ao salvar save.bin!");
            tempoMensagem = 2.0f;
        }

        if (IsKeyPressed(KEY_F9)) { // carrega o estado do jogo do arquivo binário
            bool ok = carregarJogoBinario();
            if (ok) jogador = vetorEntidades[0]; // ponteiros antigos foram liberados: reaponta para o novo jogador
            TextCopy(mensagem, ok ? "Jogo carregado de save.bin!" : "Nenhum save.bin encontrado!");
            tempoMensagem = 2.0f;
        }

        if (IsKeyPressed(KEY_DELETE)) { // Exercício 2: apaga o arquivo save.bin
            if (remove(ARQUIVO_SAVE) == 0) {
                TextCopy(mensagem, "save.bin apagado com sucesso!");
            } else {
                TextCopy(mensagem, "Nenhum save encontrado");
            }
            tempoMensagem = 2.0f;
        }

        if (tempoMensagem > 0.0f) tempoMensagem -= GetFrameTime();

        BeginDrawing();
            ClearBackground(RAYWHITE);

            for (int i = 0; i < totalEntidades; i++) {
                desenharEntidade(vetorEntidades[i]);
            }

            DrawText(TextFormat("%s | Vida: %d   Pontuacao: %d   Recorde: %d%s%s%s",
                                 nomeJogador, jogador->vida, pontuacao, melhorPontuacao,
                                 nomeRecordista[0] ? " (" : "", nomeRecordista,
                                 nomeRecordista[0] ? ")" : ""),
                     10, 10, 22, DARKGRAY);
            DrawText("F5 salva placar | F6 salva jogo | F9 carrega jogo | DELETE apaga save",
                      10, 34, 18, GRAY);
            DrawText("Setas movem o jogador | ESC sai", 10, ALTURA_JANELA - 25, 16, GRAY);

            if (tempoMensagem > 0.0f) {
                DrawText(mensagem, 10, 58, 20, DARKGREEN);
            }

        EndDrawing();
    }

    liberarTodasEntidades();

    CloseWindow();
    return 0;
}