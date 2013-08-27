#include "define.h"
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


//*************************************
//Função Main
//*************************************
int main() {
	init();
	
	MIDI *music = load_midi("graphics/music.mid");
	play_midi(music,TRUE);
	
	int loop = true;
	while(loop){
        objects *obj = (objects*)malloc(sizeof(objects));
        spaceship *ship = (spaceship*)malloc(sizeof(spaceship));
        obj->ship = ship;
        setShipSprite(obj->ship);
        
        game_option escolha;
        escolha = callMenu();
        switch(escolha){
            case EASY_GAME: ship->speed = 1; break;
            case MEDIUM_GAME: ship->speed = 3; break;
            case HARD_GAME: ship->speed = 5; break;
            case EXIT_GAME: loop = FALSE; break;
        }
        int level = 1;
        while(loop&&level<=6){
            char path[100] = "graphics/";
            char filename[100];
            sprintf(filename,"stage%d.w3d",level);
            strcat(path,filename);
            FILE *stage = fopen(path,"rb");
            getAllCells(obj,stage);
            fclose(stage);
            
            game_condition progress;
            int background = backgroundColor(level);
            progress = gameLoop(obj,background);
            switch(progress){
                case NEXT_STAGE: level++; break;
                case GAME_OVER:{
                    BITMAP *gameOver;
                    gameOver = load_bitmap("graphics/gameover.bmp",NULL);
                    updateScreen(gameOver);
                    rest(2000);
                    level = 100;
                }break;
                case LEAVE_GAME: loop = FALSE; break;
            }
        }
        if(level==7){
            BITMAP *win;
            win = load_bitmap("graphics/venceu.bmp",NULL);
            updateScreen(win);
            rest(2000);
        }
    }
	
	deinit();
	return 0;
}
END_OF_MAIN()
