#include "Inimigo.h"
#include "Modelo.h"
#include "Colisao.h"
#include "Camera.h"
#include <GL/glut.h>
#include <cmath>

struct AreaPermitida {
    float minX, maxX;
    float minZ, maxZ;
};

AreaPermitida areasPermitidas[] = {

    // corredor inicial
    {-29.60f, 10.70f, -1.35f, 1.90f},

    // sala do meio
    {-11.00f, -7.00f, -4.70f, 3.85f},

    // corredor final 
    {-30.20f, -28.80f, -1.35f, 1.90f},

    // sala final
    {-33.00f, -29.50f, -5.35f, 3.00f},

    // corredor inicial duplicado
    {-73.30f, -33.00f, -1.35f, 1.90f},

    // sala do meio duplicada
    {-54.70f, -50.70f, -4.70f, 3.85f},

    // corredor final 
    {-73.90f, -72.50f, -1.35f, 1.90f},

    // sala final 
    {-76.70f, -73.20f, -5.35f, 3.00f},
};

bool podeMoverPara(float x, float z) {
    for (const auto& area : areasPermitidas) {
        if (x >= area.minX && x <= area.maxX &&
            z >= area.minZ && z <= area.maxZ) {
            return true;
        }
    }

    return false;
}

// Adicionado no final do arquivo:
bool atingiuInimigo(float x, float z, float ix, float iz, float raio) {
    float dx = x - ix;
    float dz = z - iz;
    float distancia = sqrt(dx * dx + dz * dz);
    return distancia <= raio;
}

bool atingiuInimigoSegmento(float xAnterior, float zAnterior, float x, float z,
                             float ix, float iz, float raio) {
    float segX = x - xAnterior;
    float segZ = z - zAnterior;
    float segLenSq = segX * segX + segZ * segZ;

    // Segmento degenerado (tiro nao se moveu nesse frame): cai no caso simples
    if (segLenSq < 1e-8f) {
        return atingiuInimigo(x, z, ix, iz, raio);
    }

    // Projeta o centro do inimigo sobre o segmento e limita t em [0,1]
    float t = ((ix - xAnterior) * segX + (iz - zAnterior) * segZ) / segLenSq;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    float perX = xAnterior + segX * t;
    float perZ = zAnterior + segZ * t;

    float dx = perX - ix;
    float dz = perZ - iz;
    float distancia = sqrt(dx * dx + dz * dz);

    return distancia <= raio;
}

bool inimigoNaDirecaoCamera(float camX, float camZ, float anguloY, float ix, float iz,
                             float anguloMaximoGraus, float distanciaMaxima) {
    float distanciaX = ix - camX;
    float distanciaZ = iz - camZ;
    float distancia = sqrt(distanciaX * distanciaX + distanciaZ * distanciaZ);

    if (distancia > distanciaMaxima || distancia <= 0.0001f) {
        return false;
    }

    float anguloCamRad = anguloY * 3.14159f / 180.0f;
    float dirCamX = sin(anguloCamRad);
    float dirCamZ = -cos(anguloCamRad);

    float dirInimigoX = distanciaX / distancia;
    float dirInimigoZ = distanciaZ / distancia;

    float produtoEscalar = (dirCamX * dirInimigoX) + (dirCamZ * dirInimigoZ);

    if (produtoEscalar > 1.0f) produtoEscalar = 1.0f;
    if (produtoEscalar < -1.0f) produtoEscalar = -1.0f;

    float anguloEntreDirecoes = acos(produtoEscalar) * 180.0f / 3.14159f;

    return anguloEntreDirecoes <= anguloMaximoGraus;
}