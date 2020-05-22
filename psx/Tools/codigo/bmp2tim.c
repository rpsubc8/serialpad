/****************************************************************************/
/* Programa para convertir una imagen en un archivo TIM                     */
/* Utilidad: Usar en el compilador psyq-46                                  */
/*           de PSOne en modo crudo o comprimido                            */
/* Autor: Jaime Jose Gavin Sierra                          Alias: JJ        */
/* Se distribuye bajo licencia GPL                                          */
/* Compilar: gcc -s -O6 bmp2tim.c -obmp2tim.exe -lalleg -lkernel32 -luser32 */
/*    -lgdi32 -lcomdlg32 -lole32 -ldinput -lddraw -ldxguid -lwinmm -ldsound */
/* Optimizaciones: Ninguna, es C semiestandar, y no se necesitaba           */
/*                 una velocidad excesiva.                                  */
/****************************************************************************/
#include <allegro.h>
unsigned char cabecera[32]={0x10,0,0,0,0x9,0,0,0,0x0C,02,0,0,0,0x02,0x04,0,0,0x01,0x01,0,0xFF,0x7F,0,0x80,0,0x80,0,0x80,0,0x80,0,0x80};

int main(int argc, char* argv[]) {
 set_uformat(U_ASCII);
 allegro_init();
 set_window_title("BMP Power TIM");
 set_display_switch_mode(SWITCH_BACKAMNESIA);
 allegro_exit();
}
END_OF_MAIN()
