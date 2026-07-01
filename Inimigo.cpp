#include "Inimigo.h"
#include "ModeloAnimado.h"
#include "Colisao.h"
#include "Camera.h"
#include "Atirar.h"

#include <GL/glut.h>
#include <cmath>
#include <chrono>
#include <vector>

ModeloAnimado Inimigo::modelo;
extern Atirar atirar;

// Define funções iniciais
Inimigo::Inimigo() {
    inimigoX = 9.0f;
    inimigoY = -1.25f;
    inimigoZ = -0.25f;

    rotY = 0.0f;

    vida = 100;
    vivo = true;

    ultimoAtaque = std::chrono::steady_clock::now();
}

void Inimigo::carregarModelo(){
    modelo.carregar("data/inimigosAtt.glb");
}

void Inimigo::setPosicao(float x, float y, float z) {
    inimigoX = x;
    inimigoY = y;
    inimigoZ = z;
}

void Inimigo::atualizar(float camX, float camY, float camZ, const std::vector<Inimigo>& inimigos) {
    if (!vivo) {
        return;
    }

    modelo.atualizarAnimacao(0.02f);

    // Distância do inimigo para o jogador
    float distanciaX = camX - inimigoX;
    float distanciaZ = camZ - inimigoZ;

    float novoX = inimigoX;
    float novoZ = inimigoZ;

    float distancia =
        sqrt((distanciaX * distanciaX) +
             (distanciaZ * distanciaZ));

    // Olhando para o jogador
    rotY = atan2(distanciaX, distanciaZ) * 180.0f / 3.14159f;

    // Movendo até o jogador
    if (distancia >= 2.0f) {
        float velocidade = 0.07f;

        novoX += (distanciaX / distancia) * velocidade;
        novoZ += (distanciaZ / distancia) * velocidade;
    }

    bool muitoPerto = false;

    for (const auto& outro : inimigos) {

        if (&outro == this || !outro.estarVivo()) {
            continue;
        }

        float dx = novoX - outro.getX();
        float dz = novoZ - outro.getZ();

        float d = sqrt(dx * dx + dz * dz);

        if (d < 1.6f) {
            muitoPerto = true;
            break;
        }
    }

    inimigoX = novoX;
    inimigoZ = novoZ;
}

void Inimigo::desenhar() {
    if (!vivo) return;

    glPushMatrix();

    glTranslatef(inimigoX, inimigoY, inimigoZ);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    glScalef(0.2f, 0.2f, 0.2f);

    modelo.desenhar();

    glPopMatrix();
}

void Inimigo::atacarJogador(float camX, float camZ) {
    if (!vivo) {
        return;
    }

    // Distância até o jogador
    float dx = inimigoX - camX;
    float dz = inimigoZ - camZ;

    float distancia = sqrt(dx * dx + dz * dz);

    // Distância máxima para atacar
    const float DISTANCIA_ATAQUE = 6.5f;

    if (distancia > DISTANCIA_ATAQUE) {
        return;
    }

    auto agora = std::chrono::steady_clock::now();

    long long tempoPassado =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            agora - ultimoAtaque
        ).count();

    // Ataca apenas uma vez por segundo
    if (tempoPassado >= 1000) {

        atirar.dispararInimigo(
            inimigoX,
            inimigoY,
            inimigoZ,
            camX,
            0.0f,
            camZ
        );

        ultimoAtaque = agora;
    }
}

void Inimigo::tomarDano(int dano) {
    vida -= dano;

    if (vida <= 0) {
        vivo = false;
    }
}

bool Inimigo::estarVivo() const{
    return vida > 0;
}

float Inimigo::getX() const {
    return inimigoX;
}

float Inimigo::getZ() const {
    return inimigoZ;
}

int Inimigo::getVida() const {
    return vida;
}