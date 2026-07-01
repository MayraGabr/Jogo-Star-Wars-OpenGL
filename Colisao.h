#include "Modelo.h"
#ifndef COLISAO_H
#define COLISAO_H

bool podeMoverPara(float x, float z);

bool atingiuInimigo(float x, float z, float ix, float iz, float raio = 0.45f);

bool atingiuInimigoSegmento(float xAnterior, float zAnterior, float x, float z,
                             float ix, float iz, float raio = 0.45f);

bool inimigoNaDirecaoCamera(float camX, float camZ, float anguloY, float ix, float iz,
                             float anguloMaximoGraus = 3.0f, float distanciaMaxima = 100.0f);
#endif