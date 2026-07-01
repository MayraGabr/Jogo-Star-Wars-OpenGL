#include "ModeloAnimado.h"
#include <cstring>
#include <iostream>
#include <assimp/postprocess.h>
#include "stb_image.h"

ModeloAnimado::ModeloAnimado() {
    cena = nullptr;
}

void ModeloAnimado::carregarTexturasDosMateriais() {
    texturasMateriais.clear();

    for (unsigned int i = 0; i < cena->mNumMaterials; i++) {
        aiMaterial* material = cena->mMaterials[i];

        aiString caminhoTextura;
        GLuint textura = 0;

        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &caminhoTextura) == AI_SUCCESS ||
            material->GetTexture(aiTextureType_BASE_COLOR, 0, &caminhoTextura) == AI_SUCCESS)
        {
            std::string nome = caminhoTextura.C_Str();

            // Textura embutida no GLB: *0, *1, *2...
            if (!nome.empty() && nome[0] == '*') {
                const aiTexture* texInterna = cena->GetEmbeddedTexture(nome.c_str());

                if (texInterna) {
                    textura = carregarTexturaEmbutida(texInterna);
                }
            }
            else {
                size_t barra = nome.find_last_of("/\\");
                if (barra != std::string::npos) {
                    nome = nome.substr(barra + 1);
                }

                std::string caminhoFinal = pastaModelo + "texturas/" + nome;
                textura = carregarTextura(caminhoFinal);
            }
        }

        texturasMateriais.push_back(textura);
    }
}

GLuint ModeloAnimado::carregarTexturaEmbutida(const aiTexture* texInterna) {
    int largura, altura, canais;
    unsigned char* dados = nullptr;

    if (texInterna->mHeight == 0) {
        dados = stbi_load_from_memory(
            reinterpret_cast<unsigned char*>(texInterna->pcData),
            texInterna->mWidth,
            &largura,
            &altura,
            &canais,
            0
        );
    } else {
        dados = stbi_load_from_memory(
            reinterpret_cast<unsigned char*>(texInterna->pcData),
            texInterna->mWidth * texInterna->mHeight * 4,
            &largura,
            &altura,
            &canais,
            0
        );
    }

    if (!dados) {
        std::cout << "Erro ao carregar textura embutida." << std::endl;
        return 0;
    }

    GLenum formato = (canais == 4) ? GL_RGBA : GL_RGB;

    GLuint textura;
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        formato,
        largura,
        altura,
        0,
        formato,
        GL_UNSIGNED_BYTE,
        dados
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(dados);

    return textura;
}

GLuint ModeloAnimado::carregarTextura(const std::string& caminho) {
    int largura, altura, canais;

    unsigned char* dados = stbi_load(
        caminho.c_str(),
        &largura,
        &altura,
        &canais,
        0
    );

    if (!dados) {
        std::cout << "Erro ao carregar textura: " << caminho << std::endl;
        return 0;
    }

    GLenum formato = (canais == 4) ? GL_RGBA : GL_RGB;

    GLuint textura;
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        formato,
        largura,
        altura,
        0,
        formato,
        GL_UNSIGNED_BYTE,
        dados
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(dados);

    return textura;
}

bool ModeloAnimado::carregar(const std::string& caminho) {
    size_t pos = caminho.find_last_of("/\\");
    pastaModelo = caminho.substr(0, pos + 1);

    cena = importer.ReadFile(
        caminho,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices
    );

    if (!cena) {
        std::cout << "Erro Assimp: " << importer.GetErrorString() << std::endl;
        return false;
    }

    carregarTexturasDosMateriais();
    carregarOssos();
    carregarPesosDosVertices();


    if (cena->HasAnimations()) {
        std::cout << "Animacoes: " << cena->mNumAnimations << std::endl;
    } else {
        std::cout << "Sem animacoes." << std::endl;
    }

    transformacaoGlobalInversa = cena->mRootNode->mTransformation;
    transformacaoGlobalInversa.Inverse();

    return true;
}

void ModeloAnimado::atualizarAnimacao(float deltaTempo) {
    if (!cena || !cena->HasAnimations()) {
        return;
    }

    aiAnimation* animacao = cena->mAnimations[0];

    float ticksPorSegundo = animacao->mTicksPerSecond != 0
        ? animacao->mTicksPerSecond
        : 25.0f;

    tempoAnimacao += deltaTempo * ticksPorSegundo;

    float duracao = animacao->mDuration;

    while (tempoAnimacao > duracao) {
        tempoAnimacao -= duracao;
    }

    aiMatrix4x4 identidade;

    lerHierarquia(tempoAnimacao, cena->mRootNode, identidade);
}

void ModeloAnimado::desenhar() {
    if (!cena) return;

    for (unsigned int i = 0; i < cena->mNumMeshes; i++) {
        desenharMesh(cena->mMeshes[i]);
    }
}

