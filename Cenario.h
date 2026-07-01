#ifndef CENARIO_H
#define CENARIO_H

#include "Modelo.h"

class Cenario {
private:
    Modelo mapa;

public:
    Cenario();

    void carregar();
    void desenhar();
};

#endif