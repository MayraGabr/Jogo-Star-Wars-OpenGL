#ifndef MENU_H
#define MENU_H

#include <vector>

struct EstrelaMenu {
    float x, y;          // posicao normalizada relativa a area onde sera desenhada
    float brilho;
    float tamanho;
    float fase;
    float profundidade;  
};

class Menu {
private:
    bool noMenu;
    int vidaMaxima;

    bool estrelasGeradas;
    std::vector<EstrelaMenu> estrelas;

    void prepararTela2D(int largura, int altura);
    void restaurarTela3D();
    void escreverTexto(float x, float y, const char* texto);
    void escreverTextoCentralizado(float y, const char* texto, void* fonte);
    void desenharRetangulo(float x, float y, float largura, float altura, float r, float g, float b);
    void desenharContorno(float x, float y, float largura, float altura, float r, float g, float b);
    void desenharTecla(float x, float y, float largura, float altura, const char* texto);
    void desenharEstrelas(float x, float y, float largura, float altura);
    void desenharLogoStarWars(float centroX, float y);
    float larguraStroke(const char* texto, void* fonte);

    void gerarEstrelas();
    void desenharRetanguloAlpha(float x, float y, float largura, float altura, float r, float g, float b, float a);
    void desenharContornoAlpha(float x, float y, float largura, float altura, float r, float g, float b, float a);
    void desenharCantos(float x, float y, float largura, float altura, float tam, float r, float g, float b, float a);
    void escreverTextoEspacado(float centroX, float y, const char* texto, float espacoExtra, void* fonte);
    void escreverTextoComBrilho(float centroX, float y, const char* texto, float r, float g, float b, float intensidade, void* fonte);
    void desenharIconeAlvo(float cx, float cy, float r);
    void desenharIconeMira(float cx, float cy, float r);

public:
    Menu();

    bool estaNoMenu() const;
    void iniciarJogo();

    void desenharMenu();
    void desenharBarraVida(int vidaAtual);
};

#endif
