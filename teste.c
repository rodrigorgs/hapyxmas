/*    _   _   _____   _     _    _        _
 *   | | | | |     | |  \  |  \  \ \    / /
 *   | |_| | |  O  | | D | | D |   \ \/ /
 *   |  _  | |  _  | |  /  |  /     `  '
 *   | | | | | | | | | |   | |       ||
 *   |_| |_| |_| |_| |_|   |_|       ||
 *
 *    ____   _   _   _      _   ____   _______   _    _   _____   ____
 *   |  __) | | | | |  \   | | '  __) (__   __) |  \/  | |     | '  __)
 *   | |    | |_| | | D )  | | | (__     | |    |      | |  O  | | (__
 *   | |    |  _  | |  /   | | `__  `    | |    | |\/| | |  _  | `__  `
 *   | |__  | | | | |  \   | |  __) |    | |    | |  | | | | | |  __) |
 *   |____) |_| |_| |_|\_\ |_| (____'    |_|    |_|  |_| |_| |_| (____'
 *
 *     by Rodrigo Rocha Gomes e Souza (rod@svn.com.br)
 *          for 1997's Christmas
 *
 *     Graphics by Ari Feldman (www.walrus.com/~ari/spritlib.htm)
 *                         special thanks to D.J. Delorie and Shawn Hargreaves
 *     [see readme.txt for more informations]
 */


/* I don't speak English very well, so ignore my mistakes =) */
#include <stdio.h>
#include "allegro.h"
#include "hapyxmas.h"
#include "add.h"

#define GAME_DELAY 75 /* Milliseconds per cicle */
#define XSIZE 19      /* Max number of tiles in a row */
#define YSIZE 12      /*  "    "    "    "    " a column */
#define MAXSCR 20     /* Maximum of screens */
//#define NOFLEV 6      /* Number of levels */
#define DMAXJUMP 12
#define DVEL 5        /* Default player's velocity */
#define DJUMP 4       /* Default player's jump */

#define VOL 255
#define PAN 128
#define PITCH 1000

struct herotype {
  int x1,x2,x3; /* x1=tile; x2= position on tile,x3=screen*/
  int endx1,endx2; /* The same as above, but final pixel */
  int y1,y2;    /* The same as above */
  int dx;    /* x movement. -128 = left; 128 = right; 0 = stopped */
  int olddx;  /* The dx of previous cicle */
  int dy;    /* the same as in dx */
  int olddy;/* the same as in olddx */
  int dir;   /* if the hero is looking to right (1) or to left (-1) */
  int olddir; /* Do I need to repeat what means the prefix ``old''? */
  int frame; /* what frame is going to be displayed */
  int vel;   /* hero velocity */
  int life;  /* life */
  int level; /* level */
  int maxjump; /**/
  int jump; /* if hero is jumping, and how much he's jumping */
  int jjump; /* how much high is the jump */
  int score; /* Player score */
  int jump_flag;
  int slow_flag;
  int inv_flag;
  int stop_flag;
  int twox_flag;
  int fire_flag;
} player;

struct monstertype {
  int x1,x2,x3; /* x1=tile; x2= position on tile,x3=screen*/
  int endx1,endx2; /* The same as above, but final pixel */
  int y1,y2;    /* The same as above */
  int topy1,topy2; /* Ypos of top */
  int dir;   /* if the badguy is looking right (1) or left (-1) */
  int olddir; /* Do I need to repeat what means the prefix ``old''? */
  int frame; /* what frame is going to be displayed */
  int maxframe;
  int vel;   /* velocity */
  int life;  /* life */
  int left;
  int fly;
  int w;
  int h;
  int id;
  int tle;
} badguy [100];  /* What about 100 badguys in a level? */

DATAFILE *dat; /* for the datafile */
DATAFILE *add; /* additional datafile */
BITMAP *hs1; /* hidden_screen1 - this will contain the scenary */
BITMAP *hs2; /* hidden_screen2 - this will add objects, hero, badguys, etc. */
BITMAP *blank; /* clean tile */
PALLETE gamepal; /* normal game palette */
PALLETE initpal; /* pallete of first screen */

