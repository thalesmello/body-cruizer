#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/*
Nota do Autor:
    Este jogo foi feito tentando simular um ambiente tridimencional.
    Por falta de tempo, alguns aspectos não puderam ser implantados: 
        -Animação de movimentação na navinha
        -Modo de jogo infinito (fases geradas aleatoriamente pelo programa)
    
    Existem algumas funções para ordenação da lista ligada das células,
    mas devido à existência de um bug que não tive tempo de depurar,
    preferi terminar o jogo sem utilizá-las. As funções ordenavam as
    células para a figura de uma mais longe não sobrepusesse uma mais
    próxima. Para contornar o problema, as fases foram criadas de forma
    a já fornecer as células ordenadamente.
    
    A função randomizer() foi criada para a geração aleatória das fases
    com base em um nível de dificuldade. Apesar de não ser usada explicitamente
    durante a execução deste jogo, ela foi usada em um código paralelo para
    que pudesse prodizir as fases.
    
Atenciosamente,
    Thales Mello, 2009
*/

volatile int COUNTER;
void ADD_COUNTER(){
    COUNTER++;
}

//*************************************
//Declarações de tipos de variáveis
//*************************************
enum horizontalPosition { LEFT=0, MIDDLE=1, RIGHT=2 };
enum tipo_de_celula{COELHO,MUTANTE,VIRUS,RIP,QUEIJO,SANGUE};
enum game_option { EASY_GAME=0 , MEDIUM_GAME=1, HARD_GAME=2, EXIT_GAME=3, NULL_OPTION };
char game_option_tab[4][100] = {"Facil", "Medio", "Dificil", "Sair"};
enum game_condition { NEXT_STAGE, GAME_OVER, LEAVE_GAME };

struct relativePoint{
    horizontalPosition x;
    int h; //Armazena a altura da localização na fase, em que altura que tal objeto está
};
struct spaceship{
    BITMAP *sprite;
    relativePoint p;
    int speed; //Armazena a velocidade da nave, com base na dificuldade que o jogador escolheu
};
struct cell{
    tipo_de_celula type;
    BITMAP *sprite;
    relativePoint p;
    cell *nextCell;
    cell *previousCell;
};
struct objects{
    spaceship *ship;
    cell *firstCell;
};



//*************************************
//Declarações de função
//*************************************
void setCellSprite(cell *c);
void putCell(cell *c,int alturaNave,BITMAP *buffer);
void putAllCells(cell *firstCell,int alturaNave,BITMAP *buffer);
void setShipSprite(spaceship *ship);
void putShip(spaceship *ship,BITMAP *buffer);
cell setFinnishLine(cell *firstCell);
void setPosition(spaceship *ship);
int verifyCollision(spaceship *ship,cell *c);
int verifyEnd(int alturaNave,int alturaLine);
game_condition gameLoop(objects *obj, int background);
int backgroundColor(int level);
void getCell(cell *current,FILE *arq);
void getAllCells(objects *obj, FILE *arq);
void saveAllCells(cell *firstCell,FILE *arq);
void freeAllCells(cell *firstCell);
int countCells(cell *firstCell);
void sortOneCell(cell *firstCell,int numCells);
void sortAllCells(cell *firstCell);
horizontalPosition int2pos(int i);
void randomizer(int length,int level);
game_option int2option(int x);
void posToString(char string[], game_option cursorPosition, game_option ind);
int getMenuMovement(int previous);
game_option callMenu();
void init();
void deinit();
void updateScreen(BITMAP *buffer);
