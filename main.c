#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

//VARIÁVEIS IMPORTANTES
typedef struct setas setas;
#define FPS 5
#define SCREEN_W 700
#define SCREEN_H 600
#define KEYBOARDTIME 0.001

struct setas{
    int x,y,tipo,visivel;
    /* tipo 0 = esquerda, tipo 1 = cima, tipo 2 = baixo, tipo 3 = direita */
};

//declaracoes
void destroiComponentesCriados();
int keyboard();
void telaFim();
void iniciaSeta(setas* s);
void pintar();
void pintar_setas();
int createmusicafundo();
void acerto();
void telaInicio();


//Variaveis
FILE *hs ;
int antigo;
int limite5pt=20,limite3pt=40,limite1pt=60;
int numSetas=500;
int XE = 20,YE = 20;
int XC = 120,YC = 20;
int XB = 220,YB = 20;
int XD = 320,YD = 20;
int XP = 550,YP = 50;
int UY0=SCREEN_H,UY1=SCREEN_H,UY2=SCREEN_H,UY3=SCREEN_H,UYL=SCREEN_H;
int taxa = 840; // taxa a mais de y que vem
int pont=0;
int velocidadeSeta=1;
int setasPassaram=0;
int opc = 1;//musica
int opcD=15;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *imagemd  = NULL;
ALLEGRO_BITMAP *imagemc  = NULL;
ALLEGRO_BITMAP *imagemb  = NULL;
ALLEGRO_BITMAP *imageme  = NULL;
ALLEGRO_BITMAP *imagemds  = NULL;
ALLEGRO_BITMAP *imagemcs  = NULL;
ALLEGRO_BITMAP *imagembs  = NULL;
ALLEGRO_BITMAP *imagemes  = NULL;
ALLEGRO_BITMAP *fundo = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_EVENT_QUEUE *queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_FONT *fonte =NULL;
ALLEGRO_FONT *fontep =NULL;
int running;

ALLEGRO_SAMPLE *musicafundo = NULL;
ALLEGRO_SAMPLE_INSTANCE *musicafundoinstance =NULL;

ALLEGRO_SAMPLE *somacerto= NULL;
ALLEGRO_SAMPLE *somfim= NULL;


//setas listaSetas[1000];
setas** listaSetas;




void atualiza(){

    for(int i=0;i<numSetas;i++){
        //printf("%d\n",listaSetas[i]->y);
        if(listaSetas[i]->y<velocidadeSeta){
            listaSetas[i]->y=-1;
        }else{
            listaSetas[i]->y-=velocidadeSeta;
        }
    }
}
void pintar(){
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_bitmap(fundo,-50,-50,0);
    al_draw_bitmap(imageme,XE,YE,0);
    al_draw_bitmap(imagemc,XC,YC,0);
    al_draw_bitmap(imagemb,XB,YB,0);
    al_draw_bitmap(imagemd,XD,YD,0);
    al_draw_textf(fontep, al_map_rgb(255, 255, 255), XP, YP, ALLEGRO_ALIGN_CENTRE, "PONTUAÇÃO   %d", pont);
    al_draw_textf(fontep, al_map_rgb(255, 255, 255), XP, YP+100, ALLEGRO_ALIGN_CENTRE, "RESTANTES   %d", numSetas-setasPassaram);
    al_draw_textf(fontep, al_map_rgb(255, 255, 255), XP, YP+200, ALLEGRO_ALIGN_CENTRE, "HIGHSCORE   %d", antigo);
    pintar_setas();
    al_flip_display();
}
void pintar_setas(){
    for(int i=0;i<numSetas;i++){
        //printf("%d\n",listaSetas[i].y);
        if(listaSetas[i]->y<=0 && listaSetas[i]->visivel==1){
            setasPassaram++;
            listaSetas[i]->visivel=0;
        }
        if(listaSetas[i]->y>0 && listaSetas[i]->y<=SCREEN_H && listaSetas[i]->visivel==1){
            if(listaSetas[i]->tipo==0){
                al_draw_bitmap(imagemes,listaSetas[i]->x,listaSetas[i]->y,0);
            }
            if(listaSetas[i]->tipo==1){
                al_draw_bitmap(imagemcs,listaSetas[i]->x,listaSetas[i]->y,0);
            }
            if(listaSetas[i]->tipo==2){
                al_draw_bitmap(imagembs,listaSetas[i]->x,listaSetas[i]->y,0);
            }
            if(listaSetas[i]->tipo==3){
                al_draw_bitmap(imagemds,listaSetas[i]->x,listaSetas[i]->y,0);
            }
            }
        }
    }
