#include "altera_up_avalon_accelerometer_spi.h"
#include "system.h"
#include<stdio.h>
#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sys/alt_stdio.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "altera_avalon_timer_regs.h"
static char tableau_seg7[] = "\x40\x79\x24\x30\x19\x12\x02\x78\x00\x10\x04\x03\x46\x21\x06\x0E";
#include <stdio.h>
#include <sys/unistd.h>
#include "system.h"
#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include "altera_up_avalon_accelerometer_spi.h"


#define SW(x) sw_chaque[x]
#define Presse(x) presse_chaque[x]


#define _1DO 262
#define _1RE 294
#define _1MI 330
#define _1FA 349
#define _1SO 392
#define _1LA 440
#define _1TI 494
#define _DO 523
#define _RE 587
#define _MI 659
#define _FA 698
#define _SO 784
#define _LA 880
#define _TI 988
#define _DO1 1047
#define _RE1 1175
#define _MI1 1319
#define _FA1 1397
#define _SO1 1568
#define _LA1 1760
#define _TI1 1976


/////////////////////////////////////////////////////

#define T 8
#define _0 0
#define _1 T*4
#define _1d T*6
#define _2 T*2
#define _2d T*3
#define _4 T*1
#define _4d T*3/2
#define _8 T*1/2
#define _8d T*3/4
#define _16 T*1/4
#define _16d T*3/8
#define _32 T*1/8

/////////////////////////////////////////////////////

#define _END 100

alt_u32 second=0;
alt_u32 min=0;
alt_u32 hour=0;
alt_u32 min_alarme=0;
alt_u32 hour_alarme=0;
alt_u32 HEX5_HEX4;
alt_u32 HEX3_HEX0;
alt_u32 mode;
alt_u32 nombre_music=0;
alt_u32 temp_music=0;
alt_u32 alarme = 0;

static const alt_u32 seg[11] = {0x40 , 0x79 , 0x24 , 0x30 , 0x19 , 0x12 , 0x02 , 0x78 , 0x0 , 0x10 , 0x7F};


int presse = 0x00;
int presse_chaque[2];
int sw = 0x00;
char sw_chaque[10];
int perio_music=0;
int mesure=0;
int Music_Buf7[]=
{
_DO,_8,_RE,_8,_MI,_8,_FA,_8,_SO,_8,_LA,_8,_TI,_8,_DO,_8,_END,4,
_END,4,
};

int Music_Buf8[]=
{
_MI,_16,_MI,_16,_MI,_4,_MI,_16,_MI,_16,_MI,_4,' ',_32,_MI,_8,_SO,_8,_DO,_8,_RE,_16,' ',_32,
_MI,_4d,_FA,_8,_FA,_8,_FA,_8,_FA,_8,_FA,_8,_MI,_8,_MI,_8,
_MI,_16,_MI,_4,' ',_32,_MI,_4,_RE,_8,_RE,_8,_MI,_8,_RE,_8,_SO,_4,
_END,4,
}; //jingle bells

int Music_Buf9[]=
{
_DO,_16,' ',_32,_DO,_8,_RE,_4,_DO,_4,_FA,_4,_MI,_2,
_DO,_16,' ',_32,_DO,_8,_RE,_4,_DO,_4,_SO,_4,_FA,_2,
_DO,_16,' ',_32,_DO,_8,_DO,_4,_LA,_4,_FA,_8,' ',_32,_FA,_4,
_MI,_4,_RE,_2,
_TI,_8,' ',_32,_TI,_4,_LA,_4,_FA,_4,_SO,_4,_FA,_1,
_END,4,
}; //joyeux anniversaire


#define Transcode_seg7_min_sec(fonction,minute,seconde) fonction =\
	(tableau_seg7[minute/10] << 24) + \
	(tableau_seg7[minute %10] << 16)+\
	(tableau_seg7[seconde /10] << 8 )\
	+tableau_seg7[seconde %10]

#define Transcode_seg7_heure_24(fonction,hour) fonction =\
	 (*(tableau_seg7 +hour /10 )  <<8)+\
	 ((*(tableau_seg7 + hour %10))  )

#define Transcode_seg7_heure_12(fonction,hour) fonction =\
(*(tableau_seg7 + ((hour >12)?((hour - 12 )/10) :(hour/10) ))<<8)+\
tableau_seg7[(hour >12)?((hour - 12)%10):(hour %10 )]

//volatile int edge_capture;
//#define actual_hour(present_hour,present_min,present_sec) present_hour*3600+60*present_min+present_sec
int max_seconds = 60;
int max_minuts  = 60;
int max_hours 	= 24;

