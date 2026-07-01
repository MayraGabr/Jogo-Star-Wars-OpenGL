#include <GL/glut.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "Menu.h"

Menu::Menu() {
    noMenu = true;
    vidaMaxima = 100;
    estrelasGeradas = false;
}

bool Menu::estaNoMenu() const {
    return noMenu;
}

void Menu::iniciarJogo() {
    noMenu = false;
}

void Menu::gerarEstrelas() {
    estrelas.clear();
    for (int i = 0; i < 220; i++) {
        EstrelaMenu e;
        e.x = (float)(rand() % 1000) / 1000.0f;
        e.y = (float)(rand() % 1000) / 1000.0f;
        e.profundidade = (float)(rand() % 100) / 100.0f;
        e.brilho = 0.3f + (rand() % 100) / 100.0f * 0.6f;
        e.tamanho = 1.0f + e.profundidade * 1.8f;
        e.fase = (rand() % 100) / 100.0f * 6.28f;
        estrelas.push_back(e);
    }
    estrelasGeradas = true;
}

void Menu::prepararTela2D(int largura, int altura) {
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
}

void Menu::restaurarTela3D() {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

void Menu::escreverTexto(float x, float y, const char* texto) {
    glRasterPos2f(x, y);

    for (int i = 0; texto[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, texto[i]);
    }
}

void Menu::escreverTextoCentralizado(float y, const char* texto, void* fonte) {
    int larguraTela = glutGet(GLUT_WINDOW_WIDTH);
    int larguraTexto = glutBitmapLength(fonte, (const unsigned char*)texto);
    float x = (larguraTela - larguraTexto) / 2.0f;

    glRasterPos2f(x, y);
    for (int i = 0; texto[i] != '\0'; i++) {
        glutBitmapCharacter(fonte, texto[i]);
    }
}

void Menu::escreverTextoEspacado(float centroX, float y, const char* texto, float espacoExtra, void* fonte) {
    int n = (int)strlen(texto);
    float largura = 0.0f;
    for (int i = 0; i < n; i++) {
        largura += glutBitmapWidth(fonte, texto[i]);
    }
    largura += espacoExtra * (n - 1);

    float x = centroX - largura / 2.0f;
    for (int i = 0; i < n; i++) {
        glRasterPos2f(x, y);
        glutBitmapCharacter(fonte, texto[i]);
        x += glutBitmapWidth(fonte, texto[i]) + espacoExtra;
    }
}

void Menu::escreverTextoComBrilho(float centroX, float y, const char* texto, float r, float g, float b, float intensidade, void* fonte) {
    int larguraTexto = glutBitmapLength(fonte, (const unsigned char*)texto);
    float x = centroX - larguraTexto / 2.0f;

    glColor4f(r, g, b, 0.15f * intensidade);
    for (int dx = -2; dx <= 2; dx++)
    for (int dy = -2; dy <= 2; dy++) {
        if (dx == 0 && dy == 0) continue;
        glRasterPos2f(x + dx, y + dy);
        for (int i = 0; texto[i] != '\0'; i++) {
            glutBitmapCharacter(fonte, texto[i]);
        }
    }

    glColor4f(r, g, b, intensidade);
    glRasterPos2f(x, y);
    for (int i = 0; texto[i] != '\0'; i++) {
        glutBitmapCharacter(fonte, texto[i]);
    }
}

void Menu::desenharRetangulo(float x, float y, float largura, float altura, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + largura, y);
        glVertex2f(x + largura, y + altura);
        glVertex2f(x, y + altura);
    glEnd();
}

void Menu::desenharRetanguloAlpha(float x, float y, float largura, float altura, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + largura, y);
        glVertex2f(x + largura, y + altura);
        glVertex2f(x, y + altura);
    glEnd();
}

void Menu::desenharContorno(float x, float y, float largura, float altura, float r, float g, float b) {
    glColor3f(r, g, b);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + largura, y);
        glVertex2f(x + largura, y + altura);
        glVertex2f(x, y + altura);
    glEnd();
}