void mudarSetasDeCor(){
    if(opcD==0){//fácil
         imagemes = al_load_bitmap("images/setaeAMARELA.png");
        imagemcs = al_load_bitmap("images/setacAZUL.png");
        imagembs = al_load_bitmap("images/setabVERDE.png");
        imagemds = al_load_bitmap("images/setadVERMELHA.png");
    }else{
        if(opcD==1){//dificil
             imagemes = al_load_bitmap("images/setaeBRANCA.png");
            imagemcs = al_load_bitmap("images/setacBRANCA.png");
            imagembs = al_load_bitmap("images/setabBRANCA.png");
            imagemds = al_load_bitmap("images/setadBRANCA.png");
        }
    }
}
void run(){
    mudarSetasDeCor();
    hs = fopen("high.txt","r+");
    fscanf(hs,"%d",&antigo);
    for(int i=0;i<numSetas;i++){
        iniciaSeta(*(listaSetas+i));
        //printf("%d\n",listaSetas[i].y);

    }
    while(1){
        if(setasPassaram>=numSetas){
            telaFim();
        }else{
            int atual = keyboard();
            verifica(atual);
            pintar();
            atualiza();
            //printf("%d\n",setasPassaram);
            al_rest(1/FPS);
        }

    }
}
void verifica(int a){
    for(int i=0;i<numSetas;i++){
        if(listaSetas[i]->tipo==(a-1) && listaSetas[i]->visivel==1){ // keyboard me retorna de 1 a 4 e os tipos são de 0 a 3 port. usei (a-1)
            if(a==1){
                if(listaSetas[i]->y-YE<limite5pt){
                    acerto();
                    pont=pont+5;
                    listaSetas[i]->visivel=0;
                    setasPassaram++;
                }else{
                    if(listaSetas[i]->y-YE<limite3pt){
                        acerto();
                        pont=pont+3;
                        listaSetas[i]->visivel=0;
                        setasPassaram++;
                        }else{
                            if(listaSetas[i]->y-YE<limite1pt){
                                acerto();
                                pont=pont+1;
                                listaSetas[i]->visivel=0;
                                setasPassaram++;
                            }
                        }
                }
            }
            if(a==2){
                if(listaSetas[i]->y-YC<limite5pt){
                    acerto();
                    pont=pont+5;
                    listaSetas[i]->visivel=0;
                    setasPassaram++;
                }else{
                    if(listaSetas[i]->y-YC<limite3pt){
                        acerto();
                        pont=pont+3;
                        listaSetas[i]->visivel=0;
                        setasPassaram++;
                        }else{
                            if(listaSetas[i]->y-YC<limite1pt){
                                acerto();
                                pont=pont+1;
                                listaSetas[i]->visivel=0;
                                setasPassaram++;
                            }
                        }
                    }
            }
            if(a==3){
                if(listaSetas[i]->y-YB<limite5pt){
                    acerto();
                    pont=pont+5;
                    listaSetas[i]->visivel=0;
                    setasPassaram++;
                }else{
                    if(listaSetas[i]->y-YB<limite3pt){
                        acerto();
                        pont=pont+3;
                        listaSetas[i]->visivel=0;
                        setasPassaram++;
                        }else{
                            if(listaSetas[i]->y-YB<limite1pt){
                                acerto();
                                pont=pont+1;
                                listaSetas[i]->visivel=0;
                                setasPassaram++;
                            }
                        }
                    }
            }
            if(a==4){
                if(listaSetas[i]->y-YD<limite5pt){
                    acerto();
                    pont=pont+5;
                    listaSetas[i]->visivel=0;
                    setasPassaram++;
                }else{
                    if(listaSetas[i]->y-YD<limite3pt){
                        acerto();
                        pont=pont+3;
                        listaSetas[i]->visivel=0;
                        setasPassaram++;
                        }else{
                            if(listaSetas[i]->y-YD<limite1pt){
                                acerto();
                                pont=pont+1;
                                listaSetas[i]->visivel=0;
                                setasPassaram++;
                            }
                        }
                    }
            }
        }
    }
}

