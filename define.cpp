#include "define.h"
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//*************************************
//Funções do jogo
//*************************************

//Funções para trabalhar com os objetos do jogo
void setCellSprite(cell *c){
    switch(c->type){
        case SANGUE: c->sprite = load_bitmap("graphics/cells/cell_sangue.bmp",NULL); break;
        case RIP: c->sprite = load_bitmap("graphics/cells/cell_rip.bmp",NULL); break;
        case MUTANTE: c->sprite = load_bitmap("graphics/cells/cell_mutante.bmp",NULL); break;
        case QUEIJO: c->sprite = load_bitmap("graphics/cells/cell_queijo.bmp",NULL); break;
        case VIRUS: c->sprite = load_bitmap("graphics/cells/cell_virus.bmp",NULL); break;
        case COELHO: c->sprite = load_bitmap("graphics/cells/cell_coelho.bmp",NULL); break;
    }
}
void putCell(cell *c,int alturaNave,BITMAP *buffer){
    int delta, answer;
    delta = c->p.h-alturaNave;
    if(delta<=45&&delta>=0){
        float ampliacao;
        ampliacao = float(5)/float(delta+5);
        int largura,altura;
        largura = int(ampliacao*c->sprite->w);
        altura = int(ampliacao*c->sprite->h);
        int deslocaLado;
        deslocaLado = int(160*float(45-delta+5)/50);  //Nota: 160 é 1/4 da largura da tela.
        int alturaTela;
        alturaTela = 120+int(float(45-delta)/45*240);
        int abscissaTela;
        if(c->p.x == LEFT) abscissaTela = 320 - deslocaLado;
        else if(c->p.x == MIDDLE) abscissaTela = 320;
        else if(c->p.x == RIGHT) abscissaTela = 320 + deslocaLado;
        stretch_sprite(buffer,c->sprite,abscissaTela-largura/2,alturaTela-altura/2,largura,altura);
    }
}
void putAllCells(cell *firstCell,int alturaNave,BITMAP *buffer){
    cell *c;
    c = firstCell;
    while(c!=0){
        putCell(c,alturaNave,buffer);
        c = c->nextCell;
    }
}
void setShipSprite(spaceship *ship){
    ship->sprite = load_bitmap("graphics/ship.bmp",NULL);
}
void putShip(spaceship *ship,BITMAP *buffer){
    int abscissaTela;
    switch(ship->p.x){
        case LEFT:{
            abscissaTela=160;
        }break;
        case MIDDLE:{
            abscissaTela=2*160;
        }break;
        case RIGHT:{
            abscissaTela=3*160;
        }break;
    }
    draw_sprite(buffer,ship->sprite,abscissaTela-(ship->sprite->w)/2,120*3-(ship->sprite->h)/2);
}
cell setFinnishLine(cell *firstCell){
    cell line;
    line.p.x = MIDDLE;
    line.p.h = firstCell->p.h + 10;
    line.sprite = load_bitmap("graphics/xadrez.bmp",NULL);
    
    return line;
}



