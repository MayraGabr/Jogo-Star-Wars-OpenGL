#ifndef ATIRAR_H
#define ATIRAR_H

#include <vector>
#include <chrono>

class Mira;
class Inimigo;
class Boss;

struct Tiro {
    float x, y, z;
    float xAnterior, yAnterior, zAnterior;
    float dirX, dirY, dirZ;
    float distancia;
    bool ativo;
    bool tiroInimigo;
};

class Atirar {
private:
    std::vector<Tiro> tiros;
    std::chrono::steady_clock::time_point ultimoDisparo;

public:
    Atirar();
    void disparar(const Mira& mira);
    void dispararInimigo(float origemX, float origemY, float origemZ,
                         float alvoX,  float alvoY,  float alvoZ);
    void atualizar(std::vector<Inimigo>& inimigos);
    void atualizarBoss(Boss& boss);
    void desenhar();
    void limparTiros();
};

#endif
