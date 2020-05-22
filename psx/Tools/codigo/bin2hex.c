/*********************************************************************/
/* Programa para convertir un binario a un ARRAY en C en hexadecimal */
/* Utilidad: Usar .RAW .TIM .HIT y .TMD en el compilador psyq-46     */
/*           de PSOne en modo crudo o comprimido                     */
/* Autor: Jaime Jose Gavin Sierra                          Alias: JJ */
/* Permite comprimir .TIM y .HIT                                     */
/* Se distribuye bajo licencia GPL                                   */
/* Compilar: gcc -s -O6 bin2hex.c -obin2hex.exe                      */
/* Optimizaciones: Ninguna, es C semiestandar, y no se necesitaba    */
/*                 una velocidad excesiva.                           */
/*********************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

 #define boolean unsigned char
 #define byte unsigned char
 
 
 enum tipoCodificacion {sin_codificacion,tim_rle4, tim_rle8, tim_rle4_2,
      tim_rle8_2,tim_rle4_3,tim_rle8_3,tim_rle4_4,tim_rle8_4,bin_rle8,
      bin_rle8_2,bin_rle8_3,bin_rle8_4};
 
 union valorDato{
  unsigned char decimal;
  struct{
   boolean bit8:1,bit7:1,bit6:1,bit5:1,bit4:1,bit3:1,bit2:1,bit1:1;
  }bin;
  struct{
   unsigned int hexadecimal2:4,hexadecimal1:4;
  }hex;
 };

 int tamFichero= -1;
 int tamCodificado= -1;
 int cont=0;
 int tamanioAux= 0;
 int tope_foto= 0;
 int numeroAux=0;
 byte *bufferFichero= NULL;
 byte *bufferFotoComprimida= NULL;
 byte *punteroBuffer= NULL;
 FILE *ficheroSalida= NULL;
 union valorDato octeto;
 enum tipoCodificacion codificacionAux;

 int Tamanio(char *cadFichero);
 int LeerRAW(char *cadFichero,byte *buffer,int tam);
 void MostrarAyuda(void);
 void MostrarError(char *cadena);
 int ComprimeBuffer(byte *bufferEntrada,byte *bufferSalida,int tam,enum tipoCodificacion tipo);


 /**********************/
 /* Programa principal */
 /**********************/
 int main(int argc,char **argv){
  
  if (argc>4){
   tamFichero = Tamanio(argv[1]);
   if (tamFichero<0) MostrarError("No se puede abrir el fichero de entrada");
   bufferFichero = (byte *)malloc(tamFichero);
   if (bufferFichero!=NULL){
    LeerRAW(argv[1],bufferFichero,tamFichero);
    printf ("Fichero de entrada leido\n");
    ficheroSalida= fopen (argv[2],"w");
    if (ficheroSalida!= NULL){
     numeroAux= atoi(argv[4]);
     if (numeroAux>12) numeroAux= 12;
     switch (numeroAux){
      case 0: tamCodificado= tamFichero; break;
      case 1: codificacionAux= tim_rle4; break;
      case 2: codificacionAux= tim_rle8; break;
      case 3: codificacionAux= tim_rle4_2; break;
      case 4: codificacionAux= tim_rle8_2; break;
      case 5: codificacionAux= tim_rle4_3; break;
      case 6: codificacionAux= tim_rle8_3; break;
      case 7: codificacionAux= tim_rle4_4; break;
      case 8: codificacionAux= tim_rle8_4; break;
      case 9: codificacionAux= bin_rle8; break;           
      case 10: codificacionAux= bin_rle8_2; break;
      case 11: codificacionAux= bin_rle8_3; break;
      case 12: codificacionAux= bin_rle8_4; break;
     };
     if (codificacionAux!= sin_codificacion){
      tope_foto= 2*tamFichero;
      bufferFotoComprimida= (byte*)malloc(tope_foto);
      if (bufferFotoComprimida==NULL)
       MostrarError("No hay memoria suficiente para el buffer Codificado"); 
      tamCodificado= ComprimeBuffer(bufferFichero,bufferFotoComprimida,tamFichero,codificacionAux);
     }
     fprintf (ficheroSalida,"enum TCodificacion {sin_codificacion,tim_rle4,tim_rle8,tim_rle4_2,tim_rle8_2,tim_rle4_3,tim_rle8_3,tim_rle4_4,tim_rle8_4,bin_rle8,bin_rle8_2,bin_rle8_3,bin_rle8_4};\n\n"); 
     fprintf (ficheroSalida,"struct TBinarioHex{\n");
     fprintf (ficheroSalida," unsigned int tamBuffer;\n unsigned int tamCodificado;\n");
     fprintf (ficheroSalida," enum TCodificacion tipoCodificacion;\n");
     fprintf (ficheroSalida," unsigned char *buffer;\n};\n\n");     
     if (codificacionAux!= sin_codificacion){
      punteroBuffer= bufferFotoComprimida;
      printf ("Tam RLE %d\n",tamCodificado);
      tamanioAux= tamCodificado;
     }
     else{
      punteroBuffer= bufferFichero;
      printf ("Tam RAW %d\n",tamFichero);
      tamanioAux= tamFichero;
     } 
     cont= 0;
     
     fprintf (ficheroSalida,"unsigned char %s_array[%d]={\n ",argv[3],tamCodificado);
     while(cont<tamanioAux){
      octeto.decimal= punteroBuffer[cont];
      if (octeto.hex.hexadecimal1>9) fprintf(ficheroSalida,"0x%c",octeto.hex.hexadecimal1+55);
      else fprintf(ficheroSalida,"0x%d",octeto.hex.hexadecimal1);
      if (octeto.hex.hexadecimal2>9) fprintf(ficheroSalida,"%c",octeto.hex.hexadecimal2+55);
      else fprintf(ficheroSalida,"%d",octeto.hex.hexadecimal2);
      if (cont<(tamanioAux-1)){
       fprintf(ficheroSalida,",");
       if ((cont%100==0)&&(cont!=0)) fprintf(ficheroSalida,"\n ");
      } 
      cont++;
     }

     fprintf (ficheroSalida,"\n};\n\nstruct TBinarioHex %s= {%d,%d,",argv[3],tamFichero,tamCodificado);
     switch (codificacionAux){
      case sin_codificacion: fprintf(ficheroSalida,"sin_codificacion,");break;
      case tim_rle4: fprintf(ficheroSalida,"tim_rle4,");break;
      case tim_rle8: fprintf(ficheroSalida,"tim_rle8,");break;
      case tim_rle4_2: fprintf(ficheroSalida,"tim_rle4_2,");break;
      case tim_rle8_2: fprintf(ficheroSalida,"tim_rle8_2,");break;
      case tim_rle4_3: fprintf(ficheroSalida,"tim_rle4_3,");break;
      case tim_rle8_3: fprintf(ficheroSalida,"tim_rle8_3,");break;
      case tim_rle4_4: fprintf(ficheroSalida,"tim_rle4_4,");break;
      case tim_rle8_4: fprintf(ficheroSalida,"tim_rle8_4,");break;
      case bin_rle8: fprintf(ficheroSalida,"bin_rle8,");break;
      case bin_rle8_2: fprintf(ficheroSalida,"bin_rle8_2,");break;
      case bin_rle8_3: fprintf(ficheroSalida,"bin_rle8_3,");break;
      case bin_rle8_4: fprintf(ficheroSalida,"bin_rle8_4,");break;
     };
     fprintf (ficheroSalida,"%s_array};",argv[3]); 
     
     punteroBuffer= NULL;
     fclose(ficheroSalida);
     printf ("Fichero de salida grabado");
    }
    else printf ("Error en el fichero de salida\n");
    
    if (bufferFotoComprimida!= NULL){
     free(bufferFotoComprimida); bufferFotoComprimida= NULL;
    } 
    free(bufferFichero);
    bufferFichero=NULL;
   }
   else MostrarError("No hay memoria para cargar el fichero de entrada");
  } 
  else{
   MostrarAyuda();
  }
  
  return (0);
 }


