#include <GL/glut.h>

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <ctime>
#include <cstdio>
#include <vector>
#include <chrono>

#include "Colisao.h"
#include "Cenario.h"
#include "Camera.h"
#include "Mira.h"
#include "Atirar.h"
#include "Inimigo.h"
#include "Menu.h"
#include "Spawn.h"
//#include "Som.h"
#include "TelaFinal.h"
#include "Boss.h"
#include "TelaInicial.h"

Boss boss;
Menu menu;
//Som som;
Cenario cenario;
Mira mira;
Atirar atirar;
TelaFinal telaFinal;
TelaInicial telaInicial;

std::vector<Inimigo> inimigos;
int troopersEliminados = 0;
bool jogoAcabou = false;
float camXOriginal;
float camZOriginal;
bool modoBoss = false;
float alturaAlvoBoss = 0.6f;
auto ultimoDanoBoss = std::chrono::steady_clock::now();

void desenharIconeAlvoHUD(float cx, float cy, float r) {
    glColor3f(1.0f, 0.87f, 0.05f);
    glLineWidth(1.5f);
    int passos = 16;
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < passos; i++) {
            float ang = (float)i / passos * 6.2832f;
            glVertex2f(cx + cosf(ang) * r, cy + sinf(ang) * r);
        }
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(cx - r, cy);
        glVertex2f(cx - r * 0.4f, cy);
        glVertex2f(cx + r * 0.4f, cy);
        glVertex2f(cx + r, cy);
        glVertex2f(cx, cy - r);
        glVertex2f(cx, cy - r * 0.4f);
        glVertex2f(cx, cy + r * 0.4f);
        glVertex2f(cx, cy + r);
    glEnd();
    glLineWidth(1.0f);
}

