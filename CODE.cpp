
/****************************************************************************************
                                    INCLUDES
******************************************************************************************/
#include<allegro5\allegro.h>
#include<allegro5\allegro_native_dialog.h>
#include<allegro5\allegro_image.h>
#include<allegro5\allegro_primitives.h>
#include<allegro5\allegro_audio.h>
#include<allegro5\allegro_acodec.h>
#include<iostream>
using namespace std;
///////////////////////////////////////////////////////////////////////////////////////////




/*******************************************************************************************
Function for Updating Camera
Hepls to dwell in the world which is larger in size than screen
Takes cameraPosition as pointer and modifies its value
Also takes coordinates of player as input
*******************************************************************************************/
void cameraUpdate (float *cameraPosition,float x,float y)
{
	cameraPosition[0]=x-750;
	cameraPosition[1]=0;

	if(cameraPosition[0]<0)
		cameraPosition[0]=0;
}
/////////////////////////////////////////////////////////////////////////////////////////



/*****************************************************************************************
Function to create gravity effect
Takes as input the start and end points of range (x1 & x2),
player opsition and jump-status
Also creates different levels
*****************************************************************************************/
void putGravity (int x1,int x2,int y,int mx,int my, int &jumpspeed,bool &jump)
{
	if (mx>x1 && mx<=x2)
	{
		if (my<800 && my>y+60)
		{
			jumpspeed=jumpspeed+1;
			jump=false;
		}

		else if(my+100<y)
		{
			jumpspeed=jumpspeed+1;
			jump=false;
		}

		else 
		{
			jumpspeed=0;
			jump=true;
		}

	}
}
//////////////////////////////////////////////////////////////////////////////////////////




