#include "TelaFinal.h"
#include <GL/glut.h>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdlib>

TelaFinal::TelaFinal(){
    ativa = false;
    venceu = false;

    troopersMortos = 0;
    vidaRestante = 0;
    tempoFinal = 0;

    tempoAtivacao = 0.0f;
    estrelasGeradas = false;
}

void TelaFinal::iniciar(){
    inicioJogo =
        std::chrono::steady_clock::now();
}

void TelaFinal::gerarEstrelas(){
    estrelas.clear();

    for (int i = 0; i < 200; i++){
        Estrela e;
        e.x = (float)(rand() % 2000 - 1000) / 1000.0f; 
        e.y = (float)(rand() % 2000 - 1000) / 1000.0f;
        e.profundidade = (float)(rand() % 100) / 100.0f;
        e.brilho = 0.25f + (rand() % 100) / 100.0f * 0.6f;
        e.tamanho = 1.0f + e.profundidade * 2.2f;
        e.fase = (rand() % 100) / 100.0f * 6.28f;
        estrelas.push_back(e);
    }

    estrelasGeradas = true;
}

void TelaFinal::mostrarVitoria(int troopers, int vida){
    ativa = true;
    venceu = true;

    troopersMortos = troopers;
    vidaRestante = vida;
    auto agora = std::chrono::steady_clock::now();

    tempoFinal = (int)std::chrono::duration_cast<std::chrono::seconds>(agora - inicioJogo).count();
    tempoAtivacao = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    if (!estrelasGeradas){
        gerarEstrelas();
    }     
}   

void TelaFinal::mostrarDerrota(int troopers, int vida){
    ativa = true;
    venceu = false;

    troopersMortos = troopers;
    vidaRestante = vida;
    auto agora = std::chrono::steady_clock::now();

    tempoFinal =(int)std::chrono::duration_cast<std::chrono::seconds>(agora - inicioJogo).count();
    tempoAtivacao = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    if (!estrelasGeradas){
        gerarEstrelas();
    }
}

bool TelaFinal::estaAtiva() const{
    return ativa;
}

static void desenharTexto(float x, float y, const std::string& texto, void* fonte = GLUT_BITMAP_HELVETICA_18){
    glRasterPos2f(x, y);

    for(char c : texto){
        glutBitmapCharacter(fonte, c);
    }
}

// Centralizar
static float larguraTexto(const std::string& texto, void* fonte = GLUT_BITMAP_HELVETICA_18){
    float total = 0;

    for (char c : texto){
        total += glutBitmapWidth(fonte, c);
    }

    return total;
}

static void desenharTextoCentralizado(float centroX,float y,const std::string& texto,void* fonte = GLUT_BITMAP_HELVETICA_18){
    float w = larguraTexto(texto, fonte);
    desenharTexto(centroX - w / 2.0f, y, texto, fonte);
}

static void desenharTextoComBrilho(float centroX,float y,const std::string& texto,float r, float g, float b,float intensidade,void* fonte = GLUT_BITMAP_HELVETICA_18){
    float w = larguraTexto(texto, fonte);
    float x = centroX - w / 2.0f;

    glColor4f(r, g, b, 0.16f * intensidade);
    for (int dx = -3; dx <= 3; dx++)//mayra
    for (int dy = -3; dy <= 3; dy++)
    {
        if (dx == 0 && dy == 0) continue;
        desenharTexto(x + dx, y + dy, texto, fonte);
    }

    glColor4f(r, g, b, intensidade);
    desenharTexto(x, y, texto, fonte);
}

static void desenharTextoEspacado(float centroX,float y,const std::string& texto,float espacoExtra,void* fonte = GLUT_BITMAP_HELVETICA_10){
    int n = (int)texto.size();
    float largura = larguraTexto(texto, fonte) + espacoExtra * (n - 1);
    float x = centroX - largura / 2.0f;

    for (char c : texto){
        std::string s(1, c);
        desenharTexto(x, y, s, fonte);
        x += glutBitmapWidth(fonte, c) + espacoExtra;
    }
}

// pequeno icone de "caveira/alvo" 