void init(){
    if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    timer = al_create_timer(1.0/FPS);//timer criado
    al_install_keyboard();//teclado criado
    display = al_create_display(SCREEN_W,SCREEN_H);//janela criada
    event_queue = al_create_event_queue();//fila de eventos
    //sound
    al_install_audio();
    al_init_acodec_addon();
    al_init_image_addon();

    somacerto = al_load_sample("sound/acerto.wav");
    somfim = al_load_sample("sound/fim.wav");
    al_reserve_samples(2);


    al_set_window_title(display, "Dancer C");
    event_queue = al_create_event_queue();
    if (!event_queue){
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        //al_destroy_display(janela);
    }
    fonte = al_load_font("fonts/CANDICE.ttf", 48, 0);
    fontep = al_load_font("fonts/BEBAS.otf", 30, 0);
    if (!fonte || !fontep){
        fprintf(stderr, "Falha ao carregar fonte.\n");
        return -1;
    }
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));


   if(!display) {
      fprintf(stderr, "failed to create display!\n");
      return -1;
   }
   if (!al_init_ttf_addon()){
        fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
        return -1;
    }
    //Imagens das setas
    fundo = al_load_bitmap("images/fundo.jpg");
    imageme = al_load_bitmap("images/setaeAMARELA.png");
    imagemc = al_load_bitmap("images/setacAZUL.png");
    imagemb = al_load_bitmap("images/setabVERDE.png");
    imagemd = al_load_bitmap("images/setadVERMELHA.png");
//    imagemes = al_load_bitmap("images/setaeBRANCA.png");
//    imagemcs = al_load_bitmap("images/setacBRANCA.png");
//    imagembs = al_load_bitmap("images/setabBRANCA.png");
//    imagemds = al_load_bitmap("images/setadBRANCA.png");
    imagemes = al_load_bitmap("images/setaeAMARELA.png");
    imagemcs = al_load_bitmap("images/setacAZUL.png");
    imagembs = al_load_bitmap("images/setabVERDE.png");
    imagemds = al_load_bitmap("images/setadVERMELHA.png");


    //malloc na lista de setas
    listaSetas = malloc(numSetas*sizeof(setas*));
    for(int i=0;i<numSetas;i++){
        *(listaSetas+i) = malloc(sizeof(setas));
    }



   //al_clear_to_color(al_map_rgb(255,255,50)); amarelo bonito


   //pintar();
    //al_rest(1.0);

    telaInicio();
   //run();
}
int main(int argc, char **argv){
    init();








   return 0;
}
//Destroi tudo
void destroiComponentesCriados(){
    al_destroy_display(display);
    al_uninstall_keyboard();
    al_destroy_timer(timer);
    al_destroy_bitmap(imageme);
    al_destroy_bitmap(imagemb);
    al_destroy_bitmap(imagemc);
    al_destroy_bitmap(imagemd);
    al_destroy_bitmap(imagemes);
    al_destroy_bitmap(imagembs);
    al_destroy_bitmap(imagemcs);
    al_destroy_bitmap(imagemds);


}
//som acerto
void acerto(){
    al_play_sample(somacerto,1.0,0.0,2.0,ALLEGRO_PLAYMODE_ONCE,0);

}
//Tela end game
void telaFim(){

   al_clear_to_color(al_map_rgb(255,255,255));
    al_draw_text(fonte, al_map_rgb(0, 0, 255), SCREEN_W / 2, 90, ALLEGRO_ALIGN_CENTRE, "Fim de Jogo");
    al_draw_textf(fontep, al_map_rgb(0, 0, 255), SCREEN_W / 2, 300, ALLEGRO_ALIGN_CENTRE, "Pontuação Final    %d",pont);

    if(pont>antigo){
        al_draw_textf(fontep, al_map_rgb(0, 0, 255), SCREEN_W / 2, 400, ALLEGRO_ALIGN_CENTRE, "NOVA HIGHSCORE",pont);
        rewind(hs);
        fprintf(hs,"%d ",pont);
    }

   al_flip_display();
   //al_play_sample(somfim,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
   while(keyboard()!=0){

   }
   destroiComponentesCriados();
}
void criarMusicaSelecionada(){
    if(!createmusicafundo()){
        printf("musica erro");
    }
    al_play_sample_instance(musicafundoinstance);
}
//tela inicio
void telaInicio(){
   al_clear_to_color(al_map_rgb(255,255,255));
    al_draw_text(fonte, al_map_rgb(191, 28, 61), SCREEN_W / 2, 90, ALLEGRO_ALIGN_CENTRE, "Dancer - C");
    al_draw_textf(fontep, al_map_rgb(0, 0, 0), SCREEN_W / 2, 200, ALLEGRO_ALIGN_CENTRE, "Escolha com as setas",pont);
    al_draw_textf(fontep, al_map_rgb(0, 0, 0), SCREEN_W / 2, 300, ALLEGRO_ALIGN_CENTRE, "<- musica 1",pont);
    al_draw_textf(fontep, al_map_rgb(0, 0, 0), SCREEN_W / 2, 350, ALLEGRO_ALIGN_CENTRE, "-> musica 2",pont);
    //al_draw_textf(fontep, al_map_rgb(0, 0, 0), SCREEN_W / 2, 500, ALLEGRO_ALIGN_CENTRE, "HIGHSCORE %d",antigo);
    if(opcD==0){
        al_draw_textf(fontep, al_map_rgb(0, 0, 0), SCREEN_W / 2, 400, ALLEGRO_ALIGN_CENTRE, "(cima) dificil",pont);
        al_draw_textf(fontep, al_map_rgb(255, 0, 0), SCREEN_W / 2, 450, ALLEGRO_ALIGN_CENTRE, "(baixo) fácil",pont);
    }
    if(opcD==1){
        al_draw_textf(fontep, al_map_rgb(255, 0, 0), SCREEN_W / 2, 400, ALLEGRO_ALIGN_CENTRE, "(cima) dificil",pont);
        al_draw_textf(fontep, al_map_rgb(0, 0, 0), SCREEN_W / 2, 450, ALLEGRO_ALIGN_CENTRE, "(baixo) fácil",pont);
    }
    if(opcD!=0 && opcD!=1){
        al_draw_textf(fontep, al_map_rgb(0, 0, 0), SCREEN_W / 2, 400, ALLEGRO_ALIGN_CENTRE, "(cima) dificil",pont);
        al_draw_textf(fontep, al_map_rgb(0, 0, 0), SCREEN_W / 2, 450, ALLEGRO_ALIGN_CENTRE, "(baixo) fácil",pont);
    }


   al_flip_display();
   int k = 9;
   while(k!=0){
        k = keyboard();
        if(k==1 && (opcD==0 || opcD==1)){
            opc=1;
            criarMusicaSelecionada();
            pintar();
            run();
        }
        if(k==4 && (opcD==0 || opcD==1)){
            opc=2;
            criarMusicaSelecionada();
            pintar();
            run();
        }
        if(k==2){
            numSetas=100;
            taxa=300;
            opcD=1;
            telaInicio();
        }
        if(k==3){
            numSetas=30;
            taxa=840;
            opcD=0;
            telaInicio();
        }
   }
   destroiComponentesCriados();
}