int map[XSIZE*MAXSCR][YSIZE];
int nof_lev;
int santa_w[5] = {22,27,27,27,27};
int tile_tab[337]; /* Tile_Table - 1 if TILE is blocker, 2 if it is killer, 0 if it is NULL */
int nof_bad; /* Number of badguys */
int nlev; /* The hero reached the final of level */
int bad_vel[13] = {4,5,10,5,5,4,2,6,2,1,3,4,2}; /* velocity */
int bad_frm[13] = {5,4,6,4,4,4,3,6,7,3,4,3,3}; /* number of frames */
int bad_lfe[13] = {1,1,1,1,1,1,1,1,1,1,1,1,1}; /* lives */
int bad_wdt[13] = {16,30,47,30,30,16,30,53,40,43,65,28,44}; /* width */
int bad_hgt[13] = {16,30,27,30,30,20,30,25,30,37,40,23,20}; /* height */
int bad_fly[13] = {1,0,0,0,0,1,0,1,0,0,0,0,0}; /* can fly */
int bad_tle[13] = {NASTY000,NASTY005,NASTY010,NASTY016,NASTY020,NAZTY000,
NAZTY011,NAZTY014,NAZTY020,NAZTY028,NAZTY031,NAZTY035,NAZTY038}; /*tilecode*/
// I didn't find a function that converts an integer to a string...
// And I don't know how to make this.
// If you know, e-mail me
char lev_arq[99][12] = {"level01.lev","level02.lev","level03.lev",
"level04.lev","level05.lev","level06.lev","level07.lev","level08.lev",
"level09.lev","level10.lev","level11.lev","level12.lev","level13.lev",
"level14.lev","level15.lev","level16.lev","level17.lev","level18.lev",
"level19.lev","level20.lev","level21.lev","level22.lev","level23.lev",
"level24.lev","level25.lev","level26.lev","level27.lev","level28.lev",
"level29.lev","level30.lev","level31.lev","level32.lev","level33.lev",
"level34.lev","level35.lev","level36.lev","level37.lev","level38.lev",
"level39.lev","level40.lev","level41.lev","level42.lev","level43.lev",
"level44.lev","level45.lev","level46.lev","level47.lev","level48.lev",
"level49.lev","level50.lev","level51.lev","level52.lev","level53.lev",
"level54.lev","level55.lev","level56.lev","level57.lev","level58.lev",
"level59.lev","level60.lev","level61.lev","level62.lev","level63.lev",
"level64.lev","level65.lev","level66.lev","level67.lev","level68.lev",
"level69.lev","level70.lev","level71.lev","level72.lev","level73.lev",
"level74.lev","level75.lev","level76.lev","level77.lev","level78.lev",
"level79.lev","level80.lev","level81.lev","level82.lev","level83.lev",
"level84.lev","level85.lev","level86.lev","level87.lev","level88.lev",
"level89.lev","level90.lev","level91.lev","level92.lev","level93.lev",
"level94.lev","level95.lev","level96.lev","level97.lev","level98.lev",
"level99.lev"
};

//             0 1 2 3 4 5 6 7 8 9
int pwr[36] = {1,1,0,1,2,3,0,1,0,1,
               1,1,1,0,0,2,0,1,2,1,
               2,1,1,0,1,0,2,2,1,1,
               1,1,1,0,3,0};

int z1(int number1, int number2) {
/*if (number2>16) return number1+1;else return number1;*/
return number1+(number2/16);
}

int z2(int number1, int number2) {
/*if (number2>16) return number2-16;else return number2;*/
while (number2>16) {
      number2-=16;
      }
      return number2;
}

int tilet (int xxx,int yyy) {
if (map[scr_to_map(player.x3,xxx)][yyy]==0) return 0;
if ((map[scr_to_map(player.x3,xxx)][yyy]<TILE000) &&
                   (map[scr_to_map(player.x3,xxx)][yyy]>TILE335)) return 0;
return (tile_tab[map[scr_to_map(player.x3,xxx)][yyy]-TILE000]);
}

int santa_y(yyy){
return yyy-30;
}

int map_to_scr(int xxx) {
return xxx%19;
}

int scr_to_map(int scren, int xxx) {
return XSIZE*scren-XSIZE+xxx;
}

