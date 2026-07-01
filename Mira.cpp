#include <GL/glut.h>
#include <cmath>
#include "Mira.h"

Mira::Mira() {
    ativa = false;
    sobreInimigo = false;

    temAlvoFocado = false;
    vidaPercentualAlvo = 1.0f;
}

void Mira::setSobreInimigo(bool valor) {
    sobreInimigo = valor;
}

void Mira::setAlvoFocado(bool temAlvo, float vidaPercentual) {
    temAlvoFocado = temAlvo;

    if (vidaPercentual < 0.0f) vidaPercentual = 0.0f;
    if (vidaPercentual > 1.0f) vidaPercentual = 1.0f;

    vidaPercentualAlvo = vidaPercentual;
}

void Mira::alternar() {
    ativa = !ativa;
}

bool Mira::estaAtiva() const {
    return ativa;
}

void Mira::desenhar() {
    if (!ativa) {
        return;
    }

    int largura = glutGet(GLUT_WINDOW_WIDTH);
    int altura = glutGet(GLUT_WINDOW_HEIGHT);

    float centroX = largura / 2.0f;
    float centroY = altura / 2.0f;

    float raio = 35.0f;
    float riscoExterno = 25.0f;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, largura, 0, altura);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (sobreInimigo){
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    else{
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    glLineWidth(2.0f);

    // Círculo externo
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 100; i++) {
            float angulo = 2.0f * 3.14159f * i / 100.0f;
            float x = centroX + cos(angulo) * raio;
            float y = centroY + sin(angulo) * raio;
            glVertex2f(x, y);
        }
    glEnd();

    // Riscos externos da mira
    glBegin(GL_LINES);
        // esquerda
        glVertex2f(centroX - raio - riscoExterno, centroY);
        glVertex2f(centroX - raio, centroY);

        // direita
        glVertex2f(centroX + raio, centroY);
        glVertex2f(centroX + raio + riscoExterno, centroY);

        // cima
        glVertex2f(centroX, centroY + raio);
        glVertex2f(centroX, centroY + raio + riscoExterno);

        // baixo
        glVertex2f(centroX, centroY - raio - riscoExterno);
        glVertex2f(centroX, centroY - raio);
    glEnd();

    // Cantinhos internos
    float cantoDist = 10.0f;
    float cantoTam = 7.0f;

    glBegin(GL_LINES);
        // canto superior esquerdo
        glVertex2f(centroX - cantoDist, centroY + cantoDist);
        glVertex2f(centroX - cantoDist - cantoTam, centroY + cantoDist);
        glVertex2f(centroX - cantoDist, centroY + cantoDist);
        glVertex2f(centroX - cantoDist, centroY + cantoDist + cantoTam);

        // canto superior direito
        glVertex2f(centroX + cantoDist, centroY + cantoDist);
        glVertex2f(centroX + cantoDist + cantoTam, centroY + cantoDist);
        glVertex2f(centroX + cantoDist, centroY + cantoDist);
        glVertex2f(centroX + cantoDist, centroY + cantoDist + cantoTam);

        // canto inferior esquerdo
        glVertex2f(centroX - cantoDist, centroY - cantoDist);
        glVertex2f(centroX - cantoDist - cantoTam, centroY - cantoDist);
        glVertex2f(centroX - cantoDist, centroY - cantoDist);
        glVertex2f(centroX - cantoDist, centroY - cantoDist - cantoTam);

        // canto inferior direito
        glVertex2f(centroX + cantoDist, centroY - cantoDist);
        glVertex2f(centroX + cantoDist + cantoTam, centroY - cantoDist);
        glVertex2f(centroX + cantoDist, centroY - cantoDist);
        glVertex2f(centroX + cantoDist, centroY - cantoDist - cantoTam);
    glEnd();

    // Bolinha central
    float raioBolinha = 3.0f;

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centroX, centroY);

        for (int i = 0; i <= 30; i++) {
            float angulo = 2.0f * 3.14159f * i / 30.0f;
            float x = centroX + cos(angulo) * raioBolinha;
            float y = centroY + sin(angulo) * raioBolinha;
            glVertex2f(x, y);
        }
    glEnd();

    //Barra de vida do inimigo focado
    if (temAlvoFocado){
        float barraLargura = 90.0f;
        float barraAltura = 8.0f;
        float barraX = centroX - barraLargura / 2.0f;
        float barraY = centroY - raio - riscoExterno - 22.0f;

        // fundo
        glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
        glBegin(GL_QUADS);
            glVertex2f(barraX - 3, barraY - 3);
            glVertex2f(barraX + barraLargura + 3, barraY - 3);
            glVertex2f(barraX + barraLargura + 3, barraY + barraAltura + 3);
            glVertex2f(barraX - 3, barraY + barraAltura + 3);
        glEnd();

        // trilho vazio
        glColor3f(0.15f, 0.15f, 0.15f);
        glBegin(GL_QUADS);
            glVertex2f(barraX, barraY);
            glVertex2f(barraX + barraLargura, barraY);
            glVertex2f(barraX + barraLargura, barraY + barraAltura);
            glVertex2f(barraX, barraY + barraAltura);
        glEnd();

        // preenchimento, cor reage ao nivel: verde -> amarelo -> vermelho
        float corR, corG;
        if (vidaPercentualAlvo > 0.5f) {
            corR = (1.0f - vidaPercentualAlvo) * 2.0f * 0.9f;
            corG = 0.8f;
        } else {
            corR = 0.9f;
            corG = vidaPercentualAlvo * 2.0f * 0.8f;
        }

        glColor3f(corR, corG, 0.12f);
        glBegin(GL_QUADS);
            glVertex2f(barraX, barraY);
            glVertex2f(barraX + barraLargura * vidaPercentualAlvo, barraY);
            glVertex2f(barraX + barraLargura * vidaPercentualAlvo, barraY + barraAltura);
            glVertex2f(barraX, barraY + barraAltura);
        glEnd();

        // contorno
        glColor3f(0.9f, 0.9f, 0.95f);
        glLineWidth(1.2f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(barraX, barraY);
            glVertex2f(barraX + barraLargura, barraY);
            glVertex2f(barraX + barraLargura, barraY + barraAltura);
            glVertex2f(barraX, barraY + barraAltura);
        glEnd();

        // pequenos tracos nas pontas (estilo HUD militar)
        glBegin(GL_LINES);
            glVertex2f(barraX, barraY - 3);
            glVertex2f(barraX, barraY + barraAltura + 3);
            glVertex2f(barraX + barraLargura, barraY - 3);
            glVertex2f(barraX + barraLargura, barraY + barraAltura + 3);
        glEnd();
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}