void Menu::desenharContornoAlpha(float x, float y, float largura, float altura, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + largura, y);
        glVertex2f(x + largura, y + altura);
        glVertex2f(x, y + altura);
    glEnd();
}

void Menu::desenharCantos(float x, float y, float largura, float altura, float tam, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        // inferior esquerdo
        glVertex2f(x - 4, y - 4);
        glVertex2f(x - 4 + tam, y - 4);
        glVertex2f(x - 4, y - 4);
        glVertex2f(x - 4, y - 4 + tam);

        // inferior direito
        glVertex2f(x + largura + 4, y - 4);
        glVertex2f(x + largura + 4 - tam, y - 4);
        glVertex2f(x + largura + 4, y - 4);
        glVertex2f(x + largura + 4, y - 4 + tam);

        // superior esquerdo
        glVertex2f(x - 4, y + altura + 4);
        glVertex2f(x - 4 + tam, y + altura + 4);
        glVertex2f(x - 4, y + altura + 4);
        glVertex2f(x - 4, y + altura + 4 - tam);

        // superior direito
        glVertex2f(x + largura + 4, y + altura + 4);
        glVertex2f(x + largura + 4 - tam, y + altura + 4);
        glVertex2f(x + largura + 4, y + altura + 4);
        glVertex2f(x + largura + 4, y + altura + 4 - tam);
    glEnd();
    glLineWidth(1.0f);
}

void Menu::desenharTecla(float x, float y, float largura, float altura, const char* texto) {
    desenharRetangulo(x + 3.0f, y - 3.0f, largura, altura, 0.02f, 0.02f, 0.03f);

    // leve gradiente vertical simulando relevo na tecla
    glBegin(GL_QUADS);
        glColor3f(0.20f, 0.21f, 0.27f);
        glVertex2f(x, y + altura);
        glVertex2f(x + largura, y + altura);
        glColor3f(0.12f, 0.12f, 0.16f);
        glVertex2f(x + largura, y);
        glVertex2f(x, y);
    glEnd();

    desenharContorno(x, y, largura, altura, 0.88f, 0.88f, 0.95f);

    void* fonte = GLUT_BITMAP_HELVETICA_18;
    int larguraTexto = glutBitmapLength(fonte, (const unsigned char*)texto);
    float textoX = x + (largura - larguraTexto) / 2.0f;
    float textoY = y + altura / 2.0f - 5.0f;

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(textoX, textoY);
    for (int i = 0; texto[i] != '\0'; i++) {
        glutBitmapCharacter(fonte, texto[i]);
    }
}

float Menu::larguraStroke(const char* texto, void* fonte) {
    float largura = 0.0f;
    for (int i = 0; texto[i] != '\0'; i++) {
        largura += glutStrokeWidth(fonte, texto[i]);
    }
    return largura;
}

void Menu::desenharLogoStarWars(float centroX, float y) {
    const char* texto = "STAR WARS";
    void* fonte = GLUT_STROKE_ROMAN;
    float escala = 0.30f;

    float largura = larguraStroke(texto, fonte) * escala;

    // halo/glow atras do logo: varias copias deslocadas com baixo alpha
    glPushMatrix();
        glTranslatef(centroX - largura / 2.0f, y, 0.0f);
        glScalef(escala, escala, 1.0f);

        for (int dx = -3; dx <= 3; dx += 1)
        for (int dy = -3; dy <= 3; dy += 1) {
            if (dx == 0 && dy == 0) continue;
            glColor4f(1.0f, 0.87f, 0.05f, 0.05f);
            glPushMatrix();
                glTranslatef((float)dx / escala * 0.3f, (float)dy / escala * 0.3f, 0.0f);
                glLineWidth(4.0f);
                for (int i = 0; texto[i] != '\0'; i++) {
                    glutStrokeCharacter(fonte, texto[i]);
                }
            glPopMatrix();
        }
    glPopMatrix();

    // logo principal por cima do glow
    glLineWidth(4.0f);
    glColor3f(1.0f, 0.87f, 0.05f);

    glPushMatrix();
        glTranslatef(centroX - largura / 2.0f, y, 0.0f);
        glScalef(escala, escala, 1.0f);
        for (int i = 0; texto[i] != '\0'; i++) {
            glutStrokeCharacter(fonte, texto[i]);
        }
    glPopMatrix();
    glLineWidth(1.0f);
}