//Funções da dinâmica do jogo
void setPosition(spaceship *ship){
    if(key[KEY_LEFT]) ship->p.x=LEFT;
    else if(key[KEY_RIGHT]) ship->p.x=RIGHT;    
    else ship->p.x=MIDDLE;    
}
int verifyCollision(spaceship *ship,cell *c){
    int answer;
    if(ship->p.x==c->p.x){
        int delta=(c->p.h)-(ship->p.h);
        if(0>=delta&&delta>=0) answer=TRUE;
        else answer=FALSE;
    }
    else answer=FALSE;
    return answer;
}
int verifyAllCollisions(spaceship *ship, cell *firstCell){
    int ans;
    cell *c;
    c = firstCell;
    ans = FALSE;
    while(c!=0 && ans==FALSE){
        int col;
        col = verifyCollision(ship,c);
        if(col == TRUE) ans = TRUE;
        c=c->nextCell;
    }
    return ans;
}
int verifyEnd(int alturaNave,int alturaLine){
    int delta;
    delta = alturaLine-alturaNave;
    if(delta<-1) return TRUE;
    else return FALSE;
}
game_condition gameLoop(objects *obj, int background){
    BITMAP *buffer;
    buffer = create_bitmap(640,480);
    cell line;
    line = setFinnishLine(obj->firstCell);
    game_condition answer;
    obj->ship->p.h = 0;
    
    int loop=TRUE;
    int speed = obj->ship->speed;
    int dynamic = speed*COUNTER/100;
    int tempo = speed*COUNTER/100;
    while(loop){
        tempo = speed*COUNTER/100;
        while(dynamic<tempo){
            obj->ship->p.h++;
            setPosition(obj->ship);
            rectfill(buffer,0,0,640,480,background);
            putAllCells(obj->firstCell,obj->ship->p.h,buffer);
            putCell(&line,obj->ship->p.h,buffer);
            putShip(obj->ship,buffer);
            updateScreen(buffer);
            
            int col;
            col = verifyAllCollisions(obj->ship,obj->firstCell);
            if(col==TRUE){
                answer = GAME_OVER;
                loop = FALSE;
            }else{
                int end;
                end = verifyEnd(obj->ship->p.h,line.p.h);
                if(end==TRUE){
                    answer = NEXT_STAGE;
                    loop = FALSE;
                }
            }
            dynamic++;
        }
    }
    return answer;
}
int backgroundColor(int level){
    int color;
    switch(level){
        case 1: color = makecol(85,0,0); break;
        case 2: color = makecol(0,0,100); break;
        case 3: color = makecol(0,183,91); break;
        case 4: color = makecol(79,79,79); break;
        case 5: color = makecol(0,64,0); break; //Cinza
        case 6: color = makecol(0,0,0); break; //Preto
    }
    return color;
}


//Conjunto de manipulação da lista ligada
void getCell(cell *current,FILE *arq){
    cell *next;
    next=(cell*)malloc(sizeof(cell));
    fread(next,sizeof(cell),1,arq);
    current->nextCell = next;
    next->previousCell = current;
    next->nextCell = 0;
    setCellSprite(next);
}
void getAllCells(objects *obj, FILE *arq){
    cell aux;
    cell *c;
    c = &aux;
    while(feof(arq) == 0){
        getCell(c,arq);
        if(feof(arq) == 0) c=c->nextCell;
    }
    c->nextCell = 0;
    obj->firstCell = aux.nextCell;
    obj->firstCell->previousCell = 0;
}
void saveAllCells(cell *firstCell,FILE *arq){
    cell *c;
    c=firstCell;
    while(c!=0){
        fwrite(c,sizeof(cell),1,arq);
        c=c->nextCell;
    }
}
void freeAllCells(cell *firstCell){
    cell *current, *next;
    next = firstCell;
    do{
        current = next;
        next = current->nextCell;
        free(current);
    }while(next!=0);
}
int countCells(cell *firstCell){
    int n=0;
    cell *current;
    current=firstCell;
    while(current!=0){
        n++;
        current=current->nextCell;
    }
    return n;
}
void sortOneCell(cell *firstCell,int numCells){
    int numCurrent;
    cell *minCell, *current;
    minCell = firstCell;
    numCurrent = 1;
    current = firstCell->nextCell;
    int loop = TRUE;
    while(loop){
        if((current->p.h)<(minCell->p.h)){
            minCell=current;
        }
        if(current->nextCell!=0 && numCurrent<numCells ){
            current=current->nextCell;
            numCurrent++;
        }else loop = FALSE;
    }
    cell aux;
    aux = *minCell;
    minCell->previousCell = current->previousCell;
    minCell->nextCell = current->nextCell;
    current->previousCell = aux.previousCell;
    current->nextCell = aux.nextCell;
}
void sortAllCells(cell *firstCell){
    int numCells;
    numCells = countCells(firstCell);
    for(int n=numCells; n>1; n--) sortOneCell(firstCell,n);
}
horizontalPosition int2pos(int i){
    horizontalPosition ans;
    switch(i){
        case 0: ans = LEFT; break;
        case 1: ans = MIDDLE; break;
        case 2: ans = RIGHT; break;
    }
    return ans;
}
void randomizer(int length,int level){
    srand(time(NULL));
    cell *firstCell = (cell*)malloc(sizeof(cell));
    cell *current, *next;
    
    //Tabela para definição de variedade de células
    tipo_de_celula typeTab[6] = {SANGUE, MUTANTE, VIRUS, QUEIJO, COELHO, RIP};
    
    current = firstCell;
    current->previousCell=0;
    current->p.h = length;
    current->p.x = int2pos(rand()%3);
    current->type = typeTab[rand()%level];
    
    int h = current->p.h;
    while(h>45){
        next = (cell*)malloc(sizeof(cell));
        current->nextCell = next;
        next->previousCell = current;
        current = next;
        current->p.h = h - 2 - (7-level)*(1 + rand()%3);
        current->p.x = int2pos(rand()%3);
        current->type = typeTab[rand()%level];
        h = current->p.h;
    }
    current->nextCell = 0;
    
    char add[100];
    sprintf(add,"stage%d.w3d",level);
    
    FILE *arq = fopen(add,"wb");
    saveAllCells(firstCell,arq);
    fclose(arq); 
}



