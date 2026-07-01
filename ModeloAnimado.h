#ifndef MODELO_ANIMADO_H
#define MODELO_ANIMADO_H

#include <string>
#include <vector>
#include <map>
#include <string>
#include <GL/glut.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

const int MAX_BONES_POR_VERTICE = 4;

struct VertexBoneData {

    int ids[MAX_BONES_POR_VERTICE];
    float pesos[MAX_BONES_POR_VERTICE];

    VertexBoneData() {

        for(int i = 0; i < MAX_BONES_POR_VERTICE; i++) {

            ids[i] = -1;
            pesos[i] = 0.0f;

        }
    }

    void adicionar(int id, float peso){

        for(int i = 0; i < MAX_BONES_POR_VERTICE; i++){

            if(ids[i] == -1){

                ids[i] = id;
                pesos[i] = peso;
                return;

            }
        }

    }

};

class ModeloAnimado {
private:
    const aiScene* cena;
    Assimp::Importer importer;

    std::string pastaModelo;
    std::vector<GLuint> texturasMateriais;

    struct BoneInfo {
        aiMatrix4x4 offset;
        aiMatrix4x4 finalTransform;
    };

    std::map<std::string, int> mapaOssos;
    std::vector<BoneInfo> ossos;
    std::vector<VertexBoneData> dadosBones;

    aiMatrix4x4 transformacaoGlobalInversa;

    const aiNodeAnim* encontrarCanal(aiAnimation* animacao, const std::string& nomeNo);

    unsigned int encontrarIndicePosicao(float tempo, const aiNodeAnim* canal);
    unsigned int encontrarIndiceRotacao(float tempo, const aiNodeAnim* canal);
    unsigned int encontrarIndiceEscala(float tempo, const aiNodeAnim* canal);

    aiVector3D interpolarPosicao(float tempo, const aiNodeAnim* canal);
    aiQuaternion interpolarRotacao(float tempo, const aiNodeAnim* canal);
    aiVector3D interpolarEscala(float tempo, const aiNodeAnim* canal);
    aiVector3D aplicarSkinning(unsigned int indiceGlobal, const aiVector3D& vertice);

    void lerHierarquia(float tempo, aiNode* no, const aiMatrix4x4& transformacaoPai);

    float tempoAnimacao = 0.0f;

public:
    ModeloAnimado();

    bool carregar(const std::string& caminho);
    void desenhar();

    void atualizarAnimacao(float deltaTempo);
    void carregarOssos();
    void carregarPesosDosVertices();

    void carregarTexturasDosMateriais();
    GLuint carregarTextura(const std::string& caminho);
    GLuint carregarTexturaEmbutida(const aiTexture* texInterna);

    void desenharNo(aiNode* no);
    void desenharMesh(aiMesh* mesh);
};

#endif