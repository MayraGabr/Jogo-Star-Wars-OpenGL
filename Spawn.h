#ifndef SPAWN_H
#define SPAWN_H

enum LocalSpawn {
    CorredorInicial,
    SalaMeio,
    CorredorFinal1,
    CorredorFinal2,
    SalaFinal,

    CorredorInicial1Duplicado,
    CorredorInicial2Duplicado,
    SalaMeioDuplicada,
    CorredorFinalDuplicado,
    SalaFinalDuplicada
};

void gerarPosicaoInimigo(float& x, float& z, LocalSpawn local);
void criarInimigoAleatorio(LocalSpawn local);
void spawnarGrupo(LocalSpawn local, int quantidade);
void verificarSpawnsPorMapa();
void resetarSpawns();

#endif