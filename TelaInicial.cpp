#include "TelaInicial.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

TelaInicial::TelaInicial()
{
    ativa = true;
    iniciarJogo = false;
    mouseSobreBotao = false;

    estrelasGeradas = false;
    tempoAtivacao = 0.0f;

    botaoX = 0;
    botaoY = 0;
    botaoLargura = 240.0f;
    botaoAltura = 60.0f;
}

void TelaInicial::gerarEstrelas()
{
    estrelas.clear();
    for (int i = 0; i < 220; i++)
    {
        EstrelaInicial e;
        e.x = (float)(rand() % 2000 - 1000) / 1000.0f; // -1..1
        e.y = (float)(rand() % 2000 - 1000) / 1000.0f;
        e.profundidade = (float)(rand() % 100) / 100.0f;
        e.brilho = 0.25f + (rand() % 100) / 100.0f * 0.6f;
        e.tamanho = 1.0f + e.profundidade * 2.2f;
        e.fase = (rand() % 100) / 100.0f * 6.28f;
        estrelas.push_back(e);
    }
    estrelasGeradas = true;
}

void TelaInicial::ativar()
{
    ativa = true;
    iniciarJogo = false;
    tempoAtivacao = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    if (!estrelasGeradas)
        gerarEstrelas();
}

bool TelaInicial::estaAtiva() const
{
    return ativa;
}

bool TelaInicial::deveIniciarJogo() const
{
    return iniciarJogo;
}

bool TelaInicial::pontoDentroDoBotao(int x, int y) const
{
    return x >= botaoX &&
           x <= botaoX + botaoLargura &&
           y >= botaoY &&
           y <= botaoY + botaoAltura;
}

void TelaInicial::clicar(int x, int y)
{
    if (!ativa)
        return;

    if (pontoDentroDoBotao(x, y))
    {
        iniciarJogo = true;
        ativa = false;
    }
}

void TelaInicial::moverMouse(int x, int y)
{
    mouseSobreBotao = pontoDentroDoBotao(x, y);
}

// ===================== utilidades de texto =====================

static void desenharTexto(
    float x,
    float y,
    const char* texto,
    void* fonte)
{
    glRasterPos2f(x, y);
    for (const char* c = texto; *c != '\0'; c++)
        glutBitmapCharacter(fonte, *c);
}

static float larguraTexto(const char* texto, void* fonte)
{
    float total = 0;
    for (const char* c = texto; *c != '\0'; c++)
        total += glutBitmapWidth(fonte, *c);
    return total;
}

static void desenharTextoCentralizado(
    float centroX,
    float y,
    const char* texto,
    void* fonte)
{
    float w = larguraTexto(texto, fonte);
    desenharTexto(centroX - w / 2.0f, y, texto, fonte);
}

static void desenharTextoComBrilho(
    float centroX,
    float y,
    const char* texto,
    float r, float g, float b,
    float intensidade,
    void* fonte)
{
    float w = larguraTexto(texto, fonte);
    float x = centroX - w / 2.0f;

    glColor4f(r, g, b, 0.16f * intensidade);
    for (int dx = -3; dx <= 3; dx++)
    for (int dy = -3; dy <= 3; dy++)
    {
        if (dx == 0 && dy == 0) continue;
        desenharTexto(x + dx, y + dy, texto, fonte);
    }

    glColor4f(r, g, b, intensidade);
    desenharTexto(x, y, texto, fonte);
}

// também desenha o texto espaçado entre as letras (efeito "letreiro" sci-fi)
static void desenharTextoEspacado(
    float centroX,
    float y,
    const char* texto,
    float espacoExtra,
    void* fonte)
{
    // calcula largura total com espaco extra entre caracteres
    int n = 0;
    for (const char* c = texto; *c != '\0'; c++) n++;

    float largura = larguraTexto(texto, fonte) + espacoExtra * (n - 1);
    float x = centroX - largura / 2.0f;

    for (const char* c = texto; *c != '\0'; c++)
    {
        char buf[2] = { *c, '\0' };
        desenharTexto(x, y, buf, fonte);
        x += glutBitmapWidth(fonte, *c) + espacoExtra;
    }
}

// ===================== desenho principal =====================