static void desenharIconeAlvo(float cx, float cy, float r, float alpha){
    glColor4f(1.0f, 1.0f, 1.0f, alpha);
    int passos = 16;

    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < passos; i++){
            float ang = (float)i / passos * 6.2832f;
            glVertex2f(cx + cosf(ang) * r, cy + sinf(ang) * r);
        }
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(cx - r * 0.6f, cy - r * 0.6f);
        glVertex2f(cx + r * 0.6f, cy + r * 0.6f);
        glVertex2f(cx - r * 0.6f, cy + r * 0.6f);
        glVertex2f(cx + r * 0.6f, cy - r * 0.6f);
    glEnd();
}

// pequeno icone de "coracao/vida" (losango)
static void desenharIconeVida(float cx, float cy, float r, float alpha){
    glColor4f(0.9f, 0.25f, 0.25f, alpha);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx, cy + r);
        glVertex2f(cx + r, cy);
        glVertex2f(cx, cy - r);
        glVertex2f(cx - r, cy);
    glEnd();
}

// pequeno icone de "relogio" (circulo com ponteiro)
static void desenharIconeTempo(float cx, float cy, float r, float alpha){
    glColor4f(0.6f, 0.8f, 1.0f, alpha);
    int passos = 16;
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < passos; i++){
            float ang = (float)i / passos * 6.2832f;
            glVertex2f(cx + cosf(ang) * r, cy + sinf(ang) * r);
        }
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(cx, cy);
        glVertex2f(cx, cy + r * 0.6f);
        glVertex2f(cx, cy);
        glVertex2f(cx + r * 0.5f, cy);
    glEnd();
}

// Desenho Princiapl