void ModeloAnimado::desenharNo(aiNode* no) {

    glPushMatrix();

    for (unsigned int i = 0; i < no->mNumMeshes; i++) {
        aiMesh* mesh = cena->mMeshes[no->mMeshes[i]];
        desenharMesh(mesh);
    }

    for (unsigned int i = 0; i < no->mNumChildren; i++) {
        desenharNo(no->mChildren[i]);
    }

    glPopMatrix();
}

void ModeloAnimado::desenharMesh(aiMesh* mesh) {

    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    GLuint textura = 0;

    if (mesh->mMaterialIndex < texturasMateriais.size()) {
        textura = texturasMateriais[mesh->mMaterialIndex];
    }

    if (textura != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textura);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.7f, 0.7f, 0.7f);
    }

    unsigned int baseVertice = 0;

    for (unsigned int m = 0; m < cena->mNumMeshes; m++) {
        if (cena->mMeshes[m] == mesh) {
            break;
        }

        baseVertice += cena->mMeshes[m]->mNumVertices;
    }

    glBegin(GL_TRIANGLES);

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {

        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++) {

            unsigned int indice = face.mIndices[j];

            if (mesh->HasTextureCoords(0)) {
                aiVector3D tex = mesh->mTextureCoords[0][indice];
                glTexCoord2f(tex.x, tex.y);
            }

            if (mesh->HasNormals()) {
                aiVector3D normal = mesh->mNormals[indice];
                glNormal3f(normal.x, normal.y, normal.z);
            }

            aiVector3D vertice = mesh->mVertices[indice];

            unsigned int indiceGlobal = baseVertice + indice;

            aiVector3D verticeAnimado =
                aplicarSkinning(indiceGlobal, vertice);

            glVertex3f(
                verticeAnimado.x,
                verticeAnimado.y,
                verticeAnimado.z
            );
        }
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void ModeloAnimado::carregarOssos() {

    mapaOssos.clear();
    ossos.clear();

    for (unsigned int i = 0; i < cena->mNumMeshes; i++) {

        aiMesh* mesh = cena->mMeshes[i];

        for (unsigned int j = 0; j < mesh->mNumBones; j++) {

            aiBone* bone = mesh->mBones[j];

            std::string nome = bone->mName.C_Str();

            if (mapaOssos.find(nome) == mapaOssos.end()) {

                BoneInfo info;
                info.offset = bone->mOffsetMatrix;
                info.finalTransform = aiMatrix4x4();

                mapaOssos[nome] = ossos.size();
                ossos.push_back(info);

            }
        }
    }

}

void ModeloAnimado::carregarPesosDosVertices(){

    dadosBones.clear();

    unsigned int totalVertices = 0;

    for(unsigned int i = 0; i < cena->mNumMeshes; i++){

        totalVertices += cena->mMeshes[i]->mNumVertices;

    }

    dadosBones.resize(totalVertices);

    unsigned int baseVertice = 0;

    for(unsigned int i = 0; i < cena->mNumMeshes; i++){

        aiMesh* mesh = cena->mMeshes[i];

        for(unsigned int b = 0; b < mesh->mNumBones; b++){

            aiBone* bone = mesh->mBones[b];

            int indiceBone = mapaOssos[
                bone->mName.C_Str()
            ];

            for(unsigned int w = 0; w < bone->mNumWeights; w++){

                aiVertexWeight peso = bone->mWeights[w];

                unsigned int indiceVertice =
                    baseVertice +
                    peso.mVertexId;

                dadosBones[indiceVertice].adicionar(
                    indiceBone,
                    peso.mWeight
                );

            }

        }

        baseVertice += mesh->mNumVertices;

    }

    std::cout
        << "Vertices com pesos: "
        << dadosBones.size()
        << std::endl;

}

const aiNodeAnim* ModeloAnimado::encontrarCanal(aiAnimation* animacao, const std::string& nomeNo) {
    for (unsigned int i = 0; i < animacao->mNumChannels; i++) {
        const aiNodeAnim* canal = animacao->mChannels[i];

        if (nomeNo == canal->mNodeName.C_Str()) {
            return canal;
        }
    }

    return nullptr;
}