void quit(int exitcode) {
if (exitcode==0) {
   clear(screen);
   play_midi(dat[midi34].dat,TRUE);
   // Display credits
   set_pallete(add[CREDPAL].dat);
   blit(add[CREDITS].dat,screen,0,0,0,0,SCREEN_W,SCREEN_H);
   clear_keybuf();
   readkey();
   stop_midi();
   }
if (exitcode==1) {
   clear(screen);
   play_midi(dat[midi34].dat,TRUE);
   set_pallete(add[ENDPAL].dat);
   blit(add[END1].dat,screen,0,0,0,0,SCREEN_W,SCREEN_H);
   clear_keybuf();
   readkey();
   blit(add[END2].dat,screen,0,0,0,0,SCREEN_W,SCREEN_H);
   clear_keybuf();
   readkey();
   // Display credits
   clear(screen);
   set_pallete(add[CREDPAL].dat);
   blit(add[CREDITS].dat,screen,0,0,0,0,SCREEN_W,SCREEN_H);
   clear_keybuf();
   readkey();
   stop_midi();
   }
exit(exitcode);
}

void reset_monsters() {
int i;
for (i=1;i<nof_bad-1;i++) {
      badguy[i].vel=0;badguy[i].maxframe=0;badguy[i].life=0;badguy[i].w=0;
      badguy[i].h=0;badguy[i].fly=0;badguy[i].tle=0;badguy[i].dir=0;
      badguy[i].x3=0;badguy[i].x2=0;badguy[i].x1=0;badguy[i].y1=0;
      badguy[i].y2=0;badguy[i].dir=0;badguy[i].frame=0;
      }
nof_bad=0;
}

void load_level(char *filename) {
int xx,yy,count=0;
FILE *arq;

reset_monsters();

arq=fopen(filename, "rb");
  fread(map,sizeof(int),XSIZE*20*YSIZE,arq);
fclose(arq);

for (xx=1;xx<=XSIZE*20;xx++) {
for (yy=1;yy<=YSIZE;yy++){
  if ((map[xx][yy]>=NASTY000) && (map[xx][yy]<=NAZTY038)) {/* Oh, a badguy! */
    count++;

      if (map[xx][yy]==NASTY000) badguy[count].id=0;
      if (map[xx][yy]==NASTY005) badguy[count].id=1;
      if (map[xx][yy]==NASTY010) badguy[count].id=2;
      if (map[xx][yy]==NASTY016) badguy[count].id=3;
      if (map[xx][yy]==NASTY020) badguy[count].id=4;
      if (map[xx][yy]==NAZTY000) badguy[count].id=5;
      if (map[xx][yy]==NAZTY011) badguy[count].id=6;
      if (map[xx][yy]==NAZTY014) badguy[count].id=7;
      if (map[xx][yy]==NAZTY020) badguy[count].id=8;
      if (map[xx][yy]==NAZTY028) badguy[count].id=9;
      if (map[xx][yy]==NAZTY031) badguy[count].id=10;
      if (map[xx][yy]==NAZTY035) badguy[count].id=11;
      if (map[xx][yy]==NAZTY038) badguy[count].id=12;

      badguy[count].vel=bad_vel[badguy[count].id];
      badguy[count].maxframe=bad_frm[badguy[count].id]-1;
      badguy[count].life=bad_lfe[badguy[count].id];
      badguy[count].w=bad_wdt[badguy[count].id];
      badguy[count].h=bad_hgt[badguy[count].id];
      badguy[count].fly=bad_fly[badguy[count].id];
      badguy[count].tle=bad_tle[badguy[count].id];
      badguy[count].dir=-1;
      badguy[count].x3=(xx/XSIZE)+1;
      badguy[count].x2=1;
      badguy[count].x1=(xx%XSIZE);
      badguy[count].y1=yy;
      badguy[count].y2=16;
      badguy[count].dir=-1;
      badguy[count].frame=0;
    map[xx][yy]=0;
  }
}
}
nof_bad=count;
}


