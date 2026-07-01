#include "Modelo.h"

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Modelo::Modelo() {
    listaDisplay = 0;
    listaCriada = false;
}

GLuint Modelo::carregarTextura(const std::string& caminho) {
    if (texturas.count(caminho)) {
        return texturas[caminho];
    }

    int largura, altura, canais;
    unsigned char* dados = stbi_load(caminho.c_str(), &largura, &altura, &canais, 0);

    if (!dados) {

        return 0;
    }

    GLenum formato = (canais == 4) ? GL_RGBA : GL_RGB;

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(dados);

    texturas[caminho] = id;
    return id;
}

void Modelo::criarDisplayList() {
    if (listaCriada) {
        return;
    }

    listaDisplay = glGenLists(1);
    glNewList(listaDisplay, GL_COMPILE);

    glEnable(GL_TEXTURE_2D);

    int materialAtual = -999;
    GLuint texturaAtual = 0;

    for (const auto& face : faces) {
        if (face.materialId != materialAtual) {
            if (materialAtual != -999) {
                glEnd();
            }

            materialAtual = face.materialId;
            texturaAtual = 0;

            if (materialAtual >= 0 && materialAtual < (int)materiais.size()) {
                std::string nome = materiais[materialAtual].diffuse_texname;

                if (!nome.empty()) {
                    size_t barra = nome.find_last_of("/\\");

                    if (barra != std::string::npos) {
                        nome = nome.substr(barra + 1);
                    }

                    std::string caminho = pastaTexturasSalva + nome;
                    texturaAtual = carregarTextura(caminho);
                }
            }

            if (texturaAtual != 0) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texturaAtual);
                glColor3f(1.0f, 1.0f, 1.0f);
            } else {
                glDisable(GL_TEXTURE_2D);

                if (materialAtual >= 0 && materialAtual < (int)materiais.size()) {
                    GLfloat difusa[] = {
                        materiais[materialAtual].diffuse[0],
                        materiais[materialAtual].diffuse[1],
                        materiais[materialAtual].diffuse[2],
                        1.0f
                    };

                    GLfloat ambiente[] = {
                        materiais[materialAtual].ambient[0],
                        materiais[materialAtual].ambient[1],
                        materiais[materialAtual].ambient[2],
                        1.0f
                    };

                    GLfloat especular[] = {
                        materiais[materialAtual].specular[0],
                        materiais[materialAtual].specular[1],
                        materiais[materialAtual].specular[2],
                        1.0f
                    };

                    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, difusa);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambiente);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
                    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materiais[materialAtual].shininess);
                }
                else {
                    GLfloat difusa[] = {0.7f, 0.7f, 0.7f, 1.0f};
                    GLfloat ambiente[] = {0.3f, 0.3f, 0.3f, 1.0f};

                    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, difusa);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambiente);
                }

            }
            glBegin(GL_TRIANGLES);
            } 

        for (int i = 0; i < 3; i++) {
            glTexCoord2f(face.t[i][0], face.t[i][1]);
            glNormal3f(face.n[i][0], face.n[i][1], face.n[i][2]);
            glVertex3f(face.v[i][0], face.v[i][1], face.v[i][2]);
        }
    }

    if (materialAtual != -999) {
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    glEndList();

    listaCriada = true;
}

bool Modelo::carregar(
    const std::string& arquivoObj,
    const std::string& pastaMtl,
    const std::string& pastaTexturas
) {
    tinyobj::ObjReaderConfig config;
    config.mtl_search_path = pastaMtl;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(arquivoObj, config)) {
        return false;
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    materiais = reader.GetMaterials();

    faces.clear();

    for (const auto& shape : shapes) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int qtdVertices = shape.mesh.num_face_vertices[f];

            if (qtdVertices != 3) {
                index_offset += qtdVertices;
                continue;
            }

            FaceModelo face;
            face.materialId = shape.mesh.material_ids[f];

            for (int v = 0; v < 3; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                face.v[v][0] = attrib.vertices[3 * idx.vertex_index + 0];
                face.v[v][1] = attrib.vertices[3 * idx.vertex_index + 1];
                face.v[v][2] = attrib.vertices[3 * idx.vertex_index + 2];

                if (idx.normal_index >= 0) {
                    face.n[v][0] = attrib.normals[3 * idx.normal_index + 0];
                    face.n[v][1] = attrib.normals[3 * idx.normal_index + 1];
                    face.n[v][2] = attrib.normals[3 * idx.normal_index + 2];
                } else {
                    face.n[v][0] = 0.0f;
                    face.n[v][1] = 1.0f;
                    face.n[v][2] = 0.0f;
                }

                if (idx.texcoord_index >= 0) {
                    face.t[v][0] = attrib.texcoords[2 * idx.texcoord_index + 0];
                    face.t[v][1] = 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1];
                } else {
                    face.t[v][0] = 0.0f;
                    face.t[v][1] = 0.0f;
                }
            }

            faces.push_back(face);
            index_offset += qtdVertices;
        }
    }
    pastaTexturasSalva = pastaTexturas;
    criarDisplayList();

    return true;
}

void Modelo::desenhar() {
    if (listaCriada) {
        glCallList(listaDisplay);
    }
}