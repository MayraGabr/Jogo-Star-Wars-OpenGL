
#include "Spawn.h"
#include "Inimigo.h"
#include "Colisao.h"
#include "Boss.h"
#include "Camera.h"

#include <cstdlib>
#include <cmath>
#include <vector>
#include <chrono>

extern std::vector<Inimigo> inimigos;
extern Boss boss;

extern float camX;
extern float camZ;
extern float camXOriginal;
extern float camZOriginal;

extern bool modoBoss;
extern std::chrono::steady_clock::time_point ultimoDanoBoss;

static bool spawnCorredorInicialCriado = false;
static bool spawnSalaMeioCriado = false;
static bool spawnCorredorFinal1Criado = false;
static bool spawnCorredorFinal2Criado = false;
static bool spawnSalaFinalCriado = false;

static bool spawnCorredorInicial1DuplicadoCriado = false;
static bool spawnCorredorInicial2DuplicadoCriado = false;
static bool spawnSalaMeioDuplicadaCriado = false;
static bool spawnCorredorFinalDuplicadoCriado = false;
static bool spawnSalaFinalDuplicadaCriado = false;

static bool bossCriado = false;

float numeroAleatorio(float min, float max) {
    return min + (float)rand() / RAND_MAX * (max - min);
}

void gerarPosicaoInimigo(float& x, float& z, LocalSpawn local) {

    switch (local) {

        case CorredorInicial:
            x = numeroAleatorio(-7.0f, 1.0f);
            z = numeroAleatorio(-1.35f, 1.60f);
            break;

        case SalaMeio:
            x = numeroAleatorio(-12.0f, -8.0f);
            z = numeroAleatorio(-4.50f, 3.80f);
            break;

        case CorredorFinal1:
            x = numeroAleatorio(-18.0f, -12.0f);
            z = numeroAleatorio(-1.35f, 1.60f);
            break;

        case CorredorFinal2:
            x = numeroAleatorio(-28.0f, -20.0f);
            z = numeroAleatorio(-1.35f, 1.60f);
            break;

        case SalaFinal:
            x = numeroAleatorio(-32.5f, -30.0f);
            z = numeroAleatorio(-5.50f, 3.90f);
            break;

        case CorredorInicial1Duplicado:
            x = numeroAleatorio(-42.0f,-35.0f);
            z = numeroAleatorio(-1.35f, 1.60f);
            break;

        case CorredorInicial2Duplicado:
            x = numeroAleatorio(-49.0f, -42.0f);
            z = numeroAleatorio(-1.35f, 1.60f);
            break;

        case SalaMeioDuplicada:
            x = numeroAleatorio(-56.0f, -51.0f);
            z = numeroAleatorio(-4.50f, 3.70f);
            break;

        case CorredorFinalDuplicado:
            x = numeroAleatorio(-64.0f, -57.0f);
            z = numeroAleatorio(-2.0f, -0.05f);
            break;

        case SalaFinalDuplicada:
            x = numeroAleatorio(-77.0f, -72.0f);
            z = numeroAleatorio(-4.5f, 2.5f);
            break;
    }
}

bool posicaoLivre(float x, float z) {
    const float distanciaMinima = 2.2f;

    for (const auto& inimigo : inimigos) {
        if (!inimigo.estarVivo()) continue;

        float dx = x - inimigo.getX();
        float dz = z - inimigo.getZ();

        float distancia = sqrt(dx * dx + dz * dz);

        if (distancia < distanciaMinima) {
            return false;
        }
    }

    return true;
}

bool todosInimigosMortos() {
    for (const auto& inimigo : inimigos) {
        if (inimigo.estarVivo()) {
            return false;
        }
    }

    return true;
}

void criarInimigoAleatorio(LocalSpawn local) {
    Inimigo inimigo;

    float x, z;
    int tentativas = 0;

    do {
        gerarPosicaoInimigo(x, z, local);
        tentativas++;
    } while ((!podeMoverPara(x, z) || !posicaoLivre(x, z)) && tentativas < 100);

    inimigo.setPosicao(x, -1.25f, z);
    inimigos.push_back(inimigo);
}

void spawnarGrupo(LocalSpawn local, int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        criarInimigoAleatorio(local);
    }
}

void verificarSpawnsPorMapa() {
    if (!spawnCorredorInicialCriado) {
        spawnarGrupo(CorredorInicial, 3);
        spawnCorredorInicialCriado = true;
    }

    if (!spawnSalaMeioCriado && camX <= -2.0f) {
        spawnarGrupo(SalaMeio, 2;
        spawnSalaMeioCriado = true;
    }

    if (!spawnCorredorFinal1Criado && camX <= -8.0f) {
        spawnarGrupo(CorredorFinal1, 3);
        spawnCorredorFinal1Criado = true;
    }

    if (!spawnCorredorFinal2Criado && camX <= -15.0f) {
        spawnarGrupo(CorredorFinal2, 3);
        spawnCorredorFinal2Criado = true;
    }

    if (!spawnSalaFinalCriado && camX <= -26.0f) {
        spawnarGrupo(SalaFinal, 3;
        spawnSalaFinalCriado = true;
    }

    if (!spawnCorredorInicial1DuplicadoCriado && camX <= -29.0f) {
        spawnarGrupo(CorredorInicial1Duplicado, 3);
        spawnCorredorInicial1DuplicadoCriado = true;
    }

    if (!spawnCorredorInicial2DuplicadoCriado && camX <= -36.0f) {
        spawnarGrupo(CorredorInicial2Duplicado, 3);
        spawnCorredorInicial2DuplicadoCriado = true;
    }

    if (!spawnSalaMeioDuplicadaCriado && camX <= -46.0f) {
        spawnarGrupo(SalaMeioDuplicada, 3);
        spawnSalaMeioDuplicadaCriado = true;
    }

    if (!spawnCorredorFinalDuplicadoCriado && camX <= -52.0f) {
        spawnarGrupo(CorredorFinalDuplicado, 3);
        spawnCorredorFinalDuplicadoCriado = true;
    }

    if (!bossCriado && camX <= -70.0f && todosInimigosMortos()) {
        camXOriginal = camX;
        camZOriginal = camZ;

        boss.nascer();
        bossCriado = true;
        modoBoss = true;

        ultimoDanoBoss = std::chrono::steady_clock::now();
    }
}

void resetarSpawns() {
    spawnCorredorInicialCriado = false;
    spawnSalaMeioCriado = false;
    spawnCorredorFinal1Criado = false;
    spawnCorredorFinal2Criado = false;
    spawnSalaFinalCriado = false;

    spawnCorredorInicial1DuplicadoCriado = false;
    spawnCorredorInicial2DuplicadoCriado = false;
    spawnSalaMeioDuplicadaCriado = false;
    spawnCorredorFinalDuplicadoCriado = false;
    spawnSalaFinalDuplicadaCriado = false;

    bossCriado = false;
}