void move_bad() {
int i,j,c,can_move_right=TRUE,can_move_left=TRUE,oldscr;

// The loop //
for (i=0;i<=nof_bad-1;i++){

// reset can_move values
can_move_left=TRUE;can_move_right=TRUE;c=0;


if (badguy[i].x3==player.x3) {

oldscr=badguy[i].x3;

// Calculate endx //
/*badguy[i].endx1=badguy[i].x1;
badguy[i].endx2=badguy[i].endx2;*/
badguy[i].endx2=z2(badguy[i].x1,badguy[i].x2+bad_wdt[badguy[i].id]);
badguy[i].endx1=z1(badguy[i].x1,badguy[i].endx2);
/*badguy[i].endx2=badguy[i].x2+bad_wdt[badguy[i].id];
badguy[i].endx1=badguy[i].x2+(
 if (badguy[i].endx2>16) {badguy[i].endx2=1+(badguy[i].endx2-16); badguy[i].endx1++;} /* max */

// Can the badguy move left/right? //
   // No, 'cause there is a blocker tile //
if (tilet(z1(badguy[i].endx1,badguy[i].endx2+badguy[i].vel+1),badguy[i].y1)==1) can_move_right=0;
//if (tilet(z1(badguy[i].endx1,badguy[i].endx2+badguy[i].vel-1),badguy[i].y1-1)==1) can_move_right=0;
if (tilet(z1(badguy[i].x1,badguy[i].x2-badguy[i].vel-1),badguy[i].y1)==1) can_move_left=0;
//if (tilet(z1(badguy[i].x1,badguy[i].x2-badguy[i].vel),badguy[i].y1-1)==1) can_move_left=0;
   // No, 'cause badguys can't move to another screen //
if ((badguy[i].endx1==XSIZE) && (badguy[i].endx2<16-badguy[i].vel)) can_move_right=0;
if ((badguy[i].x1==1) && (badguy[i].x2>1+badguy[i].vel)) can_move_left=0;
   // No, a hole //
   if (!bad_fly[badguy[i].id]){
      if (tilet(badguy[i].endx1+1,badguy[i].y1+1)==0) can_move_right=0;
      if (tilet(badguy[i].x1-1,badguy[i].y1+1)==0) can_move_left=0;
      }
   // Else, it can :-) //

// Move left/right, if possible. If not, change direction //
if (badguy[i].dir<0) {
   if (can_move_left) badguy[i].x2-=badguy[i].vel;
   else {badguy[i].dir*=-1;c=1;}
   }
if (badguy[i].dir>0) {
   if (can_move_right) badguy[i].x2+=badguy[i].vel;
   else if (c==0) badguy[i].dir*=-1;
   }

// The badguy is in same place as the blocker tile //
/*if ((badguy[i].dir>0) && (tilet(badguy[i].endx2,badguy[i].y1)==1))
   badguy[i].dir=-1;
if ((badguy[i].dir<0) && (tilet(badguy[i].x2,badguy[i].y1)==1))
   badguy[i].dir=1;*/

// Update x1, x2 //
if (badguy[i].x2<1) {badguy[i].x2=16+badguy[i].x2; badguy[i].x1--;} /* min */
if (badguy[i].x2>16) {badguy[i].x2=1+(badguy[i].x2-16); badguy[i].x1++;} /* max */

// Ha-ha-ha! I got the hero! //
if (((z1(badguy[i].x1,badguy[i].x2)==z1(player.x1,player.x2)) || (z1(badguy[i].endx1,badguy[i].endx2)==z1(player.x1,player.x2)))
  //&& ((badguy[i].y1-(bad_hgt[badguy[i].id]/16)>=player.y1-1) && (badguy[i].y1<=player.y1))
    && ((badguy[i].y1==player.y1) || (badguy[i].y1==player.y1-1)))
{
     if (!player.fire_flag) {
     if (!player.inv_flag) {
       player.life--;
       player.inv_flag=40;
       }
       }
     else
       {badguy[i].x3=-1; player.score+=10;}
   }

// Badguy.life==0? Hasta la vista, badguy! //
if (badguy[i].life==0) badguy[i].x3=-3;

// Let's give them some aNiMaTiOn! //
badguy[i].frame++;
if (badguy[i].frame==badguy[i].maxframe) badguy[i].frame=0;

  }
  }
}

void draw_bad() {
int i;

for (i=0;i<=nof_bad-1;i++)
if (badguy[i].x3==player.x3) {
if ((badguy[i].dir<0) || ((badguy[i].id==2) && (badguy[i].dir>0)))
   draw_sprite(hs2,dat[badguy[i].tle+badguy[i].frame].dat,(badguy[i].x1*16-15)+(badguy[i].x2-1),(badguy[i].y1*16-15)-bad_hgt[badguy[i].id]+(badguy[i].y2-1));
if ((badguy[i].dir>0) || ((badguy[i].id==2) && (badguy[i].dir<0)))
   draw_sprite_h_flip(hs2,dat[badguy[i].tle+badguy[i].frame].dat,(badguy[i].x1*16-15)+(badguy[i].x2-1),(badguy[i].y1*16-15)-bad_hgt[badguy[i].id]+(badguy[i].y2-1));
   }
}