void Menu::desenharEstrelas(float x, float y, float largura, float altura) {
    desenharRetangulo(x, y, largura, altura, 0.0f, 0.0f, 0.0f);

    if (!estrelasGeradas) gerarEstrelas();

    float tempo = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    for (const auto& e : estrelas) {
        float deriva = tempo * (0.005f + e.profundidade * 0.015f);
        float ex = e.x + deriva;
        ex -= floorf(ex); 

        float px = x + ex * largura;
        float py = y + e.y * altura;

        float pulso = 0.5f + 0.5f * sinf(tempo * (1.0f + e.profundidade * 1.8f) + e.fase);
        float brilhoFinal = e.brilho * (0.55f + 0.45f * pulso);

        glPointSize(e.tamanho);
        glBegin(GL_POINTS);
            glColor3f(brilhoFinal, brilhoFinal, brilhoFinal * 1.05f);
            glVertex2f(px, py);
        glEnd();
    }
}

void Menu::desenharIconeAlvo(float cx, float cy, float r) {
    int passos = 16;
    glColor3f(1.0f, 0.87f, 0.05f);
    glLineWidth(1.5f);
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

void Menu::desenharIconeMira(float cx, float cy, float r) {
    glColor3f(0.4f, 0.85f, 1.0f);
    glLineWidth(1.5f);
    int passos = 16;

    glBegin(GL_LINE_LOOP);

        for (int i = 0; i < passos; i++) {
            float ang = (float)i / passos * 6.2832f;
            glVertex2f(cx + cosf(ang) * r, cy + sinf(ang) * r);
        }

    glEnd();
    glBegin(GL_LINES);
        glVertex2f(cx, cy + r * 1.4f);
        glVertex2f(cx, cy + r * 0.5f);
        glVertex2f(cx, cy - r * 1.4f);
        glVertex2f(cx, cy - r * 0.5f);
        glVertex2f(cx + r * 1.4f, cy);
        glVertex2f(cx + r * 0.5f, cy);
        glVertex2f(cx - r * 1.4f, cy);
        glVertex2f(cx - r * 0.5f, cy);
    glEnd();
    glLineWidth(1.0f);
}

void Menu::desenharMenu() {
    int largura = glutGet(GLUT_WINDOW_WIDTH);
    int altura = glutGet(GLUT_WINDOW_HEIGHT);

    prepararTela2D(largura, altura);

    float tempo = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    // Fundo geral com gradiente sutil + estrelas
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(0, 0);
        glVertex2f(largura, 0);
        glColor3f(0.04f, 0.045f, 0.09f);
        glVertex2f(largura, altura);
        glVertex2f(0, altura);
    glEnd();

    desenharEstrelas(0, 0, (float)largura, (float)altura);

    // Janela flutuante central
    float janelaLargura = 860.0f;
    float janelaAltura = 600.0f;
    float janelaX = (largura - janelaLargura) / 2.0f;
    float janelaY = (altura - janelaAltura) / 2.0f;

    // sombra/profundidade por tras da janela
    desenharRetanguloAlpha(janelaX + 12.0f, janelaY - 12.0f, janelaLargura, janelaAltura, 0.0f, 0.0f, 0.0f, 0.6f);

    desenharEstrelas(janelaX, janelaY, janelaLargura, janelaAltura);

    // veu translucido por cima das estrelas internas, para o conteudo ler melhor
    desenharRetanguloAlpha(janelaX, janelaY, janelaLargura, janelaAltura, 0.0f, 0.0f, 0.05f, 0.18f);

    desenharContorno(janelaX, janelaY, janelaLargura, janelaAltura, 0.95f, 0.95f, 1.0f);
    desenharContorno(janelaX + 9.0f, janelaY + 9.0f, janelaLargura - 18.0f, janelaAltura - 18.0f, 0.25f, 0.35f, 0.58f);
    desenharCantos(janelaX, janelaY, janelaLargura, janelaAltura, 18.0f, 1.0f, 0.87f, 0.05f, 0.8f);

    {
        float cicloScan = 5.0f;
        float fracao = fmodf(tempo, cicloScan) / cicloScan;
        float scanY = janelaY + janelaAltura * (1.0f - fracao);

        glBegin(GL_QUADS);
            glColor4f(0.5f, 0.7f, 1.0f, 0.0f);
            glVertex2f(janelaX, scanY + 14.0f);
            glVertex2f(janelaX + janelaLargura, scanY + 14.0f);
            glColor4f(0.5f, 0.7f, 1.0f, 0.06f);
            glVertex2f(janelaX + janelaLargura, scanY);
            glVertex2f(janelaX, scanY);
        glEnd();
        glBegin(GL_QUADS);
            glColor4f(0.5f, 0.7f, 1.0f, 0.06f);
            glVertex2f(janelaX, scanY);
            glVertex2f(janelaX + janelaLargura, scanY);
            glColor4f(0.5f, 0.7f, 1.0f, 0.0f);
            glVertex2f(janelaX + janelaLargura, scanY - 14.0f);
            glVertex2f(janelaX, scanY - 14.0f);
        glEnd();
    }

    // Logo e subtítulo
    desenharLogoStarWars(janelaX + janelaLargura / 2.0f, janelaY + janelaAltura - 82.0f);

    glColor3f(0.75f, 0.80f, 1.0f);
    escreverTextoCentralizado(janelaY + janelaAltura - 128.0f, "MISSAO NO CORREDOR IMPERIAL", GLUT_BITMAP_HELVETICA_18);

    // linha decorativa fina abaixo do subtitulo
    {
        float lx = janelaX + janelaLargura / 2.0f;
        float ly = janelaY + janelaAltura - 142.0f;
        glColor4f(1.0f, 0.87f, 0.05f, 0.5f);
        glBegin(GL_LINES);
            glVertex2f(lx - 120, ly);
            glVertex2f(lx + 120, ly);
        glEnd();
    }

    // Objetivo
    float objetivoX = janelaX + 55.0f;
    float objetivoY = janelaY + janelaAltura - 250.0f;
    float objetivoLargura = janelaLargura - 110.0f;
    float objetivoAltura = 90.0f;

    desenharRetanguloAlpha(objetivoX, objetivoY, objetivoLargura, objetivoAltura, 0.025f, 0.025f, 0.04f, 0.85f);
    desenharContorno(objetivoX, objetivoY, objetivoLargura, objetivoAltura, 0.95f, 0.95f, 1.0f);
    desenharCantos(objetivoX, objetivoY, objetivoLargura, objetivoAltura, 12.0f, 1.0f, 0.87f, 0.05f, 0.6f);

    desenharIconeAlvo(objetivoX + 20.0f, objetivoY + 73.0f, 8.0f);

    glColor3f(1.0f, 0.87f, 0.05f);
    escreverTexto(objetivoX + 37.0f, objetivoY + 66.0f, "OBJETIVO");

    glColor3f(0.88f, 0.92f, 1.0f);
    escreverTexto(objetivoX + 37.0f, objetivoY + 39.0f,
        "Chegue ao final da ultima sala com vida.");
    escreverTexto(objetivoX + 38.0f, objetivoY + 17.0f,
        "Elimine o maximo de oponentes possiveis pelo caminho.");

    desenharIconeMira(janelaX + 58.0f, janelaY + 315.0f, 7.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    escreverTexto(janelaX + 75.0f, janelaY + 310.0f, "CONTROLES");

    glColor4f(0.6f, 0.65f, 0.85f, 0.4f);
    glBegin(GL_LINES);
        glVertex2f(janelaX + 70.0f, janelaY + 305.0f);
        glVertex2f(janelaX + janelaLargura - 70.0f, janelaY + 305.0f);
    glEnd();

    // WASD visual 
    float tecla = 48.0f;
    float espaco = 8.0f;
    float tecladoX = janelaX + 100.0f;
    float tecladoY = janelaY + 186.0f;

    desenharTecla(tecladoX + tecla + espaco, tecladoY + tecla + espaco, tecla, tecla, "W");
    desenharTecla(tecladoX, tecladoY, tecla, tecla, "A");
    desenharTecla(tecladoX + tecla + espaco, tecladoY, tecla, tecla, "S");
    desenharTecla(tecladoX + (tecla + espaco) * 2.0f, tecladoY, tecla, tecla, "D");

    //Mouse ilustrado 
    {
        float legendaX = janelaX + 590.0f;

        float mw  = 52.0f;
        float mh  = 40.0f;
        float corpo = 30.0f; // parte de baixo

        float mcx = legendaX - 45.0f;
        float mby = tecladoY + 58.0f; 

        float ml  = mcx - mw / 2.0f;
        float mr2 = mcx + mw / 2.0f;
        float mtop = mby + mh;

        float pulsoMouse = 0.5f + 0.5f * sinf(tempo * 2.8f);
        int seg = 24;
        float rt = mw / 2.0f;

        // corpo completo do mouse
        glColor4f(0.20f, 0.21f, 0.25f, 1.0f);
        glBegin(GL_POLYGON);

            // lado esquerdo
            glVertex2f(ml, mby);

            // arco superior
            for (int i = 0; i <= seg; i++) {
                float a = (float)i / seg * 3.14159f;
                glVertex2f(mcx + cosf(a) * rt, mby + sinf(a) * rt);
            }

            // lado direito
            glVertex2f(mr2, mby);

            // final arredondado
            for (int i = 0; i <= seg; i++) {
                float a = (float)i / seg * 3.14159f;
                glVertex2f(mcx + cosf(a) * rt, mby - corpo + sinf(a + 3.14159f) * 8.0f);
            }

        glEnd();

        // highlight esquerdo
        glColor4f(1.0f, 0.45f, 0.05f, 0.30f + 0.20f * pulsoMouse);
        glBegin(GL_POLYGON);
            glVertex2f(mcx, mby);
            glVertex2f(ml + 2, mby);

            for (int i = 0; i <= seg / 2; i++) {

                float a = 3.14159f / 2.0f + (float)i / (seg / 2) * (3.14159f / 2.0f);
                glVertex2f(mcx + cosf(a) * (rt - 2), mby + sinf(a) * (rt - 2));
            }
        glEnd();

        // highlight direito
        glColor4f(0.25f, 0.65f, 1.0f, 0.30f + 0.20f * pulsoMouse);
        glBegin(GL_POLYGON);
            glVertex2f(mcx, mby);

            for (int i = 0; i <= seg / 2; i++) {

                float a = (float)i / (seg / 2) * (3.14159f / 2.0f);
                glVertex2f(mcx + cosf(a) * (rt - 2), mby + sinf(a) * (rt - 2));
            }
            glVertex2f(mr2 - 2, mby);
        glEnd();

        // contorno
        glColor4f(0.92f, 0.92f, 0.97f, 1.0f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);

            glVertex2f(ml, mby);

            for (int i = 0; i <= seg; i++) {

                float a = (float)i / seg * 3.14159f;
                glVertex2f(mcx + cosf(a) * rt, mby + sinf(a) * rt);
            }

            glVertex2f(mr2, mby);
            glVertex2f(mr2, mby - corpo + 8.0f);

            for (int i = 0; i <= seg; i++) {

                float a = (float)i / seg * 3.14159f;
                glVertex2f(mcx + cosf(a) * rt, mby - corpo + sinf(a + 3.14159f) * 8.0f);
            }

            glVertex2f(ml, mby - corpo + 8.0f);

        glEnd();
        glLineWidth(1.0f);

        // linha divisória vertical
        glColor4f(0.92f, 0.92f, 0.97f, 0.95f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(mcx, mby - corpo + 6.0f);
            glVertex2f(mcx, mtop);
        glEnd();
        glLineWidth(1.0f);

        // scroll wheel
        float swCx = mcx;
        float swCy = mby + 2.0f;
        float swRx = 6.0f;
        float swRy = 10.0f;

        glColor4f(0.20f, 0.21f, 0.25f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(swCx, swCy);

            for (int i = 0; i <= seg; i++) {

                float a = (float)i / seg * 6.2832f;
                glVertex2f(swCx + cosf(a) * swRx, swCy + sinf(a) * swRy);
            }
        glEnd();

        glColor4f(0.92f, 0.92f, 0.97f, 1.0f);
        glLineWidth(2.5f);
        glBegin(GL_LINE_LOOP);

            for (int i = 0; i <= seg; i++) {

                float a = (float)i / seg * 6.2832f;
                glVertex2f(swCx + cosf(a) * swRx, swCy + sinf(a) * swRy);
            }
        glEnd();
        glLineWidth(1.0f);

        // legenda
        float legendaY1 = tecladoY + 60.0f;
        float legendaY2 = tecladoY + 40.0f;

        auto desenharLegendaMouse = [](float x, float y, const char* texto) {

            for (int dx = 0; dx <= 1; dx++) {
                glRasterPos2f(x + dx, y);

                for (int i = 0; texto[i] != '\0'; i++) {
                    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, texto[i]);  
                }
            }
        };

        glColor3f(1.0f, 0.45f, 0.05f);
        desenharLegendaMouse(legendaX, legendaY1, "MOUSE ESQUERDO ATIRAR");

        glColor3f(0.25f, 0.65f, 1.0f);
        desenharLegendaMouse(legendaX, legendaY2, "MOUSE DIREITO MOVER CAMERA");
    }

    // Tecla E e ESC 
    float colEsq = janelaX + 100.0f;
    float linhaM = janelaY + 125.0f;
    float linhaEsc = janelaY + 66.0f;

    desenharTecla(colEsq, linhaM, 58.0f, 38.0f, "E");
    glColor3f(0.88f, 0.92f, 1.0f);
    escreverTexto(colEsq + 78.0f, linhaM + 12.0f, "Ativar / desativar mira");

    desenharTecla(colEsq, linhaEsc, 58.0f, 38.0f, "ESC");
    glColor3f(0.88f, 0.92f, 1.0f);
    escreverTexto(colEsq + 78.0f, linhaEsc + 12.0f, "Sair do jogo");

    //Vida 
    float vidaX = janelaX + 520.0f;
    float vidaY = janelaY + 72.0f;

    glColor3f(1.0f, 1.0f, 1.0f);
    escreverTexto(vidaX, vidaY + 55.0f, "VIDA DO JOGADOR");

    desenharRetangulo(vidaX, vidaY + 18.0f, 210.0f, 24.0f, 0.12f, 0.12f, 0.12f);

    // barra de vida com leve gradiente
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.85f, 0.25f);
        glVertex2f(vidaX, vidaY + 18.0f);
        glVertex2f(vidaX + 210.0f, vidaY + 18.0f);
        glColor3f(0.0f, 0.6f, 0.18f);
        glVertex2f(vidaX + 210.0f, vidaY + 42.0f);
        glVertex2f(vidaX, vidaY + 42.0f);
    glEnd();

    desenharContorno(vidaX, vidaY + 18.0f, 210.0f, 24.0f, 1.0f, 1.0f, 1.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    escreverTexto(vidaX, vidaY - 10.0f, "100 / 100 pontos");

    // Rodapé com leve pulsar para chamar atencao
    float pulso = 0.7f + 0.3f * sinf(tempo * 2.5f);
    glColor4f(1.0f, 0.87f, 0.05f, pulso);
    escreverTextoCentralizado(janelaY + 27.0f, "PRESSIONE ENTER PARA INICIAR", GLUT_BITMAP_HELVETICA_18);

    {
        float rcx = janelaX + janelaLargura / 2.0f;
        float rcy = janelaY + 27.0f;
        float largaroTexto = 230.0f * pulso + 30.0f;

        glColor4f(1.0f, 0.87f, 0.05f, pulso * 0.7f);
        glBegin(GL_LINES);
            glVertex2f(rcx - largaroTexto, rcy);
            glVertex2f(rcx - largaroTexto + 14.0f, rcy);
            glVertex2f(rcx - largaroTexto + 14.0f, rcy);
            glVertex2f(rcx - largaroTexto + 8.0f, rcy - 6.0f);

            glVertex2f(rcx + largaroTexto, rcy);
            glVertex2f(rcx + largaroTexto - 14.0f, rcy);
            glVertex2f(rcx + largaroTexto - 14.0f, rcy);
            glVertex2f(rcx + largaroTexto - 8.0f, rcy - 6.0f);
        glEnd();
    }

    restaurarTela3D();
}

void Menu::desenharBarraVida(int vidaAtual) {
    int largura = glutGet(GLUT_WINDOW_WIDTH);
    int altura = glutGet(GLUT_WINDOW_HEIGHT);

    if (vidaAtual < 0) vidaAtual = 0;
    if (vidaAtual > vidaMaxima) vidaAtual = vidaMaxima;

    prepararTela2D(largura, altura);

    float x = 25.0f;
    float y = altura - 55.0f;
    float larguraBarra = 230.0f;
    float alturaBarra = 22.0f;
    float porcentagem = (float)vidaAtual / (float)vidaMaxima;

    desenharRetanguloAlpha(x - 10.0f, y - 12.0f, larguraBarra + 20.0f, 62.0f, 0.02f, 0.02f, 0.03f, 0.85f);
    desenharContorno(x - 10.0f, y - 12.0f, larguraBarra + 20.0f, 62.0f, 0.65f, 0.65f, 0.75f);
    desenharCantos(x - 10.0f, y - 12.0f, larguraBarra + 20.0f, 62.0f, 10.0f, 1.0f, 0.87f, 0.05f, 0.6f);

    char texto[32];
    snprintf(texto, sizeof(texto), "VIDA %d/%d", vidaAtual, vidaMaxima);

    glColor3f(1.0f, 1.0f, 1.0f);
    escreverTexto(x, y + 32.0f, texto);

    desenharRetangulo(x, y, larguraBarra, alturaBarra, 0.12f, 0.12f, 0.12f);

    // cor da barra reage ao nivel de vida
    float corR, corG;
    if (porcentagem > 0.5f) {
        corR = 0.0f + (1.0f - porcentagem) * 2.0f * 0.9f;
        corG = 0.75f;
    } else {
        corR = 0.9f;
        corG = porcentagem * 2.0f * 0.75f;
    }

    glBegin(GL_QUADS);
        glColor3f(corR, corG, 0.18f);
        glVertex2f(x, y);
        glVertex2f(x + larguraBarra * porcentagem, y);
        glColor3f(corR * 0.7f, corG * 0.7f, 0.12f);
        glVertex2f(x + larguraBarra * porcentagem, y + alturaBarra);
        glVertex2f(x, y + alturaBarra);
    glEnd();

    desenharContorno(x, y, larguraBarra, alturaBarra, 1.0f, 1.0f, 1.0f);

    restaurarTela3D();
}