void TelaInicial::desenhar()
{
    int largura = glutGet(GLUT_WINDOW_WIDTH);
    int altura  = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D(0, largura, altura, 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float tempo = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float decorrido = tempo - tempoAtivacao;

    // animacao de entrada: 0 no inicio, 1 apos ~1.4s
    float entrada = decorrido / 1.4f;
    if (entrada > 1.0f) entrada = 1.0f;
    if (entrada < 0.0f) entrada = 0.0f;
    // easing simples (suaviza o final)
    float entradaSuave = 1.0f - (1.0f - entrada) * (1.0f - entrada);

    float centroX = largura / 2.0f;
    float centroY = altura / 2.0f;

    // ---------- Fundo: gradiente espacial em 3 tons ----------
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(0, altura);
        glVertex2f(largura, altura);
        glColor3f(0.05f, 0.06f, 0.13f);
        glVertex2f(largura, centroY);
        glVertex2f(0, centroY);
    glEnd();

    glBegin(GL_QUADS);
        glColor3f(0.05f, 0.06f, 0.13f);
        glVertex2f(0, centroY);
        glVertex2f(largura, centroY);
        glColor3f(0.01f, 0.01f, 0.03f);
        glVertex2f(largura, 0);
        glVertex2f(0, 0);
    glEnd();

    // ---------- Holofote suave atras do titulo ----------
    {
        float hx = centroX;
        float hy = altura * 0.30f;
        float raioMax = largura * 0.42f;
        int passos = 24;

        glBegin(GL_TRIANGLE_FAN);
            glColor4f(0.35f, 0.4f, 0.65f, 0.10f);
            glVertex2f(hx, hy);
            glColor4f(0.35f, 0.4f, 0.65f, 0.0f);
            for (int i = 0; i <= passos; i++)
            {
                float ang = (float)i / passos * 6.2832f;
                glVertex2f(hx + cosf(ang) * raioMax, hy + sinf(ang) * raioMax * 0.6f);
            }
        glEnd();
    }

    // ---------- Campo de estrelas com parallax ----------
    for (const auto& e : estrelas)
    {
        float deriva = tempo * (0.01f + e.profundidade * 0.04f);

        float ex = e.x + sinf(e.fase) * 0.02f;
        float ey = e.y + deriva;

        // wrap vertical para a estrela "renascer" no topo ao saber da tela
        if (ey > 1.0f) ey -= 2.0f;

        float px = (ex * 0.5f + 0.5f) * largura;
        float py = (ey * 0.5f + 0.5f) * altura;

        float pulso = 0.5f + 0.5f * sinf(tempo * (1.2f + e.profundidade * 2.0f) + e.fase);
        float brilhoFinal = e.brilho * (0.55f + 0.45f * pulso);

        // estrelas mais profundas (frente) ficam levemente azuladas, fundo mais branco
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
        float py = altura + raio * 0.55f;
        int passos = 40;

        glBegin(GL_TRIANGLE_FAN);
            glColor4f(0.12f, 0.14f, 0.22f, 0.9f);
            glVertex2f(px, py);
            for (int i = 0; i <= passos; i++)
            {
                float ang = (float)i / passos * 6.2832f;
                glVertex2f(px + cosf(ang) * raio, py + sinf(ang) * raio);
            }
        glEnd();

        // brilho de borda (luz de um "sol" lateral)
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            glColor4f(0.5f, 0.55f, 0.8f, 0.5f);
            for (int i = 0; i <= passos; i++)
            {
                float ang = (float)i / passos * 6.2832f;
                glVertex2f(px + cosf(ang) * raio, py + sinf(ang) * raio);
            }
        glEnd();
        glLineWidth(1.0f);
    }

    // ---------- Naves estilizadas cruzando o ceu (silhueta tipo X-wing) ----------
    {
        auto desenharNave = [&](float baseX, float baseY, float escala, float corR, float corG, float corB, float alpha)
        {
            glPushMatrix();
            glTranslatef(baseX, baseY, 0);
            glScalef(escala, escala, 1.0f);

            glColor4f(corR, corG, corB, alpha);

            // fuselagem (triangulo alongado)
            glBegin(GL_TRIANGLES);
                glVertex2f(18, 0);
                glVertex2f(-14, -5);
                glVertex2f(-14, 5);
            glEnd();

            // asas em X
            glBegin(GL_LINES);
                glVertex2f(-6, 0);
                glVertex2f(-22, 14);
                glVertex2f(-6, 0);
                glVertex2f(-22, -14);
            glEnd();

            // rastro de propulsao
            glBegin(GL_TRIANGLES);
                glColor4f(corR, corG, corB, alpha * 0.4f);
                glVertex2f(-14, -3);
                glVertex2f(-14, 3);
                glVertex2f(-40, 0);
            glEnd();

            glPopMatrix();
        };

        float t1 = fmodf(tempo * 90.0f, largura + 200.0f) - 100.0f;
        desenharNave(t1, altura * 0.18f, 0.9f, 0.85f, 0.9f, 1.0f, 0.55f);

        float t2 = largura - fmodf(tempo * 60.0f + 150.0f, largura + 200.0f) + 100.0f;
        desenharNave(t2, altura * 0.62f, 0.6f, 0.8f, 0.85f, 1.0f, 0.35f);
    }

    // ---------- Linhas de HUD decorativas (cantos da tela) ----------
    glLineWidth(1.5f);
    glColor4f(0.4f, 0.6f, 1.0f, 0.35f);
    glBegin(GL_LINES);
        // canto superior esquerdo
        glVertex2f(30, 30);
        glVertex2f(90, 30);
        glVertex2f(30, 30);
        glVertex2f(30, 70);

        // canto superior direito
        glVertex2f(largura - 30, 30);
        glVertex2f(largura - 90, 30);
        glVertex2f(largura - 30, 30);
        glVertex2f(largura - 30, 70);

        // canto inferior esquerdo
        glVertex2f(30, altura - 30);
        glVertex2f(90, altura - 30);
        glVertex2f(30, altura - 30);
        glVertex2f(30, altura - 70);

        // canto inferior direito
        glVertex2f(largura - 30, altura - 30);
        glVertex2f(largura - 90, altura - 30);
        glVertex2f(largura - 30, altura - 30);
        glVertex2f(largura - 30, altura - 70);
    glEnd();
    glLineWidth(1.0f);

    // linha fina horizontal decorativa acima do titulo
    {
        float y = altura * 0.16f;
        float meiaLargura = 140.0f * entradaSuave;
        glBegin(GL_LINES);
            glColor4f(1.0f, 0.85f, 0.3f, 0.6f * entradaSuave);
            glVertex2f(centroX - meiaLargura, y);
            glVertex2f(centroX + meiaLargura, y);
        glEnd();

        // pequenos traços nas pontas (estilo "sabre")
        glBegin(GL_LINES);
            glVertex2f(centroX - meiaLargura, y);
            glVertex2f(centroX - meiaLargura, y - 8);
            glVertex2f(centroX + meiaLargura, y);
            glVertex2f(centroX + meiaLargura, y - 8);
        glEnd();
    }

    // ---------- Subtitulo de abertura, estilo "episodio" ----------
    glColor4f(0.7f, 0.78f, 1.0f, 0.8f * entradaSuave);
    desenharTextoEspacado(
        centroX,
        altura * 0.15f,
        "UMA NOVA AMEACA SE APROXIMA",
        2.0f,
        GLUT_BITMAP_HELVETICA_10
    );

    // ---------- Titulo principal com brilho (sobe levemente durante a entrada) ----------
    float deslocamentoTitulo = (1.0f - entradaSuave) * 20.0f;
    desenharTextoComBrilho(
        centroX,
        altura * 0.30f + deslocamentoTitulo,
        "STAR DEFENSE",
        1.0f, 0.85f, 0.2f,
        entradaSuave,
        GLUT_BITMAP_TIMES_ROMAN_24
    );

    glColor4f(0.8f, 0.85f, 1.0f, 0.85f * entradaSuave);
    desenharTextoCentralizado(
        centroX,
        altura * 0.30f + 34 + deslocamentoTitulo,
        "A galaxia precisa de voce",
        GLUT_BITMAP_HELVETICA_18
    );

    // ---------- Botao "Iniciar Jogo" ----------
    botaoLargura = 240.0f;
    botaoAltura  = 64.0f;
    botaoX = centroX - botaoLargura / 2.0f;
    botaoY = altura * 0.56f;

    float pulsoBotao = 0.7f + 0.3f * sinf(tempo * 2.5f);
    float alphaBotao = entradaSuave;

    // brilho externo (glow) atras do botao
    glBegin(GL_QUADS);
        float expand = mouseSobreBotao ? 14.0f : 8.0f;
        glColor4f(1.0f, 0.85f, 0.2f, (mouseSobreBotao ? 0.18f : 0.08f) * alphaBotao);
        glVertex2f(botaoX - expand, botaoY - expand);
        glVertex2f(botaoX + botaoLargura + expand, botaoY - expand);
        glVertex2f(botaoX + botaoLargura + expand, botaoY + botaoAltura + expand);
        glVertex2f(botaoX - expand, botaoY + botaoAltura + expand);
    glEnd();

    // fundo do botao
    glBegin(GL_QUADS);
        if (mouseSobreBotao)
            glColor4f(1.0f, 0.85f, 0.2f, 0.38f * alphaBotao);
        else
            glColor4f(1.0f, 0.85f, 0.2f, (0.14f * pulsoBotao + 0.08f) * alphaBotao);

        glVertex2f(botaoX, botaoY);
        glVertex2f(botaoX + botaoLargura, botaoY);
        glVertex2f(botaoX + botaoLargura, botaoY + botaoAltura);
        glVertex2f(botaoX, botaoY + botaoAltura);
    glEnd();

    // scanline sutil dentro do botao (linhas horizontais finas)
    glBegin(GL_LINES);
        glColor4f(1.0f, 1.0f, 1.0f, 0.06f * alphaBotao);
        for (float ly = botaoY + 6; ly < botaoY + botaoAltura; ly += 6.0f)
        {
            glVertex2f(botaoX + 2, ly);
            glVertex2f(botaoX + botaoLargura - 2, ly);
        }
    glEnd();

    // borda do botao
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        if (mouseSobreBotao)
            glColor4f(1.0f, 0.95f, 0.5f, alphaBotao);
        else
            glColor4f(1.0f, 0.85f, 0.2f, (0.7f + 0.3f * pulsoBotao) * alphaBotao);

        glVertex2f(botaoX, botaoY);
        glVertex2f(botaoX + botaoLargura, botaoY);
        glVertex2f(botaoX + botaoLargura, botaoY + botaoAltura);
        glVertex2f(botaoX, botaoY + botaoAltura);
    glEnd();
    glLineWidth(1.0f);

    // cantos decorativos do botao (estilo HUD)
    float cantoTam = 12.0f;
    glBegin(GL_LINES);
        glColor4f(1.0f, 0.95f, 0.6f, 0.9f * alphaBotao);

        glVertex2f(botaoX - 6, botaoY - 6);
        glVertex2f(botaoX - 6 + cantoTam, botaoY - 6);
        glVertex2f(botaoX - 6, botaoY - 6);
        glVertex2f(botaoX - 6, botaoY - 6 + cantoTam);

        glVertex2f(botaoX + botaoLargura + 6, botaoY - 6);
        glVertex2f(botaoX + botaoLargura + 6 - cantoTam, botaoY - 6);
        glVertex2f(botaoX + botaoLargura + 6, botaoY - 6);
        glVertex2f(botaoX + botaoLargura + 6, botaoY - 6 + cantoTam);

        glVertex2f(botaoX - 6, botaoY + botaoAltura + 6);
        glVertex2f(botaoX - 6 + cantoTam, botaoY + botaoAltura + 6);
        glVertex2f(botaoX - 6, botaoY + botaoAltura + 6);
        glVertex2f(botaoX - 6, botaoY + botaoAltura + 6 - cantoTam);

        glVertex2f(botaoX + botaoLargura + 6, botaoY + botaoAltura + 6);
        glVertex2f(botaoX + botaoLargura + 6 - cantoTam, botaoY + botaoAltura + 6);
        glVertex2f(botaoX + botaoLargura + 6, botaoY + botaoAltura + 6);
        glVertex2f(botaoX + botaoLargura + 6, botaoY + botaoAltura + 6 - cantoTam);
    glEnd();

    // pequenos triangulos "play" decorativos nas laterais do texto
    float setaY = botaoY + botaoAltura / 2.0f;
    glColor4f(1.0f, 1.0f, 1.0f, alphaBotao);
    if (mouseSobreBotao)
    {
        glBegin(GL_TRIANGLES);
            glVertex2f(botaoX + 24, setaY - 6);
            glVertex2f(botaoX + 24, setaY + 6);
            glVertex2f(botaoX + 34, setaY);
        glEnd();
    }

    // texto do botao
    glColor4f(1.0f, 1.0f, 1.0f, alphaBotao);
    desenharTextoCentralizado(
        centroX,
        botaoY + botaoAltura / 2.0f + 6,
        "INICIAR JOGO",
        GLUT_BITMAP_HELVETICA_18
    );

    // Linha
    {
        float y = altura * 0.74f;
        float meiaLargura = 90.0f * entradaSuave;
        glBegin(GL_LINES);
            glColor4f(0.5f, 0.6f, 0.9f, 0.4f * entradaSuave);
            glVertex2f(centroX - meiaLargura, y);
            glVertex2f(centroX + meiaLargura, y);
        glEnd();
    }

    //Rodape
    float piscar = 0.55f + 0.45f * sinf(tempo * 2.2f);

    glColor4f(0.65f, 0.65f, 0.7f, piscar * entradaSuave);
    desenharTextoCentralizado(centroX, altura - 50, "Clique no botao para comecar", GLUT_BITMAP_HELVETICA_18);

    glColor4f(0.4f, 0.42f, 0.5f, 0.7f * entradaSuave);
    desenharTextoCentralizado(centroX, altura - 22, "STAR DEFENSE  -  Episodio I", GLUT_BITMAP_HELVETICA_10);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}
