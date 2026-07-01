#include "Cenario.h"

Cenario::Cenario() {
}

void Cenario::carregar() {
    mapa.carregar("data/mapa.obj", "data/", "data/texturas/");
}

void Cenario::desenhar() {
    mapa.desenhar();
}