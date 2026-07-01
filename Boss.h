#ifndef BOSS_H
#define BOSS_H

#include <chrono>
#include "ModeloBoss.h"

class Boss {
private:
    float bossX, bossY, bossZ;
    float rotY;
    int vida;
    bool vivo;

    std::chrono::steady_clock::time_point ultimoDano;

    ModeloBoss modelo;

public:
    Boss();

    void carregar();
    void nascer(float x, float y, float z);
    void atualizar(float camX, float camZ);
    void desenhar();
    void aplicarDanoPorSegundo();

    void tomarDano(int dano);

    bool estarVivo() const;
    void nascer();
    float getX() const;
    float getZ() const;
    int getVida() const;
    void resetar();
};

#endif