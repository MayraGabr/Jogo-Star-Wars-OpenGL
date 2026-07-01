#ifndef TELAINICIAL_H
#define TELAINICIAL_H

#include <vector>

struct EstrelaInicial {
    float x, y;
    float brilho;
    float tamanho;
    float fase;
    float profundidade; 
};

class TelaInicial {
private:
    bool ativa;
    bool iniciarJogo;
    bool mouseSobreBotao;

    bool estrelasGeradas;
    std::vector<EstrelaInicial> estrelas;

    float tempoAtivacao; 
    float botaoX, botaoY;
    float botaoLargura, botaoAltura;

    void gerarEstrelas();
    bool pontoDentroDoBotao(int x, int y) const;

public:
    TelaInicial();

    void ativar();

    bool estaAtiva() const;
    bool deveIniciarJogo() const;

    void clicar(int x, int y);
    void moverMouse(int x, int y);
    void desenhar();
};

#endif