void draw_scr() {
int xx,yy,i;
for (i=1;i<=2;i++){
for (xx=1;xx<=XSIZE;xx++) {
for (yy=1;yy<=YSIZE;yy++){
   if (i==1)
      blit(blank,hs1,0,0,xx*16-15,yy*16-15,16,16);
   else {
   if (map[XSIZE*player.x3-XSIZE+xx][yy]!=0) {
   if ((map[XSIZE*player.x3-XSIZE+xx][yy]>=POWER1) && (map[XSIZE*player.x3-XSIZE+xx][yy]<=PWR029))
     draw_sprite(hs1,dat[map[XSIZE*player.x3-XSIZE+xx][yy]].dat,xx*16-15,(yy*16-15));
   else
     blit(dat[map[XSIZE*player.x3-XSIZE+xx][yy]].dat,hs1,0,0,xx*16-15,(yy*16-15),16,16);
}}}}}
blit(hs1,hs2,0,0,0,0,SCREEN_W,SCREEN_H);
}

void move_water() {
int xx,yy;
for (xx=1;xx<=XSIZE;xx++) {
for (yy=1;yy<=YSIZE;yy++){
      if (map[XSIZE*player.x3-XSIZE+xx][yy]==98)
         {(map[XSIZE*player.x3-XSIZE+xx][yy]==99); break;}
      else if (map[XSIZE*player.x3-XSIZE+xx][yy]==99)
              {(map[XSIZE*player.x3-XSIZE+xx][yy]==98); break;}
}}
}

int sgn(int number) {
if (number<0) return -1;
if (number>0) return 1;
if (number==0) return 0;
}

void init_hero() {
player.x1 = 2;
player.x2 = 1;
player.x3 = 1;
player.y1 = 1;
player.y2 = 16;
player.dx = 0;
player.dir = 1;
player.frame = 1;
player.vel = DVEL;
player.level = 1;
player.jjump = DJUMP;
player.maxjump = DMAXJUMP;
player.life = 3;
player.fire_flag=0;
}

void start_pos() {
player.x1 = 2;
player.x2 = 1;
player.x3 = 1;
player.y1 = 1;
player.y2 = 16;
player.dx = 0;
player.dir = 1;
player.frame = 1;
player.fire_flag=0;
draw_scr();
blit(hs1,hs2,0,0,0,0,SCREEN_W,SCREEN_H);
}

void draw_hero(/*int x1, int x2, int dir*/) {
/* copy hero to hs2 */
if (!player.fire_flag) {
if (player.frame==1) {
  if (player.dir>0) draw_sprite(hs2,add[SNT1].dat,(player.x1*16-15)+(player.x2-1),santa_y(player.y1*16-15)+(player.y2-1));
  else       draw_sprite_h_flip(hs2,add[SNT1].dat,(player.x1*16-15)+(player.x2-1),santa_y(player.y1*16-15)+(player.y2-1));
}
if ((player.frame==2)||(player.frame==4)){
  if (player.dir>0) draw_sprite(hs2,add[SNT2].dat,(player.x1*16-15)+(player.x2-1),santa_y(player.y1*16-15)+(player.y2-1));
  else       draw_sprite_h_flip(hs2,add[SNT2].dat,(player.x1*16-15)+(player.x2-1)-5,santa_y(player.y1*16-15)+(player.y2-1));
}
if (player.frame==3){
   if (player.dir>0) draw_sprite(hs2,add[SNT3].dat,(player.x1*16-15)+(player.x2-1),santa_y(player.y1*16-15)+(player.y2-1));
   else       draw_sprite_h_flip(hs2,add[SNT3].dat,(player.x1*16-15)+(player.x2-1)-5,santa_y(player.y1*16-15)+(player.y2-1));
}
} // end -- if (!player.fire_flag) {
if (player.fire_flag){
   if (player.dir>0) draw_sprite(hs2,add[SNT_A].dat,(player.x1*16-15)+(player.x2-1),santa_y(player.y1*16-15)+(player.y2-1));
   else       draw_sprite_h_flip(hs2,add[SNT_A].dat,(player.x1*16-15)+(player.x2-1)-5,santa_y(player.y1*16-15)+(player.y2-1));
}
}