//int alarmCount  = 0;
//int nextStateLeds = 0;

	 int * LED_ptr		= (int *)LEDR_BASE;// les LEDs
	 int * SW_switch_ptr		= (int *	)INTERRUPTEURS_BASE;// les switches
	 int * KEY_ptr		= (int *)BOUTONS_POUSSOIRS_BASE;// boutons poussoirs
	 int SW_value; // la variable switch








//void check(int number) {
   // if (number < 10)
     //   printf("0%d", number);// 9 par exemple devient 09
    //}
   // else {
     //   printf("%d", number);
    //}
//}


	 int bruit_var=0x0;
	   int flag_gyro;
	   int full_stop;

int main()
{
	//int present_time;
	//int time_for_alarm;
    int present_hour = 22; // les valeurs données sur les affectations sont des valeurs de test et ne contribuent rien au code
    int present_min = 10;
    int present_sec = 45;
    int alarm_hour = 22;
    int alarm_min = 10;
    int alarm_sec = 50;
    int Seg7_byte1; // variable pour les deux premiers afficheurs
    int Seg7_byte2; // variable pour l'afficheur des heures
    int LED_bits; // variable pour les LEDS ( toute facon y a qu'une seule qui est utilisée mais ca laisse de la place pour des améliorations)
    int index =0; // i
    int press; // variable pour les boutons poussoir
    int flag_continuous; // un flag artificiel dans le code pour désigner le mode continue
   // int alarm=0;
     // un flag pour mettre du bruit de 500hz sur le haut parleur
    int jour = 3;
  //  int j=0;
    int jdex=0;



    int i_gyro=0;
    int xAccel_temp;
    int yAccel_temp;
    int zAccel_temp;
  //  int test;
    	alt_up_accelerometer_spi_dev * acc_dev;

    	acc_dev = alt_up_accelerometer_spi_open_dev ("/dev/accelerometer_spi_0");


    	//if(acc_dev == NULL)
    //	printf("Error: could not open acc device \n");
    //	else
    	//printf ("Opened acc device \n");

    	alt_32 xAccel = 0;
    	alt_32 yAccel = 0;
    	alt_32 zAccel = 0;

    	IOWR_ALTERA_AVALON_TIMER_PERIODL(SYS_CLK_TIMER_2_BASE, 0xBC20);
    	    IOWR_ALTERA_AVALON_TIMER_PERIODH(SYS_CLK_TIMER_2_BASE, 0xBE);
    	    IOWR_ALTERA_AVALON_TIMER_CONTROL(SYS_CLK_TIMER_2_BASE, 0x06);
    	    IOWR_ALTERA_AVALON_TIMER_STATUS(SYS_CLK_TIMER_2_BASE, 0x00);


    	    IOWR_ALTERA_AVALON_TIMER_PERIODL(SYS_CLK_TIMER_BASE, 0x7840);
    	    IOWR_ALTERA_AVALON_TIMER_PERIODH(SYS_CLK_TIMER_BASE, 0x017d);
    	    IOWR_ALTERA_AVALON_TIMER_CONTROL(SYS_CLK_TIMER_BASE, 0x06);
    	    IOWR_ALTERA_AVALON_TIMER_STATUS(SYS_CLK_TIMER_BASE, 0x00);

    	    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_MELODIE_BASE, 0x1100);
    	    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_MELODIE_BASE, 0x0000);
    	    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_MELODIE_BASE, 0x06);
    	    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_MELODIE_BASE, 0x00);



    	while(1){


    		alt_up_accelerometer_spi_read_x_axis(acc_dev, &xAccel);
    		alt_up_accelerometer_spi_read_y_axis(acc_dev, &yAccel);
    		alt_up_accelerometer_spi_read_z_axis(acc_dev, &zAccel);
    	//	printf("%1i %1i %1i\n", xAccel, yAccel, zAccel);
    	//	usleep(100);
    		if(i_gyro==0){
    			xAccel_temp = xAccel;
    			yAccel_temp = yAccel;
    			zAccel_temp = zAccel;
    			i_gyro++;
    		}
    		if((xAccel < xAccel_temp+16 && xAccel > xAccel_temp-16) || (yAccel < yAccel_temp+16 && yAccel > yAccel_temp-16) || (zAccel < zAccel_temp+16 && zAccel > zAccel_temp-16) ){

    			//	bruit_var =0x1;
    			    			flag_gyro =0x001;
    			    			   IOWR_ALTERA_AVALON_PIO_DATA(LED_ptr, LED_bits);
    			    			   full_stop = 0x0;

    			    	//	if(full_stop == 0x1){
    			    		//	bruit_var = 0x0;
    			    		//}


    		}
    		else{



				flag_gyro =0x000;
    		//		full_stop = 0x1;
    						 //  IOWR_ALTERA_AVALON_PIO_DATA(LED_ptr, LED_bits);
    						   bruit_var=0x0;
    					full_stop =0x1;

    		}













    	   IOWR_ALTERA_AVALON_PIO_DATA(LED_ptr, LED_bits);
    	   SW_value = IORD_ALTERA_AVALON_PIO_DATA(SW_switch_ptr);


    	   if (IORD_ALTERA_AVALON_TIMER_STATUS(SYS_CLK_TIMER_2_BASE) == 0x03) //CHECK TIMER STATUS
    	  		       {
    	  		    	   IOWR_ALTERA_AVALON_TIMER_STATUS(SYS_CLK_TIMER_2_BASE,0); // effacer le flag timeout
    	  		    	   index++;
    	  		    	   if(index > 3){
    	  		    	 present_sec++;
    	  		    	 index=0;
    	  		       }

    	  		    	   {jdex++;
    	  		    		   if(flag_continuous == 0x1 && jdex >1){


    	  		    	 press = IORD_ALTERA_AVALON_PIO_DATA(BOUTONS_POUSSOIRS_BASE);
    	  		    	 if(((SW_value & 0x3) == 1) || ((SW_value & 0x3) ==2)){

    	  		    	 			 if(press == 0x1){
    	  		    	 							alarm_min += 1;
    	  		    	 					  }
    	  		    	 					  if(press == 0x2){
    	  		    	 							  alarm_min -= 1;
    	  		    	 					  }
    	  		    	 			  }if(((SW_value & 0x3) == 0) || ((SW_value & 0x3)==3))
    	  		    	 			  {

    	  		    	 				  if(press == 0x1){
    	  		    	 							  present_min += 1;
    	  		    	 				  }
    	  		    	 				  if(press == 0x2){
    	  		    	 							  present_min -= 1;
    	  		    	 				  }
    	  		       }

    	  		    	 			  	  jdex =0;
    	  		       }}
    	  		       }
    	   if (present_hour == alarm_hour && present_min == alarm_min && present_sec == alarm_sec && flag_gyro ==0x001) {
    		   if(jour<5){
    			  // full_stop =0x0;
    		//	   if(flag_gyro ==0x1){
    	            bruit_var = 0x1;
    	          //  flag_gyro=0x1;
    	            full_stop = 0x0;
    		   }}
    	         //  }

    	           else {
    	                   if (present_sec >= max_seconds) {
    	                       present_sec = present_sec - max_seconds;
    	                       present_min++;
    	                   }
    	                   if (present_sec < 0) {
    	                        present_sec = present_sec + max_seconds;
    	                       	     present_min--;
    	                       	                   }
    	                   if (present_min == max_minuts) {
    	                       present_min = 0;
    	                       present_hour++;
    	                   }
    	                   if (present_min < 0) {
    	                       present_min = present_min + max_minuts;
    	                       present_hour--;
    	                      	                   }
    	                   if (present_hour == max_hours) {
    	                       present_hour = 0;
    	                       jour++;
    	                   }
    	                   if (present_hour < 0) {
    	                	   present_hour = present_hour + max_hours;
    	                	   jour--;
    	                   }
    	                   if (jour == 7) {
    	                	   jour = 0;
    	                   }


    	                     }
    	  /* j++;
    	   if(j > 12500 ){ //50.000 pour 1s puisqu'on travail avec l'horloge de 1us et là on génére 0.25s
    		   switch(jour){
    		   case 0 :
    	   printf("lundi");
    	   printf("\n");
    	   break;
    		   case 1 :
    			   printf("mardi");
    		   printf("\n");
    		   break;
    		   case 2 :
    			   printf("mercredi");
    		   printf("\n");
    		   break;
    		   case 3 :
    			   printf("jeudi");
    		   printf("\n");
    		   break;
    		   case 4 :
    			   printf("vendredi");
    		   printf("\n");
    		   break;
    		   case 5 :
    			   printf("samedi");
    		   printf("\n");

    		   break;
    		   case 6 :
    			   printf("dimanche");
    			   printf("\n");

    		   break;}



    		   j=0;
    	   }*/
    	   switch(SW_value & 0x3){
    	       	                         			  case 0://affichige actuel en format 24 heures
    	       	                             					Transcode_seg7_min_sec(Seg7_byte1,present_min, present_sec);
    	       	                             					Transcode_seg7_heure_24(Seg7_byte2,present_hour);

    	       	                             					break;
    	       	                             				  case 1://affichige alarme en format 24 heures
    	       	                             				  	Transcode_seg7_min_sec(Seg7_byte1,alarm_min, alarm_sec);
    	       	                             				  	Transcode_seg7_heure_24(Seg7_byte2,alarm_hour);

    	       	                             				  	break;
    	       	                             				  case 2://affichige alarme en format 12 heures
    	       	                             				  	Transcode_seg7_min_sec(Seg7_byte1,alarm_min, alarm_sec);
    	       	                             				  	Transcode_seg7_heure_12(Seg7_byte2,alarm_hour);

    	       	                             				  	break;
    	       	                             				  case 3://affichige actuel en format 12 heures
    	       	                             					Transcode_seg7_min_sec(Seg7_byte1,present_min, present_sec);
    	       	                             					Transcode_seg7_heure_12(Seg7_byte2,present_hour);

    	       	                             					break;

    	       	                             }
    	 //  if(flag_gyro ==0x1){
    		//   LED_bits = 0x0;
    	   //}
    	   if(flag_gyro == 0x001){
    		if(SW_value & 0x4){
    		  		  LED_bits = 0x4;


    		  	}else{
    		  		  LED_bits = 0x00;
    		  		  bruit_var = 0x0;

    		  	}}
    	   else {
    		   LED_bits = 0x00;
    		 //    bruit_var = 0x0;
    		//   IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_MELODIE_BASE,ALTERA_AVALON_TIMER_CONTROL_STOP_MSK);

    	   }
    		switch(SW_value & 0x18){
    		  			case 0:
    		  					flag_continuous = 0x1;



    		  			break;

    		  			case 0x10:

    		  				if(((SW_value & 0x3) == 1) || ((SW_value & 0x3) ==2)){
    		  					press = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_ptr);
    		  						IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_ptr,press);
    		  							if(press == 0x1){
    		  							alarm_sec += 10;
    		  																}
    		  									if(press == 0x2){
    		  									alarm_sec -= 10;

    		  																				}}
    		  				if(((SW_value & 0x3) == 0) || ((SW_value & 0x3)==3)){
    		  					{
    		  					    		  					press = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_ptr);
    		  					    		  						IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_ptr,press);
    		  					    		  							if(press == 0x1){
    		  					    		  							present_sec += 10;
    		  					    		  																}
    		  					    		  									if(press == 0x2){
    		  					    		  									present_sec -= 10;

    		  					    		  																				}}

    		  				}


    		  			break;
    		  	}
    		if(SW_value & 0x18){ //reglage continue marche que dans 0x00 et les autres cas sont annulés
    		    		  		flag_continuous = 0x00;
    		    		  				  	}




    	   	   	   	   IOWR_ALTERA_AVALON_PIO_DATA(HEX3_HEX0_BASE, Seg7_byte1);
    	   		  	  IOWR_ALTERA_AVALON_PIO_DATA(HEX5_HEX4_BASE, Seg7_byte2);


		sw = IORD_ALTERA_AVALON_PIO_DATA(INTERRUPTEURS_BASE);



				for(int i = 0; i <= 9; i++ )
		{
			SW(i) = sw>>i & 0x01;
		}





				int melodie=0;
				//IOWR_ALTERA_AVALON_PIO_DATA(LEDR_BASE, 0x01);
				if (bruit_var==0x1){
			//if(flag_gyro==0x1){



				if(hour_alarme == hour && min_alarme == min)
				{


					if(SW(2) == 0x01){
					if (SW(7)== 0x01)
						melodie=7;
					else if(SW(8) == 0x01)
						melodie=8;
					else if(SW(9)== 0x01)
						melodie=9;
					else
						melodie=0;

					//if(bruit_var==0x1){
					if(flag_gyro ==0x1){
					sonner(melodie);
					//}
					}

					}
					else
								{
									IOWR_ALTERA_AVALON_PIO_DATA(LEDR_BASE, 0x00);
									nombre_music = 0;
									//bruit_var =0x00;
								}

				}
				else
				{
					nombre_music = 0;
				}

			//	else{
				//	  bruit_var = 0x0;



				//}




				}

		if(SW(3) == 0x00)
			{

				if(SW(4) == 0x01)
				{


				}
				else
				{
					presse = IORD_ALTERA_AVALON_PIO_DATA(BOUTONS_POUSSOIRS_BASE);
						if(IORD_ALTERA_AVALON_TIMER_STATUS(SYS_CLK_TIMER_BASE) == 0x03)
						{
							IOWR_ALTERA_AVALON_TIMER_STATUS(SYS_CLK_TIMER_BASE, 0);
							IOWR_ALTERA_AVALON_TIMER_PERIODL(SYS_CLK_TIMER_BASE, 0x7840);
							IOWR_ALTERA_AVALON_TIMER_PERIODH(SYS_CLK_TIMER_BASE, 0x017d);
							IOWR_ALTERA_AVALON_TIMER_CONTROL(SYS_CLK_TIMER_BASE, 0x06);
							IOWR_ALTERA_AVALON_TIMER_STATUS(SYS_CLK_TIMER_BASE, 0x00);
							Presse(0) = presse>>0 & 0x01;
							Presse(1) = presse>>1 & 0x01;

						}
					}
			}



	}



  return 0;
}


