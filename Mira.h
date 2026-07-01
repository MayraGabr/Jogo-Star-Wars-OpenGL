#ifndef MIRA_H
#define MIRA_H

class Mira {
private:
    bool ativa;
    bool sobreInimigo;

    bool temAlvoFocado;
    float vidaPercentualAlvo; 

public:
    Mira();
    void alternar();
    bool estaAtiva() const;
    void desenhar();
    void setSobreInimigo(bool valor);
    void setAlvoFocado(bool temAlvo, float vidaPercentual);
};

#endif
