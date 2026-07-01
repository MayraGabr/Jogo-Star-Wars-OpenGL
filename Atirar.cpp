#include <GL/glut.h>
#include <cmath>
#include "Atirar.h"
#include "Mira.h"
#include "Camera.h"
#include "Inimigo.h"
#include "Boss.h"
#include "Colisao.h"

extern int troopersEliminados;

Atirar::Atirar() {
    ultimoDisparo = std::chrono::steady_clock::now() - std::chrono::milliseconds(1000);
}

void Atirar::disparar(const Mira& mira) {
    if (!mira.estaAtiva()) {
        return;
    }

    auto agora = std::chrono::steady_clock::now();
    auto desdeUltimo = std::chrono::duration_cast<std::chrono::milliseconds>(
        agora - ultimoDisparo
    ).count();
    if (desdeUltimo < 120) {
        return;
    }
    ultimoDisparo = agora;

    float radY = anguloY * 3.14159f / 180.0f;
    float radX = anguloX * 3.14159f / 180.0f;

    // Direcao 3D completa — segue exatamente onde a camera aponta (centro da mira)
    float dirX =  sinf(radY) * cosf(radX);
    float dirY =  sinf(radX);
    float dirZ = -cosf(radY) * cosf(radX);

    Tiro novoTiro;
    novoTiro.x  = camX + dirX * 0.8f;
    novoTiro.y  = camY + dirY * 0.8f;
    novoTiro.z  = camZ + dirZ * 0.8f;
    novoTiro.xAnterior = novoTiro.x;
    novoTiro.yAnterior = novoTiro.y;
    novoTiro.zAnterior = novoTiro.z;
    novoTiro.dirX = dirX;
    novoTiro.dirY = dirY;
    novoTiro.dirZ = dirZ;
    novoTiro.distancia  = 0.0f;
    novoTiro.ativo      = true;
    novoTiro.tiroInimigo = false;

    tiros.push_back(novoTiro);
}

void Atirar::dispararInimigo(float origemX, float origemY, float origemZ,
                             float alvoX, float alvoY, float alvoZ) {

    Tiro novoTiro;

    novoTiro.x = origemX - 0.70f;
    novoTiro.y = origemY + 1.20f;
    novoTiro.z = origemZ;

    // mantém o rastro completo
    novoTiro.xAnterior = novoTiro.x;
    novoTiro.yAnterior = novoTiro.y;
    novoTiro.zAnterior = novoTiro.z;

    // direção só no plano XZ
    float dx = alvoX - origemX;
    float dz = alvoZ - origemZ;

    float dist = sqrtf(dx * dx + dz * dz);
    if (dist == 0.0f) return;

    novoTiro.dirX = dx / dist;
    novoTiro.dirY = 0.0f;
    novoTiro.dirZ = dz / dist;

    novoTiro.ativo = true;
    novoTiro.tiroInimigo = true;
    novoTiro.distancia = 0.0f;

    tiros.push_back(novoTiro);
}

void Atirar::atualizarBoss(Boss& boss) {
    if (!boss.estarVivo()) return;

    for (auto& tiro : tiros) {
        if (!tiro.ativo || tiro.tiroInimigo) continue;

        float dx = tiro.x - boss.getX();
        float dz = tiro.z - boss.getZ();

        if (sqrtf(dx*dx + dz*dz) < 3.0f) {
            boss.tomarDano(20);
            tiro.ativo = false;
        }
    }
}

void Atirar::atualizar(std::vector<Inimigo>& inimigos) {
    float velocidade = 0.45f;

    for (auto& tiro : tiros) {
        if (!tiro.ativo) continue;

        tiro.xAnterior = tiro.x;
        tiro.yAnterior = tiro.y;
        tiro.zAnterior = tiro.z;

        tiro.x += tiro.dirX * velocidade;
        tiro.y += tiro.dirY * velocidade;
        tiro.z += tiro.dirZ * velocidade;
        tiro.distancia += velocidade;

        // Tiro do inimigo acerta jogador
        if (tiro.tiroInimigo) {
            float dx = tiro.x - camX;
            float dz = tiro.z - camZ;
            if (sqrtf(dx*dx + dz*dz) < 0.5f) {
                aplicarDanoJogador(2);
                tiro.ativo = false;
                continue;
            }
        }

        // Tiro do jogador — para imediatamente ao acertar qualquer inimigo
        if (!tiro.tiroInimigo) {
            bool acertou = false;
            for (auto& inimigo : inimigos) {
                if (!inimigo.estarVivo()) continue;

                if (atingiuInimigoSegmento(tiro.xAnterior, tiro.zAnterior, tiro.x, tiro.z, inimigo.getX(), inimigo.getZ())) {
                    bool estavaVivo = inimigo.estarVivo();
                    inimigo.tomarDano(20);
                    if (estavaVivo && !inimigo.estarVivo()) {
                        troopersEliminados++;
                    }
                    tiro.ativo = false;  // some imediatamente
                    acertou = true;
                    break;              // nao atravessa para o proximo
                }
            }
            if (acertou) continue;
        }

        if (tiro.distancia > 7.0f) {
            tiro.ativo = false;
        }
    }
}

void Atirar::desenhar() {
    glDisable(GL_LIGHTING);

    for (const auto& tiro : tiros) {
        if (!tiro.ativo) {
            continue;
        }

        glPushMatrix();

        glTranslatef(tiro.x, tiro.y, tiro.z);

        glLineWidth(3.0f);

        glBegin(GL_LINES);

        if (tiro.tiroInimigo) {
            glColor3f(1.0f, 0.0f, 0.0f);
        } else {
            glColor3f(0.0f, 1.0f, 0.0f);
        }

    glVertex3f(
        tiro.xAnterior - tiro.x,
        0.0f,
        tiro.zAnterior - tiro.z
    );
    glEnd();

    glVertex3f(0.0f, 0.0f, 0.0f);

        float angulo = atan2(tiro.dirX, tiro.dirZ) * 180.0f / 3.14159f;

        glRotatef(angulo, 0.0f, 1.0f, 0.0f);

        if (tiro.tiroInimigo) {
            glColor3f(0.7f, 0.0f, 0.0f);
        } else {
            glColor3f(0.0f, 0.7f, 0.0f);
        }

        glPushMatrix();
        glScalef(0.07f, 0.07f, 0.35f);
        glutSolidCube(1.0f);
        glPopMatrix();

        if (tiro.tiroInimigo) {
            glColor3f(1.0f, 0.1f, 0.1f);
        } else {
            glColor3f(0.2f, 1.0f, 0.2f);
        }

        glScalef(0.03f, 0.03f, 0.22f);
        glutSolidCube(1.0f);

        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
}

void Atirar::limparTiros() {
    tiros.clear();
}
