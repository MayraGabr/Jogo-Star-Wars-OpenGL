#ifndef TELAFINAL_H
#define TELAFINAL_H

#include <chrono>
#include <vector>

struct Estrela {
    float x, y;
    float brilho;
    float tamanho;
    float fase;
    float profundidade; 
};

class TelaFinal {
private:
    bool ativa;
    bool venceu;

    int troopersMortos;
    int vidaRestante;
    int tempoFinal;

    std::chrono::steady_clock::time_point inicioJogo;
    float tempoAtivacao; 

    bool estrelasGeradas;
    std::vector<Estrela> estrelas;

    void gerarEstrelas();

public:
    TelaFinal();

    void iniciar();

    void mostrarVitoria(
        int troopers,
        int vida
    );

    void mostrarDerrota(
        int troopers,
        int vida
    );

    bool estaAtiva() const;

    void desenhar();
};

#endif
