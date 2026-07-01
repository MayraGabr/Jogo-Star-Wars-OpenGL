#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include "Camera.h"
#include "Colisao.h"

#include <cstdio>

//Localizaçao inicial da camera e o angulo
float camX = 9.5f;
float camY = -0.05f;
float camZ = -0.25f;
float anguloY = 270.0f;
float anguloX = 0.0f;

// Adicionado junto com as outras variáveis globais:
int vidaJogador = 100;

// Mouse só controla a câmera enquanto o botão direito estiver pressionado
bool mousePressionado = false;
int ultimoX = 0;
int ultimoY =0;

void visao(){
    float radY = anguloY * 3.14159f / 180.0f;
    float radX = anguloX * 3.14159f / 180.0f;

    float dirX = sin(radY) * cos(radX);
    float dirY = sin(radX);
    float dirZ = -cos(radY) * cos(radX);

    gluLookAt(
        camX, camY, camZ,
        camX + dirX, camY + dirY, camZ + dirZ,
        0.0f, 1.0f, 0.0f
    );
}

void botaoMouse(int botao, int estado, int x, int y){
    if (botao == GLUT_RIGHT_BUTTON) {
        mousePressionado = (estado == GLUT_DOWN);
        ultimoX = x;
        ultimoY = y;
    }
}

void movimentacaoMouse(int x, int y) {
    if (!mousePressionado) {
        ultimoX = x;
        ultimoY = y;
        return;
    }

    float sensibilidade = 0.15f;

    float deltaX = x - ultimoX;
    float deltaY = y - ultimoY;
    const float deltaMaximo = 60.0f;
    if (deltaX > deltaMaximo) deltaX = deltaMaximo;
    if (deltaX < -deltaMaximo) deltaX = -deltaMaximo;
    if (deltaY > deltaMaximo) deltaY = deltaMaximo;
    if (deltaY < -deltaMaximo) deltaY = -deltaMaximo;

    anguloY += deltaX * sensibilidade;
    anguloX -= deltaY * sensibilidade;

    if (anguloX > 45.0f) anguloX = 45.0f;
    if (anguloX < -45.0f) anguloX = -45.0f;

    ultimoX = x;
    ultimoY = y;

    glutPostRedisplay();
}

bool podeMoverContraInimigos(float novoX, float novoZ, const std::vector<Inimigo>& inimigos) {
    const float raioJogador = 0.6f;
    const float raioInimigo = 0.8f;
    const float distanciaMinima = raioJogador + raioInimigo;

    for (const auto& inimigo : inimigos) {
        if (!inimigo.estarVivo()) {
            continue;
        }

        float dx = novoX - inimigo.getX();
        float dz = novoZ - inimigo.getZ();
        float distancia = sqrt(dx * dx + dz * dz);

        if (distancia < distanciaMinima) {
            return false;
        }
    }

    return true;
}

void movimentacaoTeclado(unsigned char tecla, int x, int y, const std::vector<Inimigo>& inimigos){
    float velocidade = 0.25f;
    float rad = anguloY * 3.14159f / 180.0f;
    float dirX = sin(rad);
    float dirZ = -cos(rad);
    float ladoX = cos(rad);
    float ladoZ = sin(rad);

    float novoX = camX;
    float novoZ = camZ;

    //Andar
    //Frente e tras
    if (tecla == 'w' || tecla == 'W') {
        novoX += dirX * velocidade;
        novoZ += dirZ * velocidade;
    }
    if (tecla == 's' || tecla == 'S') {
        novoX -= dirX * velocidade;
        novoZ -= dirZ * velocidade;
    }

    //Esquerda e Direita
    if (tecla == 'a' || tecla == 'A') {
        novoX -= ladoX * velocidade;
        novoZ -= ladoZ * velocidade;
    }
    if (tecla == 'd' || tecla == 'D') {
        novoX += ladoX * velocidade;
        novoZ += ladoZ * velocidade;
    }

    if (podeMoverPara(novoX, novoZ) && podeMoverContraInimigos(novoX, novoZ, inimigos)) {
        camX = novoX;
        camZ = novoZ;
    }
    else if (podeMoverPara(novoX, camZ) && podeMoverContraInimigos(novoX, camZ, inimigos)) {
        camX = novoX;
    }
    else if (podeMoverPara(camX, novoZ) && podeMoverContraInimigos(camX, novoZ, inimigos)) {
        camZ = novoZ;
    }

    if (tecla == 27) {
        exit(0);
    }
    
    glutPostRedisplay();
}

// Adicionado no final do arquivo:
void aplicarDanoJogador(int dano) {
    vidaJogador -= dano;
    if (vidaJogador < 0) {
        vidaJogador = 0;
    }
}

bool jogadorVivo() {
    return vidaJogador > 0;
}