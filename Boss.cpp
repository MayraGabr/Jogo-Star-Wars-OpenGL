#include "Boss.h"
#include "Camera.h"

#include <GL/glut.h>
#include <cmath>

Boss::Boss() {
    bossX = 0.0f;
    bossY = -1.25f;
    bossZ = -40.0f;

    rotY = 0.0f;
    vida = 500;
    vivo = false;
    ultimoDano = std::chrono::steady_clock::now();
}

void Boss::nascer() {
    vivo = true;
    vida = 500;

    bossX = -74.95f;
    bossY = -1.25f;
    bossZ = -1.0f;

    ultimoDano = std::chrono::steady_clock::now();
}

void Boss::resetar() {
    bossX = 0.0f;
    bossY = -0.05f;
    bossZ = -40.0f;

    rotY = 0.0f;
    vida = 500;
    vivo = false;
    ultimoDano = std::chrono::steady_clock::now();
}

void Boss::carregar() {
    modelo.carregar("data/scene.gltf");
}

void Boss::desenhar() {
    if (!vivo) return;

    glPushMatrix();

    glTranslatef(bossX, bossY, bossZ);


    glRotatef(180.0f, 1, 0, 0);
    glRotatef(90.0f, 1, 0, 0);   
    glRotatef(rotY, 0, 1, 0);   

    glScalef(1.0f, 1.0f, 1.0f);

    modelo.desenhar();
    
    glPopMatrix();
}

void Boss::aplicarDanoPorSegundo(){
    if (!vivo) return;

    auto agora = std::chrono::steady_clock::now();

    auto tempo = std::chrono::duration_cast<std::chrono::milliseconds>(
        agora - ultimoDano
    ).count();

    if (tempo >= 1000) {
        aplicarDanoJogador(5); // dano por segundo //mayra
        ultimoDano = agora;
    }
}

void Boss::tomarDano(int dano) {
    if (!vivo) return;

    vida -= dano;

    if (vida <= 0) {
        vida = 0;
        vivo = false;
    }
}

bool Boss::estarVivo() const {
    return vivo;
}

float Boss::getX() const {
    return bossX;
}

float Boss::getZ() const {
    return bossZ;
}

int Boss::getVida() const {
    return vida;
}