/* Gets pressed keys and move the hero (checking position, etc.) */
void move_hero() {
int can_move_left=TRUE,can_move_right=TRUE;

player.olddx=player.dx;
player.olddir=player.dir;
player.dx=0;
player.dir=0;

/* Read what keys are pressed */
if (!player.stop_flag) {
if (key[KEY_LEFT])  {player.dx-=128; player.dir--;}
if (key[KEY_RIGHT]) {player.dx+=128; player.dir++;}
if (key[KEY_UP]) {
/* O heroi nao estava pulando, entao apertou a tecla UP */
if ((!player.jump) && (tile_tab[map[scr_to_map(player.x3,z1(player.x1,player.x2+santa_w[player.frame]/2))][player.y1+1]-TILE000]==1) && (player.y2==16))
  player.jump++;
/* O heroi ja' estava pulando */
if ((player.jump) && /*((map[scr_to_map(player.x3,player.x1)][player.y1-2]==0)) && */(player.jump<player.maxjump))
  player.jump++;
}
if (player.jump==player.maxjump) player.jump=0;
if (!key[KEY_UP])
player.jump=0;
if ((key[KEY_SPACE]) || (key[KEY_ENTER])) {
  if (!player.fire_flag) {
    player.fire_flag=10;
    play_sample(add[SND_SOCK].dat,VOL,PAN,PITCH,FALSE);
    }
  if (player.fire_flag)
    player.fire_flag--;
}
if ((!key[KEY_SPACE]) && (!key[KEY_ENTER])) player.fire_flag=0;
}

if (key[KEY_ESC]) quit(0);

/* Calculate endx1 and endx2 */
player.endx1=player.x1;
player.endx2=player.x2+santa_w[player.frame-1];
if (player.endx2>16) {player.endx2=1+(player.endx2-16); player.endx1++;} /* max */

/* Update Y pos */
if (player.jump) player.y2-=player.jjump; /* hero is jumping */

/* What goes up... must faaaaaaall..........! */
if ((((tile_tab[map[scr_to_map(player.x3,z1(player.x1,player.x2+santa_w[player.frame]/2))][player.y1+1]-TILE000])!=1) && (!player.jump)) || (player.y1<1)) player.y2+=player.jjump;
if (((tile_tab[map[scr_to_map(player.x3,z1(player.x1,player.x2+santa_w[player.frame]/2))][player.y1+1]-TILE000])==1) && (!player.jump) && (player.y2!=16)) {player.y2+=player.jjump; if (player.y2>16) player.y2=16;}

/* player.dir can't be 0 */
if (player.dir==0) player.dir=player.olddir;

/* Change walk direction if direction has changed */
if (sgn(player.olddx) != sgn(player.dx)) {
if (player.dx<0) player.frame = 1;
if (player.dx>0) player.frame = 1;}

/* Checks if hero can move left or right */
if (tilet(z1(player.endx1,player.endx2+player.vel-1),player.y1)==1) can_move_right=0;
if (tilet(z1(player.endx1,player.endx2+player.vel-1),player.y1-1)==1) can_move_right=0;
if (tilet(z1(player.x1,player.x2-player.vel),player.y1)==1) can_move_left=0;
if (tilet(z1(player.x1,player.x2-player.vel),player.y1-1)==1) can_move_left=0;
if ((player.x1==1) && (player.x2<=player.vel) && (player.x3==1)) can_move_left=0;

/* Update player position and player frame */
if ((player.dx<0) && (can_move_left== TRUE)) {player.x2-=player.vel; player.frame++;} /* left */
if ((player.dx>0) && (can_move_right==TRUE)) {player.x2+=player.vel; player.frame++;} /* right */

/* Checks if player is out of the limits of the current tile */
   /* X pos */
if (player.x2<1) {player.x2=16+player.x2; player.x1--;} /* min */
if (player.x2>16) {player.x2=1+(player.x2-16); player.x1++;} /* max */
   /* Y pos */
if (player.y2<1) {player.y2=16+player.y2; player.y1--;} /* min */
if (player.y2>16) {player.y2=1+(player.y2-16); player.y1++;} /* max */

/* Checks if player.frame had exceeded the maximum for the current direction */
if (player.frame>4) player.frame=1; /* left */
if (player.frame<1) player.frame=4; /* right */
/* Checks if player has moved to another screen */
if (player.x1>19) {player.x3++; player.x1=1; player.x2=1; draw_scr();}
if (player.x1<1)  {player.x3--; player.x1=19;player.x2=16;draw_scr();}

/**/
if ((can_move_left==0) && (player.dx<1)) player.dx=0;
if ((can_move_right==0) && (player.dx>1)) player.dx=0;

/* If the hero is stopped, put a sprite where he is stopped */
if ((player.dx==0) && (player.dir==+1)) player.frame=1;
if ((player.dx==0) && (player.dir==-1)) player.frame=1;

/* Ouch!!!!!! */
if ((tilet(z1(player.x1,player.x2+santa_w[player.frame]),player.y1)==2) && (player.y1>=1))
  {
  if (!player.inv_flag) {
    player.life--;
    player.inv_flag=40;
    }
  }

/* Out of scenary... */
if (player.y1>YSIZE) {player.life--; play_sample(add[SND_FALL].dat,VOL,PAN,PITCH,FALSE); start_pos();}
if (player.x3<1) {player.x3=1; player.x1=1;}

/* Sorry, but you're dead :P */
if (player.life<1) quit(0);
}