/****************************************************************************************
This function checks collision with enemy
Takes input - X and Y co-ordinate of player and existance status of enemy
****************************************************************************************/
int ecollision (int mx,int my,int ex,bool die)
{   
	if(!die)
	{
	if ((mx+50>ex)&&(ex+140>mx))
	{
		if(my<700 && my>650 )
			return 2;
		if( my>750)
			return 1;
	}
	
	}
	else
		return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////


/***************************************************************************************
Function to check collision with coins
Inputs - X & Y co-ordinates of player and coin
Existance status of coin
***************************************************************************************/

bool coinCollision (int mx, int my, int cx, int cy, bool coin)
{
	if (!coin)
	{
		if((mx+50>cx)&&(cx+66>mx)&&(my+100>cy)&&(cy+78>my))
			return true;
		else
			return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////


/*************************************************************************************
Function to check collision with wall
This wil cause reversal of direction of motion following laws of collision
*************************************************************************************/
void wallcollision (int mx,int my,int wx,int wy,int &yspeed,bool &coll)
{
	if(mx+50>wx && wx+550>mx && (!coll))
	{
		if(my<wy+60)
			yspeed=-yspeed;
		    coll=true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////



/****************************************************************************************************************
This function puts enemy in range given by x1 & x2 if enemy status is 
existing.
Also draw bitmap image of enemy alongwith.
*****************************************************************************************************************/
void putEnemy (int &ex,int dir, int x1,int x2,bool die, ALLEGRO_BITMAP *enemy1, ALLEGRO_BITMAP *enemy2)
{
	
	if(!die)
	{
	if (ex<x2 && dir==1)
		{
			ex=ex+2;
			if(ex>x2-5)
	    	dir=2;
		}
		else if (ex>x1 && dir==2)
		{
			ex=ex-2;
			if(ex<x1+5)
		    dir=1;
		}
	if(dir==1)
	al_draw_bitmap(enemy1,ex,800,NULL);

	if(dir==2)
	al_draw_bitmap(enemy2,ex,800,NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main()
{
	//DEFINING VARIABLES
	bool done=false;          //Game status (is it finished or still going on)
	bool jump=true;           //Jump status
	int x=100, y=800;         //C-orinates of player
	int i=1;
	float g=1;
	int yspeed=0;             //Jump speed
	float cameraPosition[2]={0,0};      //Camerra variables
	int ex1=1000;             
	int dir=1;                        
	bool die1=false,die2=false,die3=false,die4=false,die5=false,die6=false,die7=false;     //Enemy status
	int ex2=2000,ex3=2900,ex4=4000,ex5=4600,ex6=5000,ex7=1500;                             //Enemy positions
	int decide1=0, decide2=0, decide3=0, decide4=0,decide5=0,decide6=0,decide7=0;          //Decides how dies - mario or enemy
	bool go = false;
	bool won =false;
	int enemy=0;
	bool coin1=false, coin2=false ,coin3=false ,coin4=false ,coin5=false;        //Coin status
	int cx[5]={600,1000,1600,2500,4000};    // Coin positions - X
	int cy[5]={300,500,400,600,500};        // Coin positions - Y
	int coincount=0;
	int j[5]={1,1,1,1,1};
    bool coll = false;

	//CHECKING FOR PROPER INITIALISATON
	if (!al_init()) 
	{
		al_show_native_message_box(NULL,NULL,NULL,"COULD NOT INITIALISE",NULL,NULL);
	}

	
	//SETTING UP DISPLAY
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	ALLEGRO_DISPLAY *display = al_create_display(1500,1200);
	al_set_window_title(display,"SUPER MARIO");
	al_set_window_position(display,0,0);
	if (!display) 
	{
		al_show_native_message_box(NULL,NULL,NULL,"COULD NOT CREATE DISPLAY",NULL,NULL);
	}

    
	//INITIALISING AND INSTALLING SYSTEMS
	al_init_primitives_addon();
	al_install_audio();
	al_init_acodec_addon();
	al_init_image_addon();
	al_install_keyboard();



    //SOUND
	al_reserve_samples(4);
	ALLEGRO_SAMPLE *jumpmusic = al_load_sample("jump.wav");
	ALLEGRO_SAMPLE *crack = al_load_sample("cracker.wav");
	ALLEGRO_SAMPLE *enemydie = al_load_sample("enemydie.wav");
	ALLEGRO_SAMPLE *backmusic = al_load_sample("back.wav");
	ALLEGRO_SAMPLE *end = al_load_sample("mariodie.wav");
	ALLEGRO_SAMPLE *coinColl = al_load_sample("coin.wav");
	ALLEGRO_SAMPLE_INSTANCE *backInstance = al_create_sample_instance(backmusic);
	al_set_sample_instance_playmode(backInstance,ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(backInstance,al_get_default_mixer());
	al_play_sample_instance(backInstance);
	
	
	
	//KEYBOARDSTATE , TIMER AND EVENT QUEUE
	ALLEGRO_KEYBOARD_STATE keystate;
	ALLEGRO_TRANSFORM camera;
	ALLEGRO_TIMER *timer = al_create_timer(1.0/140);
	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	al_register_event_source(event_queue,al_get_keyboard_event_source());
	al_register_event_source(event_queue,al_get_timer_event_source(timer));
	al_start_timer(timer);


	//LOADING IMAGES
	ALLEGRO_BITMAP *mario1 = al_load_bitmap("mario1.png");
	al_convert_mask_to_alpha(mario1,al_map_rgb(0,0,0));
	ALLEGRO_BITMAP *mario2 = al_load_bitmap("mario2.png");
	al_convert_mask_to_alpha(mario2,al_map_rgb(0,0,0));
	ALLEGRO_BITMAP *mario3 = al_load_bitmap("mario3.png");
	al_convert_mask_to_alpha(mario3,al_map_rgb(0,0,0));
	ALLEGRO_BITMAP *mario4 = al_load_bitmap("mario4.png");
	al_convert_mask_to_alpha(mario4,al_map_rgb(0,0,0));
	ALLEGRO_BITMAP *e1 = al_load_bitmap("e1.png");
	al_convert_mask_to_alpha(e1,al_map_rgb(255,242,0));
	ALLEGRO_BITMAP *e2 = al_load_bitmap("e2.png");
	al_convert_mask_to_alpha(e2,al_map_rgb(255,242,0));
	ALLEGRO_BITMAP *e11 = al_load_bitmap("e11.png");
	al_convert_mask_to_alpha(e11,al_map_rgb(255,242,0));
	ALLEGRO_BITMAP *background = al_load_bitmap("background6.png");
	ALLEGRO_BITMAP *wall = al_load_bitmap("wall.png");
	ALLEGRO_BITMAP *castle = al_load_bitmap("castle.png");
	ALLEGRO_BITMAP *castle1 = al_load_bitmap("castle1.png");
	ALLEGRO_BITMAP *castle2 = al_load_bitmap("castle2.png");
	ALLEGRO_BITMAP *start = al_load_bitmap("start.png");
	ALLEGRO_BITMAP *name = al_load_bitmap("name.png");
	al_convert_mask_to_alpha(name,al_map_rgb(255,255,255));
	ALLEGRO_BITMAP *win = al_load_bitmap("win.png");
	ALLEGRO_BITMAP *gameover = al_load_bitmap("gameover.png");
	al_convert_mask_to_alpha(gameover,al_map_rgb(255,255,255));
	ALLEGRO_BITMAP *c0 = al_load_bitmap("0.png");
	al_convert_mask_to_alpha(c0,al_map_rgb(255,255,255));
	ALLEGRO_BITMAP *c1 = al_load_bitmap("1.png");
	al_convert_mask_to_alpha(c1,al_map_rgb(255,255,255));
	ALLEGRO_BITMAP *c2 = al_load_bitmap("2.png");
	al_convert_mask_to_alpha(c2,al_map_rgb(255,255,255));
	ALLEGRO_BITMAP *c3 = al_load_bitmap("3.png");
	al_convert_mask_to_alpha(c3,al_map_rgb(255,255,255));
	ALLEGRO_BITMAP *c4 = al_load_bitmap("4.png");
	al_convert_mask_to_alpha(c4,al_map_rgb(255,255,255));
	ALLEGRO_BITMAP *c5 = al_load_bitmap("5.png");
	al_convert_mask_to_alpha(c5,al_map_rgb(255,255,255));
	ALLEGRO_BITMAP *c6 = al_load_bitmap("6.png");
	al_convert_mask_to_alpha(c6,al_map_rgb(255,255,255));
	ALLEGRO_BITMAP *c7 = al_load_bitmap("7.png");
	al_convert_mask_to_alpha(c7,al_map_rgb(255,255,255));
	ALLEGRO_BITMAP *coin = al_load_bitmap("coin.png");
	al_convert_mask_to_alpha(coin,al_map_rgb(255,255,255));
	ALLEGRO_BITMAP *cracker = al_load_bitmap("cracker.png");
	al_convert_mask_to_alpha(cracker,al_map_rgb(237,28,36));

   
	//Loads first page

	while (!go)
	{
	ALLEGRO_EVENT events;
		al_wait_for_event(event_queue, &events);

		if (events.type==ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (events.keyboard.keycode)
			{
			case ALLEGRO_KEY_ENTER :
						   go=true;
						   break;
			}
		}
	al_draw_bitmap(start,0,0,NULL);
	al_flip_display();
	}
	////////////////////////////////////////










	//GAME LOOP


	while (!done)
	{
		ALLEGRO_EVENT events;
		al_wait_for_event(event_queue, &events);


		/***************************************************************
		Section to handle keyboard events
		***************************************************************/
		if (events.type==ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (events.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE :
						   done=true;
						   break;
			}
		}

		if(events.type==ALLEGRO_EVENT_TIMER)
		{
			al_get_keyboard_state(&keystate);

			if(al_key_down(&keystate,ALLEGRO_KEY_UP) &&jump)
			{
				yspeed=-30;
				al_play_sample(jumpmusic,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
			}

			if(al_key_down(&keystate,ALLEGRO_KEY_RIGHT))
			{
				x+=4;
				if(i<=3)
			        i++;
				else
					i=2;
			}

			if(al_key_down(&keystate,ALLEGRO_KEY_LEFT))
			{
				x-=4;
			    if(i<=3)
				    i++;
				else
					i=2;
			}
		}
		/////////////////////////////////////////////////////////////////////////
	    
		cameraUpdate (cameraPosition, x, y);
		al_identity_transform(&camera);
		al_translate_transform(&camera,-cameraPosition[0],-cameraPosition[1]);
		al_use_transform(&camera);

        y=y+yspeed;


	/*	if(y<800 && ((x>1053 || x<500)))
		{
			yspeed=yspeed+g;
		    jump=false;
		}

		else if (x<1053 && x>500)
		{
			if (y<400)
			{
			    yspeed=yspeed+g;
		        jump=false;
			}

			else
			{
				yspeed=0;
		        jump=true;
			}
		}

		else
		{
			yspeed=0;
			jump=true;
		}
		*/
		

		//Putting gravity fields
		putGravity(0,500,900,x,y,yspeed,jump);
		putGravity(500,1053,500,x,y,yspeed,jump);
		putGravity(1053,7000,900,x,y,yspeed,jump);
		
		

		wallcollision(x,y,500,400,yspeed,coll);
		
		
		//Deciding what to do on colliding with enemy
		if(decide1==1)
			done=true;
		if(decide1==2)
		{
			al_play_sample(enemydie,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
			enemy++;
			die1=true;
		}

		if(decide2==1)
			done=true;
		if(decide2==2)
		{
			al_play_sample(enemydie,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
			enemy++;
			die2=true;
		}

		if(decide3==1)
			done=true;
		if(decide3==2)
		{
			al_play_sample(enemydie,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
			enemy++;
			die3=true;
		}

		if(decide4==1)
			done=true;
		if(decide4==2)
		{
			al_play_sample(enemydie,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
			enemy++;
			die4=true;
		}

		if(decide5==1)
			done=true;
		if(decide5==2)
		{
			al_play_sample(enemydie,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
			enemy++;
			die5=true;
		}

		if(decide6==1)
			done=true;
		if(decide6==2)
		{
			al_play_sample(enemydie,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
			enemy++;
			die6=true;
		}

		if(decide7==1)
			done=true;
		if(decide7==2)
		{
			al_play_sample(enemydie,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
			enemy++;
			die7=true;
		}


		decide1=ecollision(x,y,ex1,die1);
		decide2=ecollision(x,y,ex2,die2);
		decide3=ecollision(x,y,ex3,die3);
		decide4=ecollision(x,y,ex4,die4);
        decide5=ecollision(x,y,ex5,die5);
		decide6=ecollision(x,y,ex6,die6);
		decide7=ecollision(x,y,ex7,die7);




		coin1=coinCollision(x,y,cx[0],cy[0],coin1);
		coin2=coinCollision(x,y,cx[1],cy[1],coin2);
		coin3=coinCollision(x,y,cx[2],cy[2],coin3);
		coin4=coinCollision(x,y,cx[3],cy[3],coin4);
		coin5=coinCollision(x,y,cx[4],cy[4],coin5);


		if( coin1 && j[0]==1)
		{
			al_play_sample(coinColl,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
			coincount++;
			j[0]++;
		}
		if( coin2 && j[1]==1)
		{
			al_play_sample(coinColl,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
			coincount++;
			j[1]++;
		}
		if( coin3 && j[2]==1)
		{
			al_play_sample(coinColl,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
			coincount++;
			j[2]++;
		}
		if( coin4 && j[3]==1)
		{
			al_play_sample(coinColl,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
			coincount++;
			j[3]++;
		}
		if( coin5 && j[4]==1)
		{
			al_play_sample(coinColl,1.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
			coincount++;
			j[4]++;
		}


		



	    	if (ex1<1500 && dir==1)
		{
			ex1=ex1+2;
			if(ex1>1495)
	    	dir=2;
		}
		else if (ex1>1000 && dir==2)
		{
			ex1=ex1-2;
			if(ex1<1005)
		    dir=1;
		}




	    al_draw_bitmap(background,0,0,NULL);
		//al_draw_bitmap(background,1500,0,NULL);
		//al_draw_bitmap(background,3000,0,NULL);
		//al_draw_bitmap(background,4500,0,NULL);
		al_draw_bitmap(name,300,200,NULL);
		al_draw_bitmap(castle,6000,0,NULL);
		al_draw_bitmap(wall,500,500,NULL);



		//Putting enemies and coins
		
		if (enemy == 0)
			al_draw_bitmap(c0,cameraPosition[0]+1300,100,NULL);
		

		if (enemy == 1)
			al_draw_bitmap(c1,cameraPosition[0]+1300,100,NULL);

		if (enemy == 2)
			al_draw_bitmap(c2,cameraPosition[0]+1300,100,NULL);

		if (enemy == 3)
			al_draw_bitmap(c3,cameraPosition[0]+1300,100,NULL);

		if (enemy == 4)
			al_draw_bitmap(c4,cameraPosition[0]+1300,100,NULL);

		if (enemy == 5)
			al_draw_bitmap(c5,cameraPosition[0]+1300,100,NULL);

		if (enemy == 6)
			al_draw_bitmap(c6,cameraPosition[0]+1300,100,NULL);

		if (enemy == 7)
			al_draw_bitmap(c7,cameraPosition[0]+1300,100,NULL);



		if (!coin1)
		{
			al_draw_bitmap(coin,cx[0],cy[0],NULL);
			
		}

		if (!coin2)
		{
			al_draw_bitmap(coin,cx[1],cy[1],NULL);
			
		}

		if (!coin3)
		{
			al_draw_bitmap(coin,cx[2],cy[2],NULL);
			
		}

		if (!coin4)
		{
			al_draw_bitmap(coin,cx[3],cy[3],NULL);
			
		}

		if (!coin5)
		{
			al_draw_bitmap(coin,cx[4],cy[4],NULL);
			
		}


		if (coincount == 0)
			al_draw_bitmap(c0,cameraPosition[0]+300,100,NULL);
		

		if (coincount == 1)
			al_draw_bitmap(c1,cameraPosition[0]+300,100,NULL);

		if (coincount == 2)
			al_draw_bitmap(c2,cameraPosition[0]+300,100,NULL);

		if (coincount == 3)
			al_draw_bitmap(c3,cameraPosition[0]+300,100,NULL);

		if (coincount == 4)
			al_draw_bitmap(c4,cameraPosition[0]+300,100,NULL);

		if (coincount == 5)
			al_draw_bitmap(c5,cameraPosition[0]+300,100,NULL);




		al_draw_bitmap(e11,cameraPosition[0]+1200,100,NULL);
		al_draw_bitmap(coin,cameraPosition[0]+200,100,NULL);


		
		if(!die1)
		al_draw_bitmap(e1,ex1,800,NULL);
	   // putEnemy (ex1,dir,1000,1500,die1,e1);
	    putEnemy (ex2,dir,2000,2600,die2,e1,e2);
		putEnemy (ex3,dir,2900,3400,die3,e1,e2);
		putEnemy (ex4,dir,4000,4500,die4,e1,e2);
		putEnemy (ex5,dir,4600,5200,die5,e1,e2);
		putEnemy (ex6,dir,5000,5600,die6,e1,e2);
		putEnemy (ex7,dir,1500,2200,die7,e1,e2);


	    if (i==1)
		al_draw_bitmap(mario1,x,y,0);
	    if (i==2)
		al_draw_bitmap(mario2,x,y,0);
		if (i==3)
		al_draw_bitmap(mario3,x,y,0);
		if (i==4)
		al_draw_bitmap(mario4,x,y,0);

		if(x>6800){
			al_draw_bitmap(castle1,6000,0,NULL);
	        al_draw_bitmap(mario1,6000,800,NULL);
	        al_flip_display();
	
	        al_rest(1.0);
	        al_draw_bitmap(castle2,6000,0,NULL);
			al_flip_display();
			al_rest(0.5);
			al_play_sample(crack,5.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
            al_draw_bitmap(castle,6000,0,NULL);
	        al_draw_bitmap(cracker,x-850+397,y-900+165,NULL);
			al_draw_bitmap(cracker,x-850+1369,y-900+209,NULL);
			done=true;
			won=true;
		}

		al_flip_display();
		al_clear_to_color(al_map_rgb(0,0,0));
	    
	}

	al_play_sample(end,3.0,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);

	
	

	al_rest (1.8);




	al_destroy_bitmap(mario1);
	al_destroy_bitmap(mario2);
	al_destroy_bitmap(mario3);
	al_destroy_bitmap(mario4);
	al_destroy_bitmap(wall);
	al_destroy_bitmap(name);
	
	al_destroy_bitmap(e1);
	al_destroy_bitmap(e2);
	al_destroy_bitmap(background);
	al_destroy_timer(timer);
	al_destroy_sample(jumpmusic);
	al_destroy_sample(backmusic);
	al_destroy_sample_instance(backInstance);
	al_destroy_sample(enemydie);
	al_destroy_sample(end);


	if(won)
	{
	al_draw_bitmap(win,6000,0,NULL);
	al_flip_display();
	al_rest(4.0);
	}

	else
	{
	al_draw_bitmap(gameover,x-700,500,NULL);
	al_flip_display();
	al_rest(4.0);
	}
	
	al_destroy_bitmap(win);

	return 04;
}