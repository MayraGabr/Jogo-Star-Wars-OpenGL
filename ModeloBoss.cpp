#include "ModeloBoss.h"

#include <iostream>
#include <vector>
#include <assimp/postprocess.h>
#include "stb_image.h"
#include <GL/glut.h>

ModeloBoss::ModeloBoss() {
    cena = nullptr;
}

bool ModeloBoss::carregar(const std::string& caminho) {
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
        return false;
    }

    carregarTexturasDosMateriais();

    return true;
}

void ModeloBoss::carregarTexturasDosMateriais() {
    texturasMateriais.clear();

    for (unsigned int i = 0; i < cena->mNumMaterials; i++) {
        aiMaterial* material = cena->mMaterials[i];

        aiString caminhoTextura;
        GLuint textura = 0;

        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &caminhoTextura) == AI_SUCCESS ||
            material->GetTexture(aiTextureType_BASE_COLOR, 0, &caminhoTextura) == AI_SUCCESS)
        {
            std::string nome = caminhoTextura.C_Str();

            if (!nome.empty() && nome[0] == '*') {
                const aiTexture* texInterna = cena->GetEmbeddedTexture(nome.c_str());

                if (texInterna) {
                    textura = carregarTexturaEmbutida(texInterna);
                }
            } else {
                // Remove o caminho e deixa só o nome do arquivo
                size_t barra = nome.find_last_of("/\\");
                if (barra != std::string::npos) {
                    nome = nome.substr(barra + 1);
                }

                // Procura em data/texturas/
                std::string caminhoFinal = "data/texturas/" + nome;
                textura = carregarTextura(caminhoFinal);

                // Se não encontrar, tenta na pasta do modelo
                if (textura == 0) {
                    caminhoFinal = pastaModelo + nome;
                    textura = carregarTextura(caminhoFinal);
                }

            }
        }

        texturasMateriais.push_back(textura);
    }
}

GLuint ModeloBoss::carregarTexturaEmbutida(const aiTexture* texInterna) {
    GLuint textura;
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);

    if (texInterna->mHeight == 0) {
        // Textura comprimida (PNG/JPG embutido no arquivo do modelo).
        // Nesse caso, mWidth guarda o TAMANHO EM BYTES do buffer comprimido.
        int largura, altura, canais;

        unsigned char* dados = stbi_load_from_memory(
            reinterpret_cast<unsigned char*>(texInterna->pcData),
            texInterna->mWidth,
            &largura,
            &altura,
            &canais,
            0
        );

        if (!dados) {
            glDeleteTextures(1, &textura);
            return 0;
        }

        GLenum formato = (canais == 4) ? GL_RGBA : GL_RGB;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            largura,
            altura,
            0,
            formato,
            GL_UNSIGNED_BYTE,
            dados
        );

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        stbi_image_free(dados);
    } else {
        // Textura NÃO comprimida: pcData já é um array de pixels aiTexel (BGRA),
        // sem cabeçalho de arquivo de imagem. Não passa pelo stb_image.
        unsigned int largura = texInterna->mWidth;
        unsigned int altura  = texInterna->mHeight;

        std::vector<unsigned char> rgba(static_cast<size_t>(largura) * altura * 4);

        for (unsigned int i = 0; i < largura * altura; i++) {
            const aiTexel& t = texInterna->pcData[i];
            rgba[i * 4 + 0] = t.r;
            rgba[i * 4 + 1] = t.g;
            rgba[i * 4 + 2] = t.b;
            rgba[i * 4 + 3] = t.a;
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            largura,
            altura,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            rgba.data()
        );

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textura;
}

GLuint ModeloBoss::carregarTextura(const std::string& caminho) {
    int largura, altura, canais;

    unsigned char* dados = stbi_load(
        caminho.c_str(),
        &largura,
        &altura,
        &canais,
        0
    );

    if (!dados) {
        return 0;
    }

    GLenum formato;

    if (canais == 1) {
        formato = GL_LUMINANCE;
    } else if (canais == 4) {
        formato = GL_RGBA;
    } else {
        formato = GL_RGB;
    }

    GLuint textura;
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(dados);

    return textura;
}

void ModeloBoss::desenhar() {
    if (!cena) return;

    glDisable(GL_CULL_FACE);

    for (unsigned int i = 0; i < cena->mNumMeshes; i++) {
        desenharMesh(cena->mMeshes[i]);
    }

    glDisable(GL_TEXTURE_2D);
}

void ModeloBoss::desenharMesh(aiMesh* mesh) {
    GLuint textura = 0;

    if (mesh->mMaterialIndex < texturasMateriais.size()) {
        textura = texturasMateriais[mesh->mMaterialIndex];
    }

    bool ombreira = (mesh->mMaterialIndex == 1);
    bool sabre = (mesh->mMaterialIndex == 29);

    if (sabre) {
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    else if (ombreira) {
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glColor3f(0.02f, 0.02f, 0.02f);
    }
    else if (textura != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textura);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    else {
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glColor3f(0.02f, 0.02f, 0.02f);
    }

    glBegin(GL_TRIANGLES);

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            unsigned int indice = face.mIndices[j];

            if (mesh->HasTextureCoords(0) && textura != 0 && !sabre && !ombreira) {
                aiVector3D tex = mesh->mTextureCoords[0][indice];
                glTexCoord2f(tex.x, tex.y);
            }

            if (mesh->HasNormals()) {
                aiVector3D normal = mesh->mNormals[indice];
                glNormal3f(normal.x, normal.y, normal.z);
            }

            aiVector3D v = mesh->mVertices[indice];
            glVertex3f(v.x, v.y, v.z);
        }
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}