/* Does Santa have picked an item? */
void chk_item() {
int u,d,i,a;
// This isn't THE place, but let's check if hero has passed the level...
if ((map[scr_to_map(player.x3,z1(player.x1,player.x2+santa_w[player.frame]/2))][player.y1-1]==TILE000+263) ||
   (map[scr_to_map(player.x3,z1(player.x1,player.x2+santa_w[player.frame]/2))][player.y1-1]==TILE000+264) ||
   ((map[scr_to_map(player.x3,z1(player.x1,player.x2+santa_w[player.frame]/2))][player.y1]==TILE000+263) ||
   (map[scr_to_map(player.x3,z1(player.x1,player.x2+santa_w[player.frame]/2))][player.y1]==TILE000+264)))
     {nlev=TRUE; play_sample(add[SND_LEV].dat,VOL,PAN,PITCH,FALSE);}



u=map[scr_to_map(player.x3,z1(player.x1,player.x2+santa_w[player.frame]/2))][player.y1-1];
d=map[scr_to_map(player.x3,z1(player.x1,player.x2+santa_w[player.frame]/2))][player.y1];

for (i=1;i<=2;i++) {
if (i==1) a=u;
else a=d;

if ((a>=POWER1) && (a<=PWR029)) {
if (a==u) map[scr_to_map(player.x3,z1(player.x1,player.x2+santa_w[player.frame]/2))][player.y1-1]=0;
if (a==d) map[scr_to_map(player.x3,z1(player.x1,player.x2+santa_w[player.frame]/2))][player.y1  ]=0;
draw_scr();
if (pwr[a-POWER1]==1) player.score+=100;
if (pwr[a-POWER1]==2) player.score+=500;
if (pwr[a-POWER1]==3) player.score+=1000;
if (pwr[a-POWER1]==0) {
   if (a-POWER1==2) {player.life++; play_sample(add[SND_HEART].dat,VOL,PAN,PITCH,FALSE);}
   if (a-POWER1==6) {player.vel/=2; player.slow_flag=67;}
   if (a-POWER1==8) {player.maxjump*=2; player.jump_flag=40;}
   if (a-POWER1==12) start_pos();
   if (a-POWER1==13) {player.life--; start_pos(); play_sample(add[SND_CAV].dat,VOL,PAN,PITCH,FALSE);}
   if (a-POWER1==15) player.life--;
   if (a-POWER1==23) {player.inv_flag=40; play_sample(add[SND_INV].dat,VOL,PAN,PITCH,FALSE);}
   if (a-POWER1==25) player.life=3;
   if (a-POWER1==35) {player.vel*=2; player.twox_flag=40;}
   if (a-POWER1==33) {player.stop_flag=63; player.dx=0;player.jump=0;}
} // end if
} // end if
} // end for

} // end void

