#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include "Inimigo.h"

extern float camX;
extern float camY;
extern float camZ;
extern float anguloY;
extern float anguloX;

void visao();
void botaoMouse(int botao, int estado, int x, int y);
void movimentacaoMouse(int x, int y);
void movimentacaoTeclado(unsigned char tecla, int x, int y, const std::vector<Inimigo>& inimigos);

// Adicionado:
extern int vidaJogador;

void aplicarDanoJogador(int dano);
bool jogadorVivo();

#endif