/*************************************************/
int Tamanio(char *cadFichero){
//Saca el tamanio del archivo
 FILE *fich; int tam=0;
 fich = fopen(cadFichero,"rb"); 
 if (fich!=NULL){ fseek(fich,0,SEEK_END); tam = ftell(fich); fclose(fich); }
 else tam= -1;
 return (tam);
}

/********************************/
int LeerRAW(char *cadFichero,byte *buffer,int tam){
 //Lee el archivo en formato crudo
 int fichero;
 fichero = open(cadFichero,O_RDONLY|O_BINARY);
 if (fichero!=0){ read(fichero,buffer,tam); close(fichero); return (0);}
 return (-1);
}

/************************************/
void MostrarAyuda(void){
 //Muestra la ayuda
 printf ("\nConversor BIN 2 ARRAY C  PSOne by JJ\n");
 printf ("---------------------------------------\n\n");
 printf (" bin2h entrada.tim salida.h nombreARRAY [0 1 2 3 4 5 6 7 8]\n");
 printf ("\n Formato\n  0 - Sin formato\n  1 - RLE TIM4\n  2 - RLE TIM8\n  3 - RLE TIM4 2\n  4 - RLE TIM8 2\n  5 - RLE TIM4 3\n  6 - RLE TIM8 3\n  7 - RLE TIM4 4\n  8 - RLE TIM8 4\n  9 - RLE BIN8\n 10 - RLE BIN8 2\n 11 - RLE BIN8 3\n 12 - RLE BIN8 4\n 13 - TIM en fichero\n\n");
 printf (" bin2h entrada.tmd salida.h nombreARRAY 0\n\n");
}

