#include "ModeloAnimado.h"
#ifndef INIMIGO_H
#define INIMIGO_H
#include <chrono>
#include <vector>

class Inimigo {
private:
    std::chrono::steady_clock::time_point ultimoAtaque;
    float inimigoX, inimigoY, inimigoZ;
    bool vivo;
    float rotY;
    int vida;

    static ModeloAnimado modelo;
    
public:
    Inimigo();
    static void carregarModelo();
    void carregar();
    void setPosicao(float x, float y, float z);
    void atualizar(float camX, float camY, float camZ, const std::vector<Inimigo>& inimigos);
    void desenhar();

    void atacarJogador(float camX, float camZ);

    void tomarDano(int dano);
    bool estarVivo() const;

    // Adicionado no final da classe:
    float getX() const;
    float getZ() const;
    int getVida() const;
};

#endif