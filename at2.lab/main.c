#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600
#define RAIO_JOGADOR 20.0f
#define TOTAL_MOEDAS 15

typedef enum {
    MOEDA_BRONZE,
    MOEDA_PRATA,
    MOEDA_OURO,
    MOEDA_DIAMANTE
} TipoMoeda;

typedef struct {
    Vector2 pos;
    float raio;
    TipoMoeda tipo;
    int valor;
    bool coletada;
    float tempoColeta;
} Moeda;

Color corDaMoeda(TipoMoeda tipo) {
    switch (tipo) {
        case MOEDA_BRONZE:
            return (Color){160, 90, 40, 255};

        case MOEDA_PRATA:
            return (Color){190, 190, 190, 255};

        case MOEDA_OURO:
            return GOLD;

        case MOEDA_DIAMANTE:
            return SKYBLUE;

        default:
            return WHITE;
    }
}

int valorDaMoeda(TipoMoeda tipo) {
    switch (tipo) {
        case MOEDA_BRONZE:
            return 5;

        case MOEDA_PRATA:
            return 10;

        case MOEDA_OURO:
            return 25;

        case MOEDA_DIAMANTE:
            return 50;

        default:
            return 0;
    }
}

TipoMoeda sortearTipoMoeda(void) {

    int chanceDiamante = GetRandomValue(0, 9);

    if (chanceDiamante == 0) {
        return MOEDA_DIAMANTE;
    }

    return (TipoMoeda)GetRandomValue(MOEDA_BRONZE, MOEDA_OURO);
}

Moeda *criarMoedas(int quantidade) {

    Moeda *moedas = (Moeda *)malloc(quantidade * sizeof(Moeda));

    if (moedas == NULL)
        return NULL;

    for (int i = 0; i < quantidade; i++) {

        Moeda *m = moedas + i;

        m->pos = (Vector2){
            GetRandomValue(30, LARGURA_JANELA - 30),
            GetRandomValue(30, ALTURA_JANELA - 30)
        };

        m->raio = 10.0f;
        m->tipo = sortearTipoMoeda();
        m->valor = valorDaMoeda(m->tipo);
        m->coletada = false;
        m->tempoColeta = 0.0f;
    }

    return moedas;
}

bool tentarColetar(Moeda *m, Vector2 posJogador, float raioJogador) {

    if (m->coletada)
        return false;

    float dx = m->pos.x - posJogador.x;
    float dy = m->pos.y - posJogador.y;

    float distancia = dx * dx + dy * dy;

    float somaRaios =
        (m->raio + raioJogador) *
        (m->raio + raioJogador);

    if (distancia <= somaRaios) {

        m->coletada = true;
        m->tempoColeta = (float)GetTime();

        return true;
    }

    return false;
}

void atualizarMoeda(Moeda *m) {

    if (!m->coletada)
        return;

    if ((float)GetTime() - m->tempoColeta >= 3.0f) {

        m->pos = (Vector2){
            GetRandomValue(30, LARGURA_JANELA - 30),
            GetRandomValue(30, ALTURA_JANELA - 30)
        };

        m->tipo = sortearTipoMoeda();
        m->valor = valorDaMoeda(m->tipo);

        m->coletada = false;
    }
}

void desenharMoeda(Moeda *m) {

    if (m->coletada)
        return;

    DrawCircleV(m->pos, m->raio, corDaMoeda(m->tipo));

    switch (m->tipo) {

        case MOEDA_BRONZE:
            DrawText("B",
                     (int)m->pos.x - 4,
                     (int)m->pos.y - 8,
                     12,
                     BLACK);
            break;

        case MOEDA_PRATA:
            DrawText("P",
                     (int)m->pos.x - 4,
                     (int)m->pos.y - 8,
                     12,
                     BLACK);
            break;

        case MOEDA_OURO:
            DrawText("O",
                     (int)m->pos.x - 4,
                     (int)m->pos.y - 8,
                     12,
                     BLACK);
            break;

        case MOEDA_DIAMANTE:
            DrawText("D",
                     (int)m->pos.x - 4,
                     (int)m->pos.y - 8,
                     12,
                     BLACK);
            break;
    }
}

int main(void) {

    srand((unsigned int)time(NULL));

    InitWindow(
        LARGURA_JANELA,
        ALTURA_JANELA,
        "Atividade 2 - Exercicios Resolvidos"
    );

    SetTargetFPS(60);

    Vector2 jogador = {
        LARGURA_JANELA / 2.0f,
        ALTURA_JANELA / 2.0f
    };

    int pontuacao = 0;

    Moeda *moedas = criarMoedas(TOTAL_MOEDAS);

    if (moedas == NULL) {
        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose()) {

        float vel = 250.0f * GetFrameTime();

        if (IsKeyDown(KEY_RIGHT))
            jogador.x += vel;

        if (IsKeyDown(KEY_LEFT))
            jogador.x -= vel;

        if (IsKeyDown(KEY_UP))
            jogador.y -= vel;

        if (IsKeyDown(KEY_DOWN))
            jogador.y += vel;

        if (jogador.x < RAIO_JOGADOR)
            jogador.x = RAIO_JOGADOR;

        if (jogador.x > LARGURA_JANELA - RAIO_JOGADOR)
            jogador.x = LARGURA_JANELA - RAIO_JOGADOR;

        if (jogador.y < RAIO_JOGADOR)
            jogador.y = RAIO_JOGADOR;

        if (jogador.y > ALTURA_JANELA - RAIO_JOGADOR)
            jogador.y = ALTURA_JANELA - RAIO_JOGADOR;

        for (int i = 0; i < TOTAL_MOEDAS; i++) {

            Moeda *m = moedas + i;

            atualizarMoeda(m);

            if (tentarColetar(m, jogador, RAIO_JOGADOR)) {
                pontuacao += m->valor;
            }
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (int i = 0; i < TOTAL_MOEDAS; i++) {
            desenharMoeda(moedas + i);
        }

        DrawCircleV(jogador, RAIO_JOGADOR, BLUE);

        DrawText(
            TextFormat("Pontuacao: %d", pontuacao),
            10,
            10,
            25,
            DARKBLUE
        );

        DrawText(
            "Bronze=5  Prata=10  Ouro=25  Diamante=50",
            10,
            45,
            18,
            BLACK
        );

        DrawText(
            "Setas movimentam o jogador",
            10,
            ALTURA_JANELA - 25,
            18,
            GRAY
        );

        EndDrawing();
    }

    free(moedas);

    CloseWindow();

    return 0;
}