/***********************************************************/
void MostrarError(char *cadena){
 //Muestra el error y sale del programa principal
 printf ("\n%s\n",cadena);
 exit(-1);
}

/*******************************************************************/
int ComprimeBuffer(byte *bufferEntrada,byte *bufferSalida,int tam,enum tipoCodificacion tipo){
//Comprime en formato RLE un buffer devolviendo otro.
//Se le tiene que pasar el tamanio del buffer de entrada
//Como salida devuelve el buffer comprimido y el tamanio del buffer
//Si hay error devuelve -1
 byte aux=0,aux2=0;
 int contColorRepetido=0;    
 int contSalida;
 int contador=0;
 int tamRLE=-1;
 int auxCont= 0;
 int maxColorRepetido=0, baseColor= 0;
 tamRLE= 0;
 contSalida= 0;
 switch (tipo){
  case sin_codificacion: auxCont= 0; maxColorRepetido=0; break;
  case tim_rle4: if (tam<0x220) MostrarError("El archivo no es una imagen TIM 4.Es diminuto.");
                 auxCont= 0x40; maxColorRepetido=64; baseColor=192; break;
  case tim_rle8: if (tam<0x220) MostrarError("El archivo no es una imagen TIM 8.Es diminuto.");
                 auxCont= 0x220; maxColorRepetido= 64; baseColor=192; break;
  case tim_rle4_2: if (tam<0x40) MostrarError("El archivo no es una imagen TIM 4.Es diminuto.");
                   auxCont= 0x40; maxColorRepetido=32; baseColor= 224; break;
  case tim_rle8_2: if (tam<0x220) MostrarError("El archivo no es una imagen TIM 8.Es diminuto.");
                   auxCont= 0x220; maxColorRepetido=32; baseColor= 224; break;
  case tim_rle4_3: if (tam<0x40) MostrarError("El archivo no es una imagen TIM 4.Es diminuto.");
                   auxCont= 0x40; maxColorRepetido=16; baseColor= 240; break;
  case tim_rle8_3: if (tam<0x220) MostrarError("El archivo no es una imagen TIM 8.Es diminuto.");
                   auxCont= 0x220; maxColorRepetido=16; baseColor= 240; break;
  case tim_rle4_4: if (tam<0x40) MostrarError("El archivo no es una imagen TIM 4.Es diminuto.");
                   auxCont= 0x40; maxColorRepetido=8; baseColor= 248; break;
  case tim_rle8_4: if (tam<0x220) MostrarError("El archivo no es una imagen TIM 8.Es diminuto.");
                   auxCont= 0x220; maxColorRepetido=8; baseColor= 248; break;                                      
  case bin_rle8: auxCont= 0; maxColorRepetido= 64; baseColor= 192; break;
  case bin_rle8_2: auxCont= 0; maxColorRepetido= 32; baseColor=224; break;
  case bin_rle8_3: auxCont= 0; maxColorRepetido= 16; baseColor= 240; break;
  case bin_rle8_4: auxCont= 0; maxColorRepetido= 8; baseColor= 248; break;
 } 
 for (contador=0;contador<auxCont;contador++)
  bufferSalida[contador]= bufferEntrada[contador];
 tamRLE= auxCont;
 contador= auxCont;
 contSalida= auxCont;
 while (contador< tam){
  aux= bufferEntrada[contador];
  aux2= bufferEntrada[contador+1];
  contColorRepetido= 0;
  if (aux == aux2){
   while ((aux == aux2)&&(contador<tam)&&(contColorRepetido<(maxColorRepetido))){
    contador++;
    aux2= bufferEntrada[contador];
    contColorRepetido++;
   }
   //printf (" R%d ",contColorRepetido);
   bufferSalida[contSalida]= (baseColor-1+contColorRepetido);
   bufferSalida[contSalida+1]= aux;
   contSalida+=2;
   tamRLE+=2;
  }
  else{
   if (aux<baseColor){
    bufferSalida[contSalida]= aux;
    contSalida++;
    tamRLE++;
   }
   else{
    bufferSalida[contSalida]= baseColor;
    bufferSalida[contSalida+1]= aux;
    tamRLE+=2;
    contSalida+=2;
   }
   contador++;
  }
  if (tamRLE>tope_foto) tamRLE= tope_foto;
 }
 return (tamRLE);
}