//Funções do menu do jogo
game_option int2option(int x){
    game_option ans;
    switch(x){
        case 0: ans=EASY_GAME; break;
        case 1: ans=MEDIUM_GAME; break;
        case 2: ans=HARD_GAME; break;
        case 3: ans=EXIT_GAME; break;
    }
    return ans;
}
void posToString(char string[], game_option cursorPosition, game_option ind){
    if(cursorPosition == ind) strcat(string," > ");
    else strcat(string, "   ");
}

int getMenuMovement(int previous){
    int min = 0;
    int max = 3;
    int current=previous;
    int loop = TRUE;
    while(loop){
        if(key[KEY_UP]){
            current--;
            loop = FALSE;
        }else if(key[KEY_DOWN]){
            current++;
            loop = FALSE;
        }else if(key[KEY_ENTER]||key[KEY_SPACE]){
            current = -10;
            loop = FALSE;
        }else if(key[KEY_ESC]){
            current = -20;
            loop = FALSE;
        }
    }
    if(current == -10) return current;
    else if(min<=current&&current<=max) return current;
    else return previous;
}

game_option callMenu(){
    BITMAP *buffer;
    buffer = create_bitmap(640,480);
    BITMAP *fundo;
    fundo = load_bitmap("graphics/title.bmp",NULL);
    
    game_option pos;
    game_option ans;
    pos = EASY_GAME;
    ans = NULL_OPTION;
    
    char phrase[100];
    while(ans == NULL_OPTION){
        blit(fundo,buffer,0,0,0,0,640,480);
        for(int i=0; i<=3; i++){
            strcpy(phrase,"");
            posToString(phrase,pos,int2option(i));
            strcat(phrase,game_option_tab[i]);
            textout_ex(buffer,font,phrase,155,320+i*10,makecol(0,0,0),-1);
        }
        updateScreen(buffer);
        rest(100);
        
        int aux;
        aux = getMenuMovement(pos);
        if(aux == -10){
            ans = pos;
        }else if(aux == -20){
            ans = EXIT_GAME;
        }else{
            pos = int2option(aux);
        }
    }
    return ans;
}



//*************************************
//Funções do Sistema
//*************************************
void init() {
	int depth, res;
	allegro_init();
	depth = desktop_color_depth();
	if (depth == 0) depth = 32;
	set_color_depth(depth);
	res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
	if (res != 0) {
		allegro_message(allegro_error);
		exit(-1);
	}

	install_timer();
	install_keyboard();
	install_mouse();
	install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,NULL);
	LOCK_VARIABLE(COUNTER);
    LOCK_FUNCTION(ADD_COUNTER);
    install_int(ADD_COUNTER,1);
	/* add other initializations here */
}

void deinit() {
	clear_keybuf();
	/* add other deinitializations here */
}
void updateScreen(BITMAP *buffer){
    vsync();
    blit(buffer,screen,0,0,0,0,640,480);
}