//coiceito de YL last
void iniciaSeta(setas* s){
    int c = rand()%4;
    s->tipo=c;
    s->visivel=1;
    switch(c){
        case 0:
            //printf("esquerda\n");
            s->x=20;
            s->y=UYL+limite1pt+(rand()%taxa);
            UYL = s->y;
            break;
        case 1:
            //printf("cima\n");
            s->x=120;
            s->y=UYL+limite1pt+(rand()%taxa);
            UYL = s->y;
            break;
        case 2:
            //printf("baixo\n");
            s->x=220;
            s->y=UYL+limite1pt+(rand()%taxa);
            UYL = s->y;
            break;
        case 3:
            //printf("direita\n");
            s->x=320;
            s->y=UYL+limite1pt+(rand()%taxa);
            UYL = s->y;
            //printf("%d\n",s.y);
            break;
        }
}
int keyboard(){
    int tecla;
    while(!al_is_event_queue_empty(event_queue)){
            ALLEGRO_EVENT evento;
            al_wait_for_event(event_queue, &evento);
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
                switch(evento.keyboard.keycode){
                case ALLEGRO_KEY_UP:
                    //printf("cima");
                    tecla = 2;
                    return tecla;
                    break;
                case ALLEGRO_KEY_DOWN:
                    //printf("baixo");
                    tecla = 3;
                    return tecla;
                    break;
                case ALLEGRO_KEY_LEFT:
                    //printf("esquerda");
                    tecla = 1;
                    return tecla;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    //printf("direita");
                    tecla = 4;
                    return tecla;
                    break;
                }
            }
            else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                //printf("Apertou X\n");
                return 0;
            }
        }
}
int createmusicafundo(){
    al_reserve_samples(1);
    if(opc == 1){
        musicafundo = al_load_sample("sound/fundo.wav");
    }
    if(opc ==2){
        musicafundo = al_load_sample("sound/fundo2.wav");
    }
    if(!musicafundo){
        printf("arquivo invalido");
    }
    musicafundoinstance = al_create_sample_instance(musicafundo);
    al_set_sample_instance_gain(musicafundoinstance,0.5);
    //som loop
    al_set_sample_instance_playmode(musicafundoinstance,ALLEGRO_PLAYMODE_LOOP);
    //arquivo ao mixer
    al_attach_sample_instance_to_mixer(musicafundoinstance,al_get_default_mixer());
    return 1;
}

