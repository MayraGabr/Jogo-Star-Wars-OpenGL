#ifndef MODELO_H
#define MODELO_H

#include <string>
#include <vector>
#include <map>
#include <GL/glut.h>

#include "tiny_obj_loader.h"

struct FaceModelo {
    float v[3][3];
    float n[3][3];
    float t[3][2];
    int materialId;
};

class Modelo {
private:
    std::vector<FaceModelo> faces;
    std::vector<tinyobj::material_t> materiais;
    std::map<std::string, GLuint> texturas;

    GLuint listaDisplay;
    bool listaCriada;
    std::string pastaTexturasSalva;

    void criarDisplayList();
    GLuint carregarTextura(const std::string& caminho);

public:
    Modelo();

    bool carregar(
        const std::string& arquivoObj,
        const std::string& pastaMtl,
        const std::string& pastaTexturas
    );

    void desenhar();
};

#endif