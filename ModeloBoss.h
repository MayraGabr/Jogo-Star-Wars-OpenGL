#ifndef MODELOBOSS_H
#define MODELOBOSS_H

#include <string>
#include <vector>
#include <GL/glut.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

class ModeloBoss {
private:
    Assimp::Importer importer;
    const aiScene* cena;
    std::string pastaModelo;
    std::vector<GLuint> texturasMateriais;

    GLuint carregarTextura(const std::string& caminho);
    GLuint carregarTexturaEmbutida(const aiTexture* texInterna);
    void carregarTexturasDosMateriais();

    void desenharMesh(aiMesh* mesh);

public:
    ModeloBoss();
    bool carregar(const std::string& caminho);
    void desenhar();
};

#endif