void TelaFinal::desenhar(){
    int largura = glutGet(GLUT_WINDOW_WIDTH);
    int altura =glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D(0,largura,0,altura);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float tempo = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float decorrido = tempo - tempoAtivacao;

    // animacao de entrada
    float entrada = decorrido / 1.2f;
    if (entrada > 1.0f) entrada = 1.0f;
    if (entrada < 0.0f) entrada = 0.0f;
    float entradaSuave = 1.0f - (1.0f - entrada) * (1.0f - entrada);

    float centroX = largura / 2.0f;
    float centroY = altura / 2.0f;

    // cores-tema de acordo com vitoria/derrota
    float temaR = venceu ? 1.0f : 0.95f;
    float temaG = venceu ? 0.85f : 0.18f;
    float temaB = venceu ? 0.2f  : 0.12f;

    //Fundo
    float corTopoR, corTopoG, corTopoB;

    if (venceu){
        corTopoR = 0.04f; corTopoG = 0.05f; corTopoB = 0.11f;
    }else{
        corTopoR = 0.12f; corTopoG = 0.015f; corTopoB = 0.015f;
    }

    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(0, 0);
        glVertex2f(largura, 0);
        glColor3f(corTopoR, corTopoG, corTopoB);
        glVertex2f(largura, centroY);
        glVertex2f(0, centroY);
    glEnd();

    glBegin(GL_QUADS);
        glColor3f(corTopoR, corTopoG, corTopoB);
        glVertex2f(0, centroY);
        glVertex2f(largura, centroY);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(largura, altura);
        glVertex2f(0, altura);
    glEnd();

    {
        float hx = centroX;
        float hy = altura - altura * 0.20f;
        float raioMax = largura * 0.42f;
        int passos = 24;

        glBegin(GL_TRIANGLE_FAN);
            glColor4f(temaR, temaG, temaB, 0.10f);
            glVertex2f(hx, hy);
            glColor4f(temaR, temaG, temaB, 0.0f);
            for (int i = 0; i <= passos; i++)
            {
                float ang = (float)i / passos * 6.2832f;
                glVertex2f(hx + cosf(ang) * raioMax, hy + sinf(ang) * raioMax * 0.6f);
            }
        glEnd();
    }

    // ---------- Campo de estrelas com parallax ----------
    for (const auto& e : estrelas){
        float deriva = tempo * (0.01f + e.profundidade * 0.04f);

        float ex = e.x + sinf(e.fase) * 0.02f;
        float ey = e.y - deriva; // deslocamento lento para baixo (ruina caindo)

        if (ey < -1.0f) ey += 2.0f;

        float px = (ex * 0.5f + 0.5f) * largura;
        float py = (ey * 0.5f + 0.5f) * altura;

        float pulso = 0.5f + 0.5f * sinf(tempo * (1.2f + e.profundidade * 2.0f) + e.fase);
        float brilhoFinal = e.brilho * (0.55f + 0.45f * pulso);

        float r = 0.85f + e.profundidade * 0.15f;
        float g = 0.88f + e.profundidade * 0.12f;
        float b = 1.0f;

        glPointSize(e.tamanho);
        glBegin(GL_POINTS);
            glColor4f(r, g, b, brilhoFinal);
            glVertex2f(px, py);
        glEnd();
    }

    {
        float raio = largura * 0.16f;
        float px = largura + raio * 0.55f;
        float py = raio * 0.55f; // perto da base, pois aqui y cresce para cima
        int passos = 40;

        glBegin(GL_TRIANGLE_FAN);
            glColor4f(0.10f, 0.11f, 0.16f, 0.9f);
            glVertex2f(px, py);
            for (int i = 0; i <= passos; i++)
            {
                float ang = (float)i / passos * 6.2832f;
                glVertex2f(px + cosf(ang) * raio, py + sinf(ang) * raio);
            }
        glEnd();

        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            glColor4f(temaR * 0.6f, temaG * 0.6f + 0.2f, temaB * 0.6f + 0.3f, 0.45f);
            for (int i = 0; i <= passos; i++)
            {
                float ang = (float)i / passos * 6.2832f;
                glVertex2f(px + cosf(ang) * raio, py + sinf(ang) * raio);
            }
        glEnd();
        glLineWidth(1.0f);
    }

    // ---------- Destroco / nave caida cruzando o ceu lentamente ----------
    {
        glPushMatrix();
        float dx = fmodf(tempo * 18.0f, largura + 300.0f) - 150.0f;
        float dy = altura * 0.78f - tempo * 4.0f;
        // mantem a "queda" dentro de uma faixa, reciclando
        dy = altura * 0.6f + fmodf(altura * 1.4f - tempo * 6.0f, altura * 0.8f);

        glTranslatef(dx, dy, 0);
        glRotatef(tempo * 25.0f, 0, 0, 1);

        glColor4f(0.5f, 0.5f, 0.55f, venceu ? 0.25f : 0.4f);
        glBegin(GL_TRIANGLES);
            glVertex2f(14, 0);
            glVertex2f(-10, -4);
            glVertex2f(-10, 4);
        glEnd();

        if (!venceu)
        {
            // rastro de fumaca/fogo (apenas na derrota)
            glBegin(GL_TRIANGLES);
                glColor4f(0.9f, 0.4f, 0.1f, 0.3f);
                glVertex2f(-10, -3);
                glVertex2f(-10, 3);
                glVertex2f(-30, 0);
            glEnd();
        }

        glPopMatrix();
    }

    // ---------- Linhas de HUD decorativas (cantos da tela) ----------
    glLineWidth(1.5f);
    glColor4f(temaR * 0.6f, temaG * 0.6f + 0.1f, temaB * 0.6f + 0.2f, 0.35f);
    glBegin(GL_LINES);
        glVertex2f(30, altura - 30);
        glVertex2f(90, altura - 30);
        glVertex2f(30, altura - 30);
        glVertex2f(30, altura - 70);

        glVertex2f(largura - 30, altura - 30);
        glVertex2f(largura - 90, altura - 30);
        glVertex2f(largura - 30, altura - 30);
        glVertex2f(largura - 30, altura - 70);

        glVertex2f(30, 30);
        glVertex2f(90, 30);
        glVertex2f(30, 30);
        glVertex2f(30, 70);

        glVertex2f(largura - 30, 30);
        glVertex2f(largura - 90, 30);
        glVertex2f(largura - 30, 30);
        glVertex2f(largura - 30, 70);
    glEnd();
    glLineWidth(1.0f);

    // ---------- Linha decorativa acima do subtitulo ----------
    {
        float y = altura - 70;
        float meiaLargura = 140.0f * entradaSuave;
        glBegin(GL_LINES);
            glColor4f(temaR, temaG, temaB, 0.6f * entradaSuave);
            glVertex2f(centroX - meiaLargura, y);
            glVertex2f(centroX + meiaLargura, y);
        glEnd();
        glBegin(GL_LINES);
            glVertex2f(centroX - meiaLargura, y);
            glVertex2f(centroX - meiaLargura, y - 8);
            glVertex2f(centroX + meiaLargura, y);
            glVertex2f(centroX + meiaLargura, y - 8);
        glEnd();
    }

    // ---------- Subtitulo estilo "comunicado" ----------
    glColor4f(0.7f, 0.78f, 1.0f, 0.8f * entradaSuave);
    desenharTextoEspacado(
        centroX,
        altura - 85,
        venceu ? "TRANSMISSAO DA ALIANCA REBELDE" : "TRANSMISSAO INTERROMPIDA",
        2.0f
    );

    // ---------- Titulo principal com brilho ----------
    float deslocamentoTitulo = (1.0f - entradaSuave) * 18.0f;

    if (venceu)
    {
        // ======= TELA DE VITORIA CAPRICHADA =======

        // Raios de luz dourados emanando do centro (estrela de vitoria)
        {
            float cx = centroX;
            float cy = altura - 130.0f + deslocamentoTitulo;
            float raioMax = 280.0f;
            int numRaios = 18;
            float rotRaios = fmodf(tempo * 8.0f, 360.0f);

            for (int i = 0; i < numRaios; i++) {
                float ang = (float)i / numRaios * 6.2832f + rotRaios * 3.14159f / 180.0f;
                float largRaio = (i % 2 == 0) ? 0.06f : 0.025f;
                float pulso = 0.5f + 0.5f * sinf(tempo * 2.5f + i * 0.7f);

                glBegin(GL_TRIANGLES);
                    glColor4f(1.0f, 0.92f, 0.3f, 0.18f * entradaSuave * pulso);
                    glVertex2f(cx, cy);
                    glColor4f(1.0f, 0.85f, 0.1f, 0.0f);
                    glVertex2f(cx + cosf(ang - largRaio) * raioMax, cy + sinf(ang - largRaio) * raioMax * 0.55f);
                    glVertex2f(cx + cosf(ang + largRaio) * raioMax, cy + sinf(ang + largRaio) * raioMax * 0.55f);
                glEnd();
            }
        }

        // Particulas de confete/estrelinhas celebrando (so na vitoria)
        {
            float semente = floorf(decorrido * 2.0f);
            for (int i = 0; i < 24; i++) {
                float t2 = fmodf(decorrido * 0.7f + i * 0.18f, 1.8f);
                float px = centroX + cosf(i * 1.618f * 6.2832f) * (100.0f + i * 14.0f) * t2;
                float py = (altura - 90.0f) - t2 * 280.0f - 40.0f * t2 * t2;
                float alpha = (1.0f - t2 / 1.8f) * entradaSuave;
                float r = (i % 3 == 0) ? 1.0f : (i % 3 == 1) ? 0.3f : 1.0f;
                float g = (i % 3 == 0) ? 0.9f : (i % 3 == 1) ? 1.0f : 0.3f;
                float b = (i % 3 == 0) ? 0.1f : (i % 3 == 1) ? 0.4f : 1.0f;
                float sz = 3.0f + (i % 4) * 2.0f;
                glColor4f(r, g, b, alpha);
                glPointSize(sz);
                glBegin(GL_POINTS);
                    glVertex2f(px, py);
                glEnd();
            }
            glPointSize(1.0f);
        }

        // Circulo de aura dourada pulsante atras do titulo
        {
            float pulsoAura = 0.85f + 0.15f * sinf(tempo * 3.2f);
            float raioAura = 120.0f * pulsoAura;
            float cx = centroX;
            float cy = altura - 130.0f + deslocamentoTitulo;
            int passos = 32;
            glBegin(GL_TRIANGLE_FAN);
                glColor4f(1.0f, 0.87f, 0.05f, 0.22f * entradaSuave);
                glVertex2f(cx, cy);
                glColor4f(1.0f, 0.75f, 0.0f, 0.0f);
                for (int i = 0; i <= passos; i++) {
                    float ang = (float)i / passos * 6.2832f;
                    glVertex2f(cx + cosf(ang) * raioAura, cy + sinf(ang) * raioAura * 0.5f);
                }
            glEnd();
        }

        // Linha horizontal decorativa dupla acima do titulo
        {
            float ly = altura - 125.0f + deslocamentoTitulo;
            float hw = 180.0f * entradaSuave;
            glLineWidth(3.0f);
            glColor4f(1.0f, 0.87f, 0.05f, 0.9f * entradaSuave);
            glBegin(GL_LINES);
                glVertex2f(centroX - hw, ly);
                glVertex2f(centroX + hw, ly);
            glEnd();
            glLineWidth(1.0f);
            glColor4f(1.0f, 0.87f, 0.05f, 0.4f * entradaSuave);
            glBegin(GL_LINES);
                glVertex2f(centroX - hw * 0.7f, ly - 5.0f);
                glVertex2f(centroX + hw * 0.7f, ly - 5.0f);
            glEnd();
        }

        // Titulo "MISSAO CONCLUIDA" com multiplas camadas de glow
        float pulsoTitulo = 0.88f + 0.12f * sinf(tempo * 2.0f);

        // Camada de glow externo laranja
        for (int dx = -5; dx <= 5; dx += 2)
        for (int dy = -5; dy <= 5; dy += 2) {
            if (dx == 0 && dy == 0) continue;
            float dist = sqrtf((float)(dx*dx + dy*dy));
            glColor4f(1.0f, 0.6f, 0.0f, 0.06f * entradaSuave * pulsoTitulo / (dist * 0.3f + 0.5f));
            float w2 = larguraTexto("MISSAO CONCLUIDA", GLUT_BITMAP_TIMES_ROMAN_24);
            glRasterPos2f(centroX - w2 / 2.0f + dx, altura - 150 + deslocamentoTitulo + dy);
            for (char c : std::string("MISSAO CONCLUIDA")) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
        }
        // Camada dourada brilhante principal
        desenharTextoComBrilho(
            centroX,
            altura - 150 + deslocamentoTitulo,
            "MISSAO CONCLUIDA",
            1.0f, 0.93f, 0.15f,
            entradaSuave * pulsoTitulo,
            GLUT_BITMAP_TIMES_ROMAN_24
        );

        // Subtitulo em branco-azulado
        glColor4f(0.9f, 0.96f, 1.0f, 0.92f * entradaSuave);
        desenharTextoCentralizado(
            centroX,
            altura - 190 + deslocamentoTitulo,
            "A REBELIAO TRIUNFOU",
            GLUT_BITMAP_HELVETICA_18
        );

        // Linha horizontal decorativa abaixo do subtitulo
        {
            float ly = altura - 205.0f + deslocamentoTitulo;
            float hw = 130.0f * entradaSuave;
            glLineWidth(1.5f);
            glColor4f(0.9f, 0.96f, 1.0f, 0.45f * entradaSuave);
            glBegin(GL_LINES);
                glVertex2f(centroX - hw, ly);
                glVertex2f(centroX + hw, ly);
            glEnd();
            glLineWidth(1.0f);
        }

        // Estrelas de conquista (3 estrelas no estilo achievement)
        {
            float sy = altura - 230.0f + deslocamentoTitulo;
            float aparecimento = (decorrido - 0.5f) / 0.8f;
            if (aparecimento > 1.0f) aparecimento = 1.0f;
            if (aparecimento < 0.0f) aparecimento = 0.0f;

            float posEstrelas[3] = { centroX - 55.0f, centroX, centroX + 55.0f };
            for (int s = 0; s < 3; s++) {
                float atrasoE = (decorrido - 0.3f - s * 0.25f) / 0.5f;
                if (atrasoE > 1.0f) atrasoE = 1.0f;
                if (atrasoE < 0.0f) atrasoE = 0.0f;

                float pulsoStar = 0.85f + 0.15f * sinf(tempo * 3.0f + s * 2.1f);
                float sc = 12.0f * atrasoE * pulsoStar;
                float ex = posEstrelas[s];

                // Estrela de 5 pontas
                glColor4f(1.0f, 0.88f, 0.0f, atrasoE);
                glBegin(GL_TRIANGLE_FAN);
                    glVertex2f(ex, sy);
                    for (int p = 0; p <= 10; p++) {
                        float ang = (float)p / 10 * 6.2832f - 1.5708f;
                        float r2 = (p % 2 == 0) ? sc : sc * 0.42f;
                        glVertex2f(ex + cosf(ang) * r2, sy + sinf(ang) * r2);
                    }
                glEnd();

                // contorno dourado
                glColor4f(1.0f, 0.7f, 0.0f, atrasoE * 0.8f);
                glLineWidth(1.2f);
                glBegin(GL_LINE_LOOP);
                    for (int p = 0; p < 10; p++) {
                        float ang = (float)p / 10 * 6.2832f - 1.5708f;
                        float r2 = (p % 2 == 0) ? sc : sc * 0.42f;
                        glVertex2f(ex + cosf(ang) * r2, sy + sinf(ang) * r2);
                    }
                glEnd();
                glLineWidth(1.0f);
            }
        }
    }
    else
    {
        // ======= GAME OVER GIGANTE E CHAMATIVO =======

        // Flash vermelho de entrada
        if (decorrido < 0.4f) {
            float flashAlpha = (0.4f - decorrido) / 0.4f * 0.6f;
            glColor4f(0.9f, 0.05f, 0.05f, flashAlpha);
            glBegin(GL_QUADS);
                glVertex2f(0, 0);
                glVertex2f((float)largura, 0);
                glVertex2f((float)largura, (float)altura);
                glVertex2f(0, (float)altura);
            glEnd();
        }

        // Raios vermelhos emanando do centro
        {
            float cx = centroX;
            float cy = altura - 145.0f + deslocamentoTitulo;
            float raioMax = 320.0f;
            int numRaios = 16;
            float rotRaios = fmodf(tempo * 4.0f, 360.0f);

            for (int i = 0; i < numRaios; i++) {
                float ang = (float)i / numRaios * 6.2832f + rotRaios * 3.14159f / 180.0f;
                float pulso = 0.5f + 0.5f * sinf(tempo * 1.8f + i * 0.5f);

                glBegin(GL_TRIANGLES);
                    glColor4f(0.9f, 0.08f, 0.08f, 0.20f * entradaSuave * pulso);
                    glVertex2f(cx, cy);
                    glColor4f(0.8f, 0.0f, 0.0f, 0.0f);
                    glVertex2f(cx + cosf(ang - 0.05f) * raioMax, cy + sinf(ang - 0.05f) * raioMax * 0.5f);
                    glVertex2f(cx + cosf(ang + 0.05f) * raioMax, cy + sinf(ang + 0.05f) * raioMax * 0.5f);
                glEnd();
            }
        }

        // Aura vermelha pulsante atras do Game Over
        {
            float pulsoV = 0.8f + 0.2f * sinf(tempo * 4.0f);
            float cx = centroX;
            float cy = altura - 145.0f + deslocamentoTitulo;
            int passos = 32;
            glBegin(GL_TRIANGLE_FAN);
                glColor4f(0.9f, 0.05f, 0.05f, 0.30f * entradaSuave * pulsoV);
                glVertex2f(cx, cy);
                glColor4f(0.8f, 0.0f, 0.0f, 0.0f);
                for (int i = 0; i <= passos; i++) {
                    float ang = (float)i / passos * 6.2832f;
                    glVertex2f(cx + cosf(ang) * 160.0f * pulsoV, cy + sinf(ang) * 80.0f * pulsoV);
                }
            glEnd();
        }

        // Linhas horizontais de escaneamento vermelhas no titulo
        {
            float scanAlpha = 0.4f + 0.4f * sinf(tempo * 8.0f);
            float lyBase = altura - 115.0f + deslocamentoTitulo;
            glColor4f(0.9f, 0.1f, 0.1f, scanAlpha * entradaSuave);
            glLineWidth(1.0f);
            for (float scan = lyBase; scan < lyBase + 60.0f; scan += 6.0f) {
                glBegin(GL_LINES);
                    glVertex2f(centroX - 160.0f, scan);
                    glVertex2f(centroX + 160.0f, scan);
                glEnd();
            }
        }

        // GAME OVER em tamanho maximo com multiplas camadas de glow
        float pulsoGO = 0.9f + 0.1f * sinf(tempo * 5.0f);
        float escalaGO = 1.0f + 0.015f * sinf(tempo * 3.5f); // leve pulsacao de escala via offset

        // Glow externo vermelho sangue (espalhado)
        for (int dx = -8; dx <= 8; dx += 2)
        for (int dy = -8; dy <= 8; dy += 2) {
            if (dx == 0 && dy == 0) continue;
            float dist = sqrtf((float)(dx*dx + dy*dy));
            glColor4f(0.95f, 0.0f, 0.0f, 0.08f * entradaSuave * pulsoGO / (dist * 0.25f + 0.4f));
            float w2 = larguraTexto("GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
            glRasterPos2f(centroX - w2 / 2.0f + dx, altura - 150 + deslocamentoTitulo + dy);
            for (char c : std::string("GAME OVER")) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
        }

        // Camada laranja intermediaria
        for (int dx = -3; dx <= 3; dx++)
        for (int dy = -3; dy <= 3; dy++) {
            if (dx == 0 && dy == 0) continue;
            glColor4f(1.0f, 0.3f, 0.0f, 0.12f * entradaSuave);
            float w2 = larguraTexto("GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
            glRasterPos2f(centroX - w2 / 2.0f + dx, altura - 150 + deslocamentoTitulo + dy);
            for (char c : std::string("GAME OVER")) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
        }

        // Texto principal em branco-laranja brilhante
        desenharTextoComBrilho(
            centroX,
            altura - 150 + deslocamentoTitulo,
            "GAME OVER",
            1.0f, 0.22f, 0.08f,
            entradaSuave * pulsoGO,
            GLUT_BITMAP_TIMES_ROMAN_24
        );

        // Linha decorativa abaixo do game over
        {
            float ly = altura - 175.0f + deslocamentoTitulo;
            float hw = 140.0f * entradaSuave;
            float pulsoLinha = 0.5f + 0.5f * sinf(tempo * 4.5f);
            glLineWidth(2.5f);
            glColor4f(0.95f, 0.15f, 0.08f, 0.9f * entradaSuave * pulsoLinha);
            glBegin(GL_LINES);
                glVertex2f(centroX - hw, ly);
                glVertex2f(centroX + hw, ly);
            glEnd();
            glLineWidth(1.0f);
        }

        glColor4f(0.95f, 0.65f, 0.65f, 0.92f * entradaSuave);
        desenharTextoCentralizado(
            centroX,
            altura - 200 + deslocamentoTitulo,
            "O IMPERIO VENCEU ESTA BATALHA",
            GLUT_BITMAP_HELVETICA_18
        );
    }

    // ---------- Painel de estatisticas ----------
    float painelLargura = 380.0f;
    float painelAltura  = 165.0f;
    float painelX = centroX - painelLargura / 2.0f;
    float painelY = centroY - painelAltura / 2.0f;

    float alphaPainel = entradaSuave;

    // fundo translucido do painel
    glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f * alphaPainel);
        glVertex2f(painelX, painelY);
        glVertex2f(painelX + painelLargura, painelY);
        glVertex2f(painelX + painelLargura, painelY + painelAltura);
        glVertex2f(painelX, painelY + painelAltura);
    glEnd();

    // scanlines sutis dentro do painel
    glBegin(GL_LINES);
        glColor4f(1.0f, 1.0f, 1.0f, 0.04f * alphaPainel);
        for (float ly = painelY + 8; ly < painelY + painelAltura; ly += 7.0f)
        {
            glVertex2f(painelX + 4, ly);
            glVertex2f(painelX + painelLargura - 4, ly);
        }
    glEnd();

    // borda do painel
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glColor4f(temaR, temaG, temaB, 0.85f * alphaPainel);
        glVertex2f(painelX, painelY);
        glVertex2f(painelX + painelLargura, painelY);
        glVertex2f(painelX + painelLargura, painelY + painelAltura);
        glVertex2f(painelX, painelY + painelAltura);
    glEnd();

    // cantos decorativos do painel
    float cantoTam = 14.0f;
    glBegin(GL_LINES);
        glColor4f(temaR, temaG, temaB, alphaPainel);

        glVertex2f(painelX - 4, painelY + painelAltura + 4);
        glVertex2f(painelX - 4 + cantoTam, painelY + painelAltura + 4);
        glVertex2f(painelX - 4, painelY + painelAltura + 4);
        glVertex2f(painelX - 4, painelY + painelAltura + 4 - cantoTam);

        glVertex2f(painelX + painelLargura + 4, painelY + painelAltura + 4);
        glVertex2f(painelX + painelLargura + 4 - cantoTam, painelY + painelAltura + 4);
        glVertex2f(painelX + painelLargura + 4, painelY + painelAltura + 4);
        glVertex2f(painelX + painelLargura + 4, painelY + painelAltura + 4 - cantoTam);

        glVertex2f(painelX - 4, painelY - 4);
        glVertex2f(painelX - 4 + cantoTam, painelY - 4);
        glVertex2f(painelX - 4, painelY - 4);
        glVertex2f(painelX - 4, painelY - 4 + cantoTam);

        glVertex2f(painelX + painelLargura + 4, painelY - 4);
        glVertex2f(painelX + painelLargura + 4 - cantoTam, painelY - 4);
        glVertex2f(painelX + painelLargura + 4, painelY - 4);
        glVertex2f(painelX + painelLargura + 4, painelY - 4 + cantoTam);
    glEnd();

    glLineWidth(1.0f);

    // titulo pequeno do painel
    glColor4f(temaR, temaG, temaB, 0.9f * alphaPainel);
    desenharTextoEspacado(
        centroX,
        painelY + painelAltura - 22,
        "RELATORIO DE MISSAO",
        2.0f
    );

    glBegin(GL_LINES);
        glColor4f(temaR, temaG, temaB, 0.4f * alphaPainel);
        glVertex2f(painelX + 20, painelY + painelAltura - 32);
        glVertex2f(painelX + painelLargura - 20, painelY + painelAltura - 32);
    glEnd();

    // ---------- Linhas de estatisticas, com icone a esquerda ----------
    float iconeX = painelX + 36;
    float textoX = painelX + 60;

    std::stringstream ss;

    glColor4f(1.0f, 1.0f, 1.0f, alphaPainel);

    float linha1Y = painelY + painelAltura - 65;
    desenharIconeAlvo(iconeX, linha1Y + 6, 8.0f, alphaPainel);
    ss << "Troopers eliminados: " << troopersMortos;
    desenharTexto(textoX, linha1Y, ss.str());

    float linha2Y = painelY + painelAltura - 100;
    desenharIconeVida(iconeX, linha2Y + 6, 8.0f, alphaPainel);
    ss.str(""); ss.clear();
    ss << "Vida restante: " << vidaRestante;
    desenharTexto(textoX, linha2Y, ss.str());

    float linha3Y = painelY + painelAltura - 135;
    desenharIconeTempo(iconeX, linha3Y + 6, 8.0f, alphaPainel);
    ss.str(""); ss.clear();
    ss << "Tempo sobrevivido: " << tempoFinal << " segundos";
    desenharTexto(textoX, linha3Y, ss.str());

    // ---------- Linha decorativa abaixo do painel ----------
    {
        float y = painelY - 25;
        float meiaLargura = 90.0f * entradaSuave;
        glBegin(GL_LINES);
            glColor4f(temaR * 0.7f, temaG * 0.7f + 0.1f, temaB * 0.7f + 0.2f, 0.4f * entradaSuave);
            glVertex2f(centroX - meiaLargura, y);
            glVertex2f(centroX + meiaLargura, y);
        glEnd();
    }

    // ---------- Instrucoes (com leve pulsar pra chamar atencao) ----------
    float pulsoInstrucao = 0.6f + 0.4f * sinf(tempo * 3.0f);

    glColor4f(1.0f, 1.0f, 1.0f, pulsoInstrucao * entradaSuave);
    desenharTextoCentralizado(
        centroX,
        140,
        "Pressione R para jogar novamente"
    );

    glColor4f(0.8f, 0.8f, 0.8f, pulsoInstrucao * entradaSuave);
    desenharTextoCentralizado(
        centroX,
        110,
        "Pressione ESC para sair"
    );

    glColor4f(0.4f, 0.42f, 0.5f, 0.7f * entradaSuave);
    desenharTextoCentralizado(
        centroX,
        50,
        "STAR DEFENSE  -  Fim de Transmissao",
        GLUT_BITMAP_HELVETICA_10
    );

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}