void eva_item() {
// Decrease //
if (player.twox_flag>0) player.twox_flag--;
if (player.slow_flag>0) player.slow_flag--;
if (player.stop_flag>0) player.stop_flag--;
if (player.jump_flag>0) player.jump_flag--;
if (player.inv_flag>0) player.inv_flag--;
// Stops the effect //
if ((player.twox_flag==0) && (player.vel!=DVEL)) player.vel=DVEL;
if ((player.slow_flag==0) && (player.vel!=DVEL)) player.vel=DVEL;
if ((player.jump_flag==0) && (player.maxjump!=DMAXJUMP)) player.maxjump=DMAXJUMP;
}

void display_score() {
char buf[80];
  sprintf(buf, "LIVES: %d  POINTS: %d | %d %d %d %d %d",player.life,player.score,player.inv_flag,player.slow_flag,player.jump_flag,player.stop_flag,player.twox_flag);
  textout(hs2,font,buf,0,0,255);
}


void help() {
clear(screen);
set_pallete(add[INSPAL].dat);
blit(add[INS1].dat,screen,0,0,0,0,SCREEN_W,SCREEN_H);
clear_keybuf();
readkey();
blit(add[INS2].dat,screen,0,0,0,0,SCREEN_W,SCREEN_H);
readkey();
clear(screen);
}

void main() {
int i,j;
char flnm[11];
FILE *arq;
/*char buf[80];*/

        /* init allegro, keyboard and timer */
        allegro_init();
        install_keyboard();
        install_timer();
        /**/
set_gfx_mode(GFX_VGA, 320,200,0,0);  /* set video mode to VGA */

dat = load_datafile("hapyxmas.dat");
add = load_datafile("add.dat");

/* Initialize hidden screens */
hs1 = create_bitmap(SCREEN_W,SCREEN_H); clear(hs1);
hs2 = create_bitmap(SCREEN_W,SCREEN_H); clear(screen);
blank=create_bitmap(16,16); clear(blank);

/* Initial Screen */
set_pallete(add[INITPAL].dat);
install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,NULL); /* initialize */
blit(add[INIT].dat,screen,0,0,0,0,SCREEN_W,SCREEN_H);
play_midi(dat[midi01].dat, TRUE); /* Play John Lennon's "Happy Christmas" */
i=0;
do {
if (key[KEY_F1]) i=1;
if (key[KEY_ESC]) quit(0);
if (key[KEY_ENTER]) i=2;
if (key[KEY_N]) i=2;
} while (!i);
clear_keybuf();
if (i==1) help();

/**/
set_pallete(dat[PALETTE_01].dat);            /* load game's pallete */


for (i=TILE000;i<=TILE335;i++)
  tile_tab[i-TILE000]=0;
for (i=TILE000;i<=TILE007;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE017;i<=TILE035;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE048;i<=TILE070;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE083;i<=TILE097;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE105;i<=TILE105;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE111;i<=TILE112;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE117;i<=TILE134;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE147;i<=TILE164;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE177;i<=TILE179;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE188;i<=TILE193;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE206;i<=TILE223;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE237;i<=TILE254;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE260;i<=TILE261;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE266;i<=TILE281;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE296;i<=TILE311;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE321;i<=TILE335;i++)
  tile_tab[i-TILE000]=1;
for (i=TILE008;i<=TILE013;i++)
  tile_tab[i-TILE000]=2;
for (i=TILE008;i<=TILE013;i++)
  tile_tab[i-TILE000]=2;
for (i=TILE110;i<=TILE013;i++)
  tile_tab[i-TILE000]=2;
for (i=TILE141;i<=TILE143;i++)
  tile_tab[i-TILE000]=2;

// Checking number of levels...
for (i=1;i<=99;i++) {
if (fopen(lev_arq[i-1],"rb")) nof_lev++;
else break;
}

if (nof_lev>99) nof_lev=99;

init_hero();
nlev=1;
player.level=0;

player.level=15;
for (;;) {
  if (nlev) {
    nlev=0;
    player.level++;
    if (player.level>nof_lev) quit(1);
    start_pos();
    load_level(lev_arq[player.level-1]);
    i=player.level;
    while (i>34)
      i-=33;
    play_midi(dat[midi01+i].dat,TRUE);
    draw_scr();
    }

  blit(hs1,hs2,0,0,0,0,SCREEN_W,SCREEN_H);

//  move_water();
  move_hero();
  move_bad();
  chk_item();
  eva_item();

  draw_bad();
  draw_hero();
  display_score();

  blit (hs2,screen,0,0,0,0,SCREEN_W,SCREEN_H);
  rest(GAME_DELAY);
}


readkey();
}