void sonner(int melodie)
{


	int perio_music=0;
	int mesure=0;
	if(flag_gyro ==0x000 || full_stop == 0x1){
										    			melodie = 0;
										    		}
	else{
	switch(melodie){
	case 7 : 	{
					perio_music = Music_Buf7[2*nombre_music];
					mesure = Music_Buf7[2*nombre_music+1];
					break;
				}
	case 8 : 	{
					perio_music = Music_Buf8[2*nombre_music];
					mesure = Music_Buf8[2*nombre_music+1];
					break;
				}
	case 9 : 	{
					perio_music = Music_Buf9[2*nombre_music];
					mesure = Music_Buf9[2*nombre_music+1];
					break;
				}
	case 0 : 	{
					IOWR_ALTERA_AVALON_PIO_DATA(LEDR_BASE, 0x00);
					break;
				}
	default: return;
	}
	}
	if(perio_music == _END)
	{
		nombre_music = 0;
		return;
	}
	else
	{
		if(IORD_ALTERA_AVALON_TIMER_STATUS(TIMER_MELODIE_BASE) == 0x03)
		{
			IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_MELODIE_BASE, 0);
			alarme =!alarme;
			IOWR_ALTERA_AVALON_PIO_DATA(GPIO_10_BASE, alarme);
			IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_MELODIE_BASE, ((50000000/perio_music)&0xffff));
			IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_MELODIE_BASE, (((50000000/perio_music)>>16)&0xffff));
			IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_MELODIE_BASE, 0x06);
			IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_MELODIE_BASE, 0x00);

		}
		if(IORD_ALTERA_AVALON_TIMER_STATUS(SYS_CLK_TIMER_BASE) == 0x03)
		{
			IOWR_ALTERA_AVALON_TIMER_STATUS(SYS_CLK_TIMER_BASE, 0);
			IOWR_ALTERA_AVALON_TIMER_PERIODL(SYS_CLK_TIMER_BASE, 0xaf08);
			IOWR_ALTERA_AVALON_TIMER_PERIODH(SYS_CLK_TIMER_BASE, 0x002f);
			IOWR_ALTERA_AVALON_TIMER_CONTROL(SYS_CLK_TIMER_BASE, 0x06);
			IOWR_ALTERA_AVALON_TIMER_STATUS(SYS_CLK_TIMER_BASE, 0x00);
			temp_music ++;

		}

		if(temp_music >= mesure)
		{
			temp_music = 0;
			nombre_music ++;
		}



	}



return;}











void display(alt_u32 hour, alt_u32 min, alt_u32 second)
{
	HEX5_HEX4 = (seg[hour/10]<<8 | seg[hour%10]);
	if(mode == 1)
    {
		HEX3_HEX0 = (seg[min/10]<<24 | seg[min%10]<<16 | seg[10]<<8 | seg[10]);
    }
    else
    {
    	HEX3_HEX0 = (seg[min/10]<<24 | seg[min%10]<<16 | seg[second/10]<<8 | seg[second%10]);
    }
  //  IOWR_ALTERA_AVALON_PIO_DATA(HEX3_HEX0_BASE,HEX3_HEX0);
    //IOWR_ALTERA_AVALON_PIO_DATA(HEX5_HEX4_BASE,HEX5_HEX4);
return;}




   // while (1) {
    //	 present_sec++;

            //check(present_hour);

           // check(present_min);

         //   check(present_sec);


//    }