// HUD do canto inferior: contador de troopers eliminados — sem fundo escuro.
void desenharHUDKills() {
    int largura = glutGet(GLUT_WINDOW_WIDTH);
    int altura  = glutGet(GLUT_WINDOW_HEIGHT);

    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, largura, 0, altura);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float painelX = 20.0f;
    float painelY = altura - 120.0f; 
    float painelAltura = 37.0f;

    // Cantos decorativos dourados (sem fundo)
    float tam = 9.0f;
    glColor4f(1.0f, 0.87f, 0.05f, 0.85f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(painelX - 4, painelY - 4);
        glVertex2f(painelX - 4 + tam, painelY - 4);
        glVertex2f(painelX - 4, painelY - 4);
        glVertex2f(painelX - 4, painelY - 4 + tam);

        glVertex2f(painelX + 174, painelY + painelAltura + 4);
        glVertex2f(painelX + 174 - tam, painelY + painelAltura + 4);
        glVertex2f(painelX + 174, painelY + painelAltura + 4);
        glVertex2f(painelX + 174, painelY + painelAltura + 4 - tam);
    glEnd();
    glLineWidth(1.0f);

    // Icone de alvo
    desenharIconeAlvoHUD(painelX + 26.0f, painelY + painelAltura / 2.0f, 11.0f);

    // Texto com sombra para legibilidade sem fundo
    char texto[32];
    sprintf(texto, "KILLS: %d", troopersEliminados);

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(painelX + 51.0f, painelY + painelAltura / 2.0f - 7.0f);
    for (char* c = texto; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    glColor4f(0.0f, 0.0f, 0.0f, 0.85f);
    glRasterPos2f(painelX + 49.0f, painelY + painelAltura / 2.0f - 6.0f);
    for (char* c = texto; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    glDisable(GL_BLEND);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

void desenharEfeitoSangue(int vida) {
    if (vida > 25) return;

    int largura = glutGet(GLUT_WINDOW_WIDTH);
    int altura  = glutGet(GLUT_WINDOW_HEIGHT);

    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, largura, 0, altura);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float t = 1.0f - (vida / 25.0f);
    float alpha = t * 0.70f;

    float borda = 200.0f; 
    int camadas = 18;

    glBegin(GL_QUADS);

    for (int i = 0; i < camadas; i++) {
        float p1 = (float)i / camadas;
        float p2 = (float)(i + 1) / camadas;

        float a1 = alpha * (1.0f - p1) * (1.0f - p1);
        float a2 = alpha * (1.0f - p2) * (1.0f - p2);

        float d1 = borda * p1;
        float d2 = borda * p2;

        // esquerda
        glColor4f(0.65f, 0.0f, 0.0f, a1);
        glVertex2f(d1, 0);
        glVertex2f(d1, altura);
        glColor4f(0.65f, 0.0f, 0.0f, a2);
        glVertex2f(d2, altura);
        glVertex2f(d2, 0);

        // direita
        glColor4f(0.65f, 0.0f, 0.0f, a1);
        glVertex2f(largura - d1, 0);
        glVertex2f(largura - d1, altura);
        glColor4f(0.65f, 0.0f, 0.0f, a2);
        glVertex2f(largura - d2, altura);
        glVertex2f(largura - d2, 0);

        // cima
        glColor4f(0.65f, 0.0f, 0.0f, a1);
        glVertex2f(0, altura - d1);
        glVertex2f(largura, altura - d1);
        glColor4f(0.65f, 0.0f, 0.0f, a2);
        glVertex2f(largura, altura - d2);
        glVertex2f(0, altura - d2);

        // baixo
        glColor4f(0.65f, 0.0f, 0.0f, a1);
        glVertex2f(0, d1);
        glVertex2f(largura, d1);
        glColor4f(0.65f, 0.0f, 0.0f, a2);
        glVertex2f(largura, d2);
        glVertex2f(0, d2);
    }

    glEnd();

    // cantos arredondados/vinheta
    glBegin(GL_TRIANGLE_FAN);
        glColor4f(0.75f, 0.0f, 0.0f, alpha);
        glVertex2f(0, 0);
        for (int i = 0; i <= 24; i++) {
            float a = i * 3.14159f / 48.0f;
            glColor4f(0.75f, 0.0f, 0.0f, 0.0f);
            glVertex2f(cosf(a) * borda, sinf(a) * borda);
        }
    glEnd();

    glDisable(GL_BLEND);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHTING);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (telaInicial.estaAtiva()) {
        telaInicial.desenhar();
        glutSwapBuffers();
        return;
    }

    if (menu.estaNoMenu()) {
        menu.desenharMenu();
        glutSwapBuffers();
        return;
    }

    if (telaFinal.estaAtiva()) {
        telaFinal.desenhar();
        glutSwapBuffers();
        return;
    }

    visao();

    // Atualiza os inimigos
    for (auto& inimigo : inimigos) {
        inimigo.atualizar(camX, camY, camZ, inimigos);
    }

    // Desenha cenário
    cenario.desenhar();

    // Desenha inimigos
    for (auto& inimigo : inimigos) {
        inimigo.desenhar();
    }

    // Desenha boss
    if (boss.estarVivo()) {
        boss.desenhar();
    }

    bool inimigoNaMira = false;

    Inimigo* inimigoFocado = nullptr;
    float menorDistanciaFocado = 1e9f;

    for (auto& inimigo : inimigos) {
        if (!inimigo.estarVivo())
            continue;

        if (inimigoNaDirecaoCamera(
                camX,
                camZ,
                anguloY,
                inimigo.getX(),
                inimigo.getZ()))
        {
            inimigoNaMira = true;

            float dx = inimigo.getX() - camX;
            float dz = inimigo.getZ() - camZ;
            float distancia = sqrt(dx * dx + dz * dz);

            if (distancia < menorDistanciaFocado) {
                menorDistanciaFocado = distancia;
                inimigoFocado = &inimigo;
            }
        }
    }

    bool bossFocado = false;

    if (boss.estarVivo()) {
        if (inimigoNaDirecaoCamera(
                camX,
                camZ,
                anguloY,
                boss.getX(),
                boss.getZ()))
        {
            inimigoNaMira = true;

            float dx = boss.getX() - camX;
            float dz = boss.getZ() - camZ;
            float distancia = sqrt(dx * dx + dz * dz);

            if (distancia < menorDistanciaFocado) {
                menorDistanciaFocado = distancia;
                inimigoFocado = nullptr;
                bossFocado = true;
            }
        }
    }

    mira.setSobreInimigo(inimigoNaMira);

    if (inimigoFocado != nullptr) {
        float vidaPercentual = inimigoFocado->getVida() / 100.0f;
        mira.setAlvoFocado(true, vidaPercentual);
    }
    else if (bossFocado) {
        float vidaPercentual = boss.getVida() / 500.0f;
        mira.setAlvoFocado(true, vidaPercentual);
    }
    else {
        mira.setAlvoFocado(false, 0.0f);
    }

    mira.desenhar();
    menu.desenharBarraVida(vidaJogador);
    desenharHUDKills();
    atirar.desenhar();
    desenharEfeitoSangue(vidaJogador);

    if (vidaJogador <= 25) glutPostRedisplay(); // anima o pulso do sangue

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(70.0, (float)w / h, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
}

void teclado(unsigned char tecla, int x, int y) {
    if (telaInicial.estaAtiva()) {
        if (tecla == 27) {
            exit(0);
        }
        return;
    }

    if (telaFinal.estaAtiva()) {

    if (tecla == 'r' || tecla == 'R') {

        inimigos.clear();
        atirar.limparTiros();

        vidaJogador = 100;
        camX = 9.0f;
        camY = -0.05f;
        camZ = -0.25f;
        anguloY = 270.0f;

        jogoAcabou = false;
        troopersEliminados = 0;

        resetarSpawns();

        modoBoss = false;
        resetarSpawns();
        boss.resetar();

        telaFinal = TelaFinal();
        telaFinal.iniciar();

        glutPostRedisplay();
    }

    if (tecla == 27) {
        exit(0);
    }

    return;
}

    if (menu.estaNoMenu()) {
        if (tecla == 13) {
            menu.iniciarJogo();
            glutPostRedisplay();
        }

        if (tecla == 27) {
            exit(0);
        }
    return;
    }
    
    if (tecla == 'e' || tecla == 'E') {
        mira.alternar();
        glutPostRedisplay();
        return;
    }

    // Enter ou espaço atiram, mas somente se a mira estiver ativa
    if (tecla == 13 || tecla == ' ' ) {
        atirar.disparar(mira);
        glutPostRedisplay();
        return;
    }

    if (jogoAcabou) {
    return;
    }

    if (modoBoss) {
        return;
    }

    movimentacaoTeclado(tecla, x, y, inimigos);
}

void mouseJogo(int botao, int estado, int x, int y) {
    if (telaInicial.estaAtiva()) {
        if (botao == GLUT_LEFT_BUTTON && estado == GLUT_DOWN) {
            telaInicial.clicar(x, y);
            glutPostRedisplay();
        }
        return;
    }

    if (botao == GLUT_LEFT_BUTTON && estado == GLUT_DOWN) {
        if (!menu.estaNoMenu()) {
            atirar.disparar(mira);
            glutPostRedisplay();
        }
    }

    botaoMouse(botao, estado, x, y);
}

void mouseMovePassivo(int x, int y) {
    if (telaInicial.estaAtiva()) {
        telaInicial.moverMouse(x, y);
        glutPostRedisplay();
    }
}

void timer(int valor) {
    if (telaInicial.estaAtiva()) {
        glutTimerFunc(16, timer, 0);
        return;
    }

    if (!jogoAcabou) {
        //AQUI
        atirar.atualizar(inimigos);
        atirar.atualizarBoss(boss);
        verificarSpawnsPorMapa();

        for (auto& inimigo : inimigos){
            inimigo.atacarJogador(camX, camZ);
        }

        static float tempo = 0.0f;
        tempo += 0.1f;

        if (modoBoss && boss.estarVivo()) {

            if (camY < alturaAlvoBoss) {
                camY += 0.01f;
            }

            float intensidade = 0.015f;

            camX = camXOriginal + sin(tempo * 3.0f) * intensidade;
            camZ = camZOriginal + cos(tempo * 2.0f) * intensidade;
        }

        if (modoBoss && boss.estarVivo()) {
            auto agora = std::chrono::steady_clock::now();

            auto tempo = std::chrono::duration_cast<std::chrono::milliseconds>(
                agora - ultimoDanoBoss
            ).count();

            if (tempo >= 1000) {
                aplicarDanoJogador(5);
                ultimoDanoBoss = agora;
            }
        }

    bool todosMortos = true;

    for (auto& inimigo : inimigos) {
        if (inimigo.estarVivo()) {
            todosMortos = false;
            break;
        }
    }

    if (modoBoss && !boss.estarVivo()) {
        modoBoss = false;

        telaFinal.mostrarVitoria(
            troopersEliminados,
            vidaJogador
        );

        jogoAcabou = true;
    }

    if (!jogadorVivo()) {

        telaFinal.mostrarDerrota(
            troopersEliminados,
            vidaJogador
        );

        jogoAcabou = true;
    }
        
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void init() {
    
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.03, 0.03, 0.05, 1.0);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat luzPos[] = {0.0, 10.0, 10.0, 1.0};
    GLfloat luzAmb[] = {0.7, 0.7, 0.7, 1.0};
    GLfloat luzDif[] = {1.0, 1.0, 1.0, 1.0};

    glLightfv(GL_LIGHT0, GL_POSITION, luzPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDif);

    cenario.carregar();
    Inimigo::carregarModelo();
    boss.carregar();
    srand(time(nullptr));

    telaInicial.ativar();
    telaFinal.iniciar();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 700);
    glutCreateWindow("Camera FPS no mapa");

    //som.tocarMusica();

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);
    glutMouseFunc(mouseJogo);
    glutPassiveMotionFunc(mouseMovePassivo);
    glutMotionFunc(movimentacaoMouse);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}