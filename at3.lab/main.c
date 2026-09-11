#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600
#define RAIO_JOGADOR 20.0f
#define TOTAL_ITENS 10

typedef enum {
    ITEM_ARMA,
    ITEM_POCAO,
    ITEM_ESCUDO
} TipoItem;

typedef struct {
    float dano;
    int alcance;
} DadosArma;

typedef struct {
    int cura;
} DadosPocao;

typedef struct {
    int absorcao;
} DadosEscudo;

typedef union {
    DadosArma arma;
    DadosPocao pocao;
    DadosEscudo escudo;
} DadosItem;

typedef struct {
    Vector2 pos;
    float raio;
    TipoItem tipo;
    DadosItem dados;
    bool coletado;
} Item;

typedef struct {
    Vector2 pos;
    float raio;
    int vida;
    float dano;
    int armadura;
} Jogador;

Item *criarItens(int quantidade) {

    Item *itens = (Item *)malloc(quantidade * sizeof(Item));

    if (itens == NULL)
        return NULL;

    for (int i = 0; i < quantidade; i++) {

        Item *it = itens + i;

        it->pos = (Vector2){
            GetRandomValue(30, LARGURA_JANELA - 30),
            GetRandomValue(30, ALTURA_JANELA - 30)
        };

        it->raio = 12.0f;
        it->coletado = false;

        it->tipo = (TipoItem)GetRandomValue(
            ITEM_ARMA,
            ITEM_ESCUDO
        );

        switch (it->tipo) {

            case ITEM_ARMA:

                it->dados.arma.dano =
                    (float)GetRandomValue(2, 8);

                it->dados.arma.alcance =
                    GetRandomValue(1, 3);

                break;

            case ITEM_POCAO: {

                int chanceVeneno =
                    GetRandomValue(0, 9);

                if (chanceVeneno < 3) {

                    it->dados.pocao.cura =
                        -GetRandomValue(10, 30);

                } else {

                    it->dados.pocao.cura =
                        GetRandomValue(10, 30);
                }

                break;
            }

            case ITEM_ESCUDO:

                it->dados.escudo.absorcao =
                    GetRandomValue(5, 20);

                break;
        }
    }

    return itens;
}

void aplicarItem(Jogador *j, Item *item) {

    switch (item->tipo) {

        case ITEM_ARMA:

            j->dano += item->dados.arma.dano;

            break;

        case ITEM_POCAO:

            j->vida += item->dados.pocao.cura;

            if (j->vida < 0)
                j->vida = 0;

            break;

        case ITEM_ESCUDO:

            j->armadura +=
                item->dados.escudo.absorcao;

            break;
    }

    item->coletado = true;
}

bool colidiu(
    Vector2 a,
    float raioA,
    Vector2 b,
    float raioB
) {

    float dx = a.x - b.x;
    float dy = a.y - b.y;

    float distancia2 =
        dx * dx + dy * dy;

    float somaRaios2 =
        (raioA + raioB) *
        (raioA + raioB);

    return distancia2 <= somaRaios2;
}

void desenharItem(Item *item) {

    if (item->coletado)
        return;

    Color cor = GRAY;

    switch (item->tipo) {

        case ITEM_ARMA:

            cor = RED;

            break;

        case ITEM_POCAO:

            if (item->dados.pocao.cura < 0)
                cor = PURPLE;
            else
                cor = GREEN;

            break;

        case ITEM_ESCUDO:

            cor = BLUE;

            break;
    }

    DrawCircleV(
        item->pos,
        item->raio,
        cor
    );
}

int main(void) {

    srand((unsigned int)time(NULL));

    InitWindow(
        LARGURA_JANELA,
        ALTURA_JANELA,
        "Atividade 3 - Union + Enum + Struct"
    );

    SetTargetFPS(60);

    Jogador jogador = {
        {
            LARGURA_JANELA / 2.0f,
            ALTURA_JANELA / 2.0f
        },
        RAIO_JOGADOR,
        100,
        5.0f,
        0
    };

    Item *itens =
        criarItens(TOTAL_ITENS);

    if (itens == NULL) {

        CloseWindow();

        return 1;
    }

    while (!WindowShouldClose()) {

        float vel =
            250.0f * GetFrameTime();

        if (IsKeyDown(KEY_RIGHT))
            jogador.pos.x += vel;

        if (IsKeyDown(KEY_LEFT))
            jogador.pos.x -= vel;

        if (IsKeyDown(KEY_UP))
            jogador.pos.y -= vel;

        if (IsKeyDown(KEY_DOWN))
            jogador.pos.y += vel;

        if (jogador.pos.x < RAIO_JOGADOR)
            jogador.pos.x = RAIO_JOGADOR;

        if (jogador.pos.x >
            LARGURA_JANELA - RAIO_JOGADOR)
            jogador.pos.x =
                LARGURA_JANELA - RAIO_JOGADOR;

        if (jogador.pos.y < RAIO_JOGADOR)
            jogador.pos.y = RAIO_JOGADOR;

        if (jogador.pos.y >
            ALTURA_JANELA - RAIO_JOGADOR)
            jogador.pos.y =
                ALTURA_JANELA - RAIO_JOGADOR;

        for (int i = 0;
             i < TOTAL_ITENS;
             i++) {

            Item *it = itens + i;

            if (!it->coletado &&
                colidiu(
                    jogador.pos,
                    jogador.raio,
                    it->pos,
                    it->raio
                )) {

                aplicarItem(
                    &jogador,
                    it
                );
            }
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (int i = 0;
             i < TOTAL_ITENS;
             i++) {

            desenharItem(itens + i);
        }

        DrawCircleV(
            jogador.pos,
            jogador.raio,
            DARKBLUE
        );

        DrawText(
            TextFormat(
                "Vida: %d",
                jogador.vida
            ),
            10,
            10,
            22,
            BLACK
        );

        DrawText(
            TextFormat(
                "Dano: %.1f",
                jogador.dano
            ),
            10,
            40,
            22,
            BLACK
        );

        DrawText(
            TextFormat(
                "Armadura: %d",
                jogador.armadura
            ),
            10,
            70,
            22,
            BLACK
        );

        DrawText(
            "Vermelho = Arma",
            10,
            110,
            18,
            RED
        );

        DrawText(
            "Verde = Pocao",
            10,
            135,
            18,
            GREEN
        );

        DrawText(
            "Roxo = Pocao Envenenada",
            10,
            160,
            18,
            PURPLE
        );

        DrawText(
            "Azul = Escudo",
            10,
            185,
            18,
            BLUE
        );

        DrawText(
            "Setas movem o jogador",
            10,
            ALTURA_JANELA - 25,
            18,
            GRAY
        );

        EndDrawing();
    }

    free(itens);

    CloseWindow();

    return 0;
}