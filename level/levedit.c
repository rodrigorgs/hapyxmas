/*
 * Level Editor for "Happy Christmas" - do not distribute
 * Editor de fases para "Feliz Natal" - nao distribua
 *   by Rodrigo Rocha Gomes e Souza (rod@svn.com.br)
 */
#include <stdio.h>
#include "allegro.h"
#include "hapyxmas.h"

#define XSIZE  19
#define YSIZE  12
#define XTILE  36
#define YTILE   8

#define TILES   1
#define BADGUYS 2
#define ITEMS   3
#define HERO    4


struct cursortype {
 int x,y,xscr,curtile;
} cursor;

DATAFILE *dat;
BITMAP *blank;
char *arg;
int cgt=TILES;  /* Current Group of Tiles */
int map[XSIZE*20][YSIZE];

int map_to_scr(int xxx) {
int i;
i=xxx%19;
return i;
}

int scr_to_map() {
int i;
i=XSIZE*cursor.xscr-XSIZE+cursor.x;
return i;
}

int lwr(int number) {
if (cursor.x==19) return 1;
else return number;
}

int upr(int number) {
if (cursor.x==1) return number+(XSIZE);
else return number;
}

save_level(char *filename) {
FILE *arq;
arq=fopen(filename, "wb");
/*for (yy=1;yy<=YSIZE;yy++){
for (xx=1;xx<=XSIZE;xx++){*/
  fwrite(map,sizeof(int),XSIZE*20*YSIZE,arq);
/*}}*/
fclose(arq);}

load_level(char *filename) {
FILE *arq;
arq=fopen(filename, "rb");
  fread(map,sizeof(int),XSIZE*20*YSIZE,arq);
fclose(arq);
}

init_cursor() {
cursor.x=1;cursor.y=12;cursor.xscr=1;cursor.curtile=TILE000;
}

draw_scr() {
int xx,yy;
for (xx=1;xx<=19;xx++) {
for (yy=1;yy<=YSIZE;yy++){
   blit(blank,screen,0,0,xx*16-15,yy*16-15,16,16);
   if (map[XSIZE*cursor.xscr-XSIZE+xx][yy] != 0)
     blit(dat[map[XSIZE*cursor.xscr-XSIZE+xx][yy]].dat,screen,0,0,xx*16-15,yy*16-15,16,16);
}}}

draw_tile(int tilecode) {
blit(dat[tilecode].dat,screen,0,0,cursor.x*16-15,cursor.y*16-15,16,16);
}

draw_tiles() {
int xx=-1,yy=1,i=0;
show_mouse(NULL);
rectfill(screen,0,200,639,479,50);

if (cgt==TILES) {
for (i=TILE000;i<=TILE335;i++) {
xx++;
if (xx>36) {xx=0;yy++;}
blit(dat[i].dat,screen,0,0,xx*16+xx,yy*16+200+yy,16,16);
}}

if (cgt==BADGUYS) {
draw_sprite(screen,dat[NASTY000].dat,0,200+yy);
draw_sprite(screen,dat[NASTY005].dat,16,200+yy);
draw_sprite(screen,dat[NASTY010].dat,46,200+yy);
draw_sprite(screen,dat[NASTY016].dat,93,200+yy);
draw_sprite(screen,dat[NASTY020].dat,123,200+yy);
draw_sprite(screen,dat[NAZTY000].dat,153,200+yy);
draw_sprite(screen,dat[NAZTY011].dat,169,200+yy);
draw_sprite(screen,dat[NAZTY014].dat,199,200+yy);
draw_sprite(screen,dat[NAZTY020].dat,252,200+yy);
draw_sprite(screen,dat[NAZTY028].dat,292,200+yy);
draw_sprite(screen,dat[NAZTY031].dat,335,200+yy);
draw_sprite(screen,dat[NAZTY035].dat,400,200+yy);
draw_sprite(screen,dat[NAZTY038].dat,428,200+yy);
}

if (cgt==ITEMS) {
for (i=POWER1;i<=PWR029;i++) {
xx++;
if (xx>27) {xx=0;yy++;}
draw_sprite(screen,dat[i].dat,xx*22,yy*18+200);
}}
show_mouse(screen);
}

init_all() {
allegro_init();
install_keyboard();
install_timer();
install_mouse();

set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0);
dat=load_datafile("hapyxmas.dat");
set_pallete(dat[PALETTE_01].dat);

init_cursor();
blank=create_bitmap(16,16);
clear(blank);
floodfill(screen,0,0,50);
rect(screen,0,0,XSIZE*16,YSIZE*16,0);
floodfill(screen,1,1,0);
draw_scr();
draw_tiles();
show_mouse(screen);
}