unsigned int ModeloAnimado::encontrarIndicePosicao(float tempo, const aiNodeAnim* canal) {
    for (unsigned int i = 0; i < canal->mNumPositionKeys - 1; i++) {
        if (tempo < canal->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

unsigned int ModeloAnimado::encontrarIndiceRotacao(float tempo, const aiNodeAnim* canal) {
    for (unsigned int i = 0; i < canal->mNumRotationKeys - 1; i++) {
        if (tempo < canal->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

unsigned int ModeloAnimado::encontrarIndiceEscala(float tempo, const aiNodeAnim* canal) {
    for (unsigned int i = 0; i < canal->mNumScalingKeys - 1; i++) {
        if (tempo < canal->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

aiVector3D ModeloAnimado::interpolarPosicao(float tempo, const aiNodeAnim* canal) {
    if (canal->mNumPositionKeys == 1) {
        return canal->mPositionKeys[0].mValue;
    }

    unsigned int indice = encontrarIndicePosicao(tempo, canal);
    unsigned int proximoIndice = indice + 1;

    float tempoInicial = canal->mPositionKeys[indice].mTime;
    float tempoFinal = canal->mPositionKeys[proximoIndice].mTime;
    float fator = (tempo - tempoInicial) / (tempoFinal - tempoInicial);

    aiVector3D inicio = canal->mPositionKeys[indice].mValue;
    aiVector3D fim = canal->mPositionKeys[proximoIndice].mValue;

    return inicio + fator * (fim - inicio);
}

aiQuaternion ModeloAnimado::interpolarRotacao(float tempo, const aiNodeAnim* canal) {
    if (canal->mNumRotationKeys == 1) {
        return canal->mRotationKeys[0].mValue;
    }

    unsigned int indice = encontrarIndiceRotacao(tempo, canal);
    unsigned int proximoIndice = indice + 1;

    float tempoInicial = canal->mRotationKeys[indice].mTime;
    float tempoFinal = canal->mRotationKeys[proximoIndice].mTime;
    float fator = (tempo - tempoInicial) / (tempoFinal - tempoInicial);

    aiQuaternion inicio = canal->mRotationKeys[indice].mValue;
    aiQuaternion fim = canal->mRotationKeys[proximoIndice].mValue;

    aiQuaternion resultado;
    aiQuaternion::Interpolate(resultado, inicio, fim, fator);
    resultado.Normalize();

    return resultado;
}

aiVector3D ModeloAnimado::interpolarEscala(float tempo, const aiNodeAnim* canal) {
    if (canal->mNumScalingKeys == 1) {
        return canal->mScalingKeys[0].mValue;
    }

    unsigned int indice = encontrarIndiceEscala(tempo, canal);
    unsigned int proximoIndice = indice + 1;

    float tempoInicial = canal->mScalingKeys[indice].mTime;
    float tempoFinal = canal->mScalingKeys[proximoIndice].mTime;
    float fator = (tempo - tempoInicial) / (tempoFinal - tempoInicial);

    aiVector3D inicio = canal->mScalingKeys[indice].mValue;
    aiVector3D fim = canal->mScalingKeys[proximoIndice].mValue;

    return inicio + fator * (fim - inicio);
}

aiVector3D ModeloAnimado::aplicarSkinning(unsigned int indiceGlobal, const aiVector3D& vertice) {

    if (indiceGlobal >= dadosBones.size()) {
        return vertice;
    }

    VertexBoneData dados = dadosBones[indiceGlobal];

    aiVector3D resultado(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < MAX_BONES_POR_VERTICE; i++) {

        int idOsso = dados.ids[i];
        float peso = dados.pesos[i];

        if (idOsso < 0 || peso <= 0.0f) {
            continue;
        }

        if (idOsso >= (int)ossos.size()) {
            continue;
        }

        aiMatrix4x4 matriz = ossos[idOsso].finalTransform;

        aiVector3D transformado = matriz * vertice;

        resultado += transformado * peso;
    }

    return resultado;
}

void ModeloAnimado::lerHierarquia(float tempo, aiNode* no, const aiMatrix4x4& transformacaoPai) {
    std::string nomeNo = no->mName.C_Str();

    aiAnimation* animacao = cena->mAnimations[0];

    aiMatrix4x4 transformacaoNo = no->mTransformation;

    const aiNodeAnim* canal = encontrarCanal(animacao, nomeNo);

    if (canal) {
        aiVector3D escala = interpolarEscala(tempo, canal);
        aiMatrix4x4 matrizEscala;
        aiMatrix4x4::Scaling(escala, matrizEscala);

        aiQuaternion rotacao = interpolarRotacao(tempo, canal);
        aiMatrix4x4 matrizRotacao = aiMatrix4x4(rotacao.GetMatrix());

        aiVector3D posicao = interpolarPosicao(tempo, canal);

        if (nomeNo.find("Hips") != std::string::npos) {
            aiVector3D posInicial = canal->mPositionKeys[0].mValue;

            posicao.x = posInicial.x;
            posicao.y = posInicial.y;
            posicao.z = posInicial.z;
        }

        aiMatrix4x4 matrizPosicao;
        aiMatrix4x4::Translation(posicao, matrizPosicao);

        transformacaoNo = matrizPosicao * matrizRotacao * matrizEscala;
    }

    aiMatrix4x4 transformacaoGlobal = transformacaoPai * transformacaoNo;

    if (mapaOssos.find(nomeNo) != mapaOssos.end()) {
        int indiceOsso = mapaOssos[nomeNo];

        ossos[indiceOsso].finalTransform =
            transformacaoGlobalInversa *
            transformacaoGlobal *
            ossos[indiceOsso].offset;
    }

    for (unsigned int i = 0; i < no->mNumChildren; i++) {
        lerHierarquia(tempo, no->mChildren[i], transformacaoGlobal);
    }
}