move_cursor() {
char buf[80];

if (key[KEY_ESC]) exit(0);
if (key[KEY_F1]) {cgt=TILES;draw_tiles();}
if (key[KEY_F2]) {cgt=BADGUYS;draw_tiles();}
if (key[KEY_F3]) {cgt=ITEMS;draw_tiles();}
if (key[KEY_F5]) {save_level(arg);}
/*if (key[KEY_F6]) {load_level();}*/

if ((key[KEY_HOME]) && (cursor.xscr>1))  {cursor.xscr--; draw_scr();}
if ((key[KEY_PGUP]) && (cursor.xscr<20)) {cursor.xscr++; draw_scr();}
if ((key[KEY_UP]) && (cursor.y>1)) cursor.y--;
if ((key[KEY_DOWN]) && (cursor.y<YSIZE)) cursor.y++;
if (key[KEY_LEFT]) {
  if (cursor.x>1) cursor.x--;
  else if (cursor.xscr>1) {cursor.xscr--; cursor.x=19; draw_scr();}
  }
if (key[KEY_RIGHT]){
  if (cursor.x<XSIZE) cursor.x++;
  else if (cursor.xscr<20) {cursor.xscr++; cursor.x=1; draw_scr();}
}
if (key[KEY_ENTER]) {
map[scr_to_map()][cursor.y]=cursor.curtile;
draw_tile(cursor.curtile);
}
if (key[KEY_5_PAD]){
map[scr_to_map()][cursor.y]=0;
blit(blank,screen,0,0,cursor.x*16-15,cursor.y*16-15,16,16);
}
clear_keybuf();

sprintf(buf, "X = %d  Y = %d  SCREEN = %d  ", cursor.x,cursor.y,cursor.xscr);
textout_ex(screen,font,buf,310,10,255,_textmode);
blit(dat[cursor.curtile].dat,screen,0,0,cursor.x*16-15,cursor.y*16-15,16,16);
rest(50);
if (map[XSIZE*cursor.xscr+(cursor.x-19)][cursor.y] != 0)
  blit(dat[map[scr_to_map()][cursor.y]].dat,screen,0,0,cursor.x*16-15,cursor.y*16-15,16,16);
else
  blit(blank,screen,0,0,cursor.x*16-15,cursor.y*16-15,16,16);
rest(25);
}

pick_tile() {
/*  blit(dat[TILE000+i].dat,screen,0,0,xx*16+xx,yy*16+200+yy,16,16);*/
if (cgt==TILES) {
if ((mouse_y >= 200) && (mouse_b & 1)) {
  cursor.curtile=TILE000+(((mouse_y-200)/17)*37-37)+(mouse_x/17);
  if (cursor.curtile>TILE335) cursor.curtile=TILE335;
}}
if (cgt==BADGUYS) {
if ((mouse_b & 1) && (mouse_x<474) && (mouse_y>=200) && (mouse_y<=240)){
if      (mouse_x<16)  cursor.curtile=NASTY000;
else if (mouse_x<46)  cursor.curtile=NASTY005;
else if (mouse_x<93)  cursor.curtile=NASTY010;
else if (mouse_x<123) cursor.curtile=NASTY016;
else if (mouse_x<153) cursor.curtile=NASTY020;
else if (mouse_x<169) cursor.curtile=NAZTY000;
else if (mouse_x<199) cursor.curtile=NAZTY011;
else if (mouse_x<252) cursor.curtile=NAZTY014;
else if (mouse_x<292) cursor.curtile=NAZTY020;
else if (mouse_x<335) cursor.curtile=NAZTY028;
else if (mouse_x<400) cursor.curtile=NAZTY031;
else if (mouse_x<428) cursor.curtile=NAZTY035;
else                  cursor.curtile=NAZTY038;
if (cursor.curtile>NAZTY038) cursor.curtile=NAZTY038;
}}

if (cgt==ITEMS) {
if ((mouse_y >= 200) && (mouse_b & 1) && (mouse_y<=244)) {
  cursor.curtile=POWER1+(((mouse_y-200)/18)*28-28)+(mouse_x/22);
  if (cursor.curtile>PWR029) cursor.curtile=PWR029;
}}

}

int main(int argc,char *argv[]) {
FILE *temp;
int load=0;

arg=argv[1];
if (!arg) {printf ("You need to specify a filename!"); readkey(); exit(0);}
temp=fopen(arg,"r");
if (temp) load=1;
fclose(temp);
if (load) load_level(arg);


init_all();


for(;;){
pick_tile();
move_cursor();
}

return 0;
}
END_OF_MAIN()