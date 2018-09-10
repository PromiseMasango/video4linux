
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "bmp.h"

/*BITMAP FILE HEADER	version 2x*/

static void writeHeader(struct Win2xFileHeader *h ,FILE *file ){
	fwrite( &h->type , sizeof(int16_t) , 1 , file );
	fwrite( &h->size , sizeof(int32_t), 1 , file );
	fwrite( &h->res1 , sizeof(int16_t) , 1 , file );
	fwrite( &h->res2 , sizeof(int16_t) , 1 , file );
	fwrite( &h->offset,sizeof(int32_t), 1 , file );
}

static void readHeader(struct Win2xFileHeader *h , FILE *file ){
	fread( &h->type , sizeof(int16_t) , 1 , file );
	fread( &h->size , sizeof(int32_t), 1 , file );
	fread( &h->res1 , sizeof(int16_t) , 1 , file );
	fread( &h->res2 , sizeof(int16_t) , 1 , file );
	fread( &h->offset,sizeof(int32_t), 1 , file );
}

static void dumpHeader( struct Win2xFileHeader *h ){
	printf("****BITMAP FILE HEADER****\n");
	printf("Type :%s\n" , (char *) &h->type );
	printf("Size :%d\n" , h->size );
	printf("offset :%d\n", h->offset );

}
/////////////////////////////////////////////////////////////

/*BITMAP BITMAP INFO*/

static void writeInfo(struct Win3xBitmapHeader *i , FILE *file ){
	fwrite( &i->size , sizeof(int32_t ) , 1 , file );
	fwrite( &i->width, sizeof( int32_t ) , 1 , file );
	fwrite( &i->height,sizeof( int32_t ) , 1 , file );
	fwrite( &i->planes,sizeof( int16_t ) , 1 , file );
	fwrite( &i->bpp   ,sizeof( int16_t ) , 1 , file );
	fwrite( &i->comp  ,sizeof( int32_t) , 1 , file );
	fwrite( &i->imagesize, sizeof(int32_t), 1 ,file);
	fwrite( &i->hres  ,sizeof( int32_t ) , 1 , file );
	fwrite( &i->vres  ,sizeof( int32_t ) , 1 , file );
	fwrite( &i->color ,sizeof( int32_t) , 1 , file );
	fwrite( &i->icolor,sizeof( int32_t) , 1 , file );
}


static void readInfo(struct Win3xBitmapHeader *i , FILE *file ){
	fread( &i->size , sizeof(int32_t ) , 1 , file );
	fread( &i->width, sizeof( int32_t ) , 1 , file );
	fread( &i->height,sizeof( int32_t ) , 1 , file );
	fread( &i->planes,sizeof( int16_t ) , 1 , file );
	fread( &i->bpp   ,sizeof( int16_t ) , 1 , file );
	fread( &i->comp  ,sizeof( int32_t) , 1 , file );
	fread( &i->imagesize, sizeof(int32_t), 1 ,file);
	fread( &i->hres  ,sizeof( int32_t ) , 1 , file );
	fread( &i->vres  ,sizeof( int32_t ) , 1 , file );
	fread( &i->color ,sizeof( int32_t) , 1 , file );
	fread( &i->icolor,sizeof( int32_t) , 1 , file );
}

static void dumpInfo( struct Win3xBitmapHeader *i){
	printf("****BITMAP BITMAP INFO****\n");
	printf("width :%d \theight :%d\n", i->width , i->height );
	printf("planes :%d \t bpp :%d \t compression :%d \t imagesize :%d\n", i->planes , i->bpp , i->comp , i->imagesize );
}

/*Excellent*/
/*Convert to grayscale using mean value*/
static void rgb2gray(int offset ,FILE *inFile, FILE *outFile, int row, int column ){
	/*SEEK IMAGE DATA*/
	fseek( inFile , 0 , offset );

	/*RED , GREEN , BLUE*/
	unsigned char red,green,blue;

	/*GRAY MEAN*/
	int mean;
	for( int r = 0 ; r < row ; r++){
		for( int c = 0 ; c < column ; c++ ){

				fread( &blue , 1, 1 , inFile );
				fread( &green , 1, 1 , inFile );
				fread( &red , 1, 1 , inFile );

				/*Calculate rgb gray mean value*/
				mean = (blue + red + green ) / 3;

				/*Write pixel to new Image*/
				fwrite( &mean , 1, 1, outFile );			
				fwrite( &mean , 1, 1, outFile );
				fwrite( &mean , 1, 1, outFile );
		}
	}
}

/*Not tested*/
/*Maximum water penetration - wavelength [0.45-0.52]*/

static void visibleBlue(int offset ,FILE *inFile, FILE *outFile, int row, int column ,float value){
	/*Validata value*/
	if( (value < 0.44 ) || ( value > 0.52 ) ){
		printf("Visible Blue [ 0.45-0.52]\n");
		exit(1);
	}

	/*SEEK IMAGE DATA*/
	fseek( inFile , 0 , offset );

	/*RED , GREEN , BLUE*/
	unsigned char red,green,blue;

	for( int r = 0 ; r < row ; r++){
		for( int c = 0 ; c < column ; c++ ){

				fread( &red , 1, 1 , inFile );
				fread( &green , 1, 1 , inFile );
				fread( &blue , 1, 1 , inFile );

				/*Calculate rgb channel values*/
				red = red * value;
				green = green * value;
				blue = blue * value;

				/*Write pixel to new Image*/
				fwrite( &blue , 1, 1, outFile );			
				fwrite( &green , 1, 1, outFile );
				fwrite( &red , 1, 1, outFile );
		}
	}
}

/*Not tested*/
/*Measure plant vigor - wavelength [0.52-0.60]*/

static void visibleGreen(int offset ,FILE *inFile, FILE *outFile, int row, int column ,float value){
	/*Validata value*/
	if( (value < 0.51 ) || ( value > 0.60 ) ){
		printf("Visible Green [ 0.52-0.60]\n");
		exit(1);
	}

	/*SEEK IMAGE DATA*/
	fseek( inFile , 0 , offset );

	/*RED , GREEN , BLUE*/
	unsigned char red,green,blue;

	for( int r = 0 ; r < row ; r++){
		for( int c = 0 ; c < column ; c++ ){

				fread( &red , 1, 1 , inFile );
				fread( &green , 1, 1 , inFile );
				fread( &blue , 1, 1 , inFile );

				/*Calculate rgb channel values*/
				red = red * value;
				green = green * value;
				blue = blue * value;

				/*Write pixel to new Image*/
				fwrite( &blue , 1, 1, outFile );			
				fwrite( &green , 1, 1, outFile );
				fwrite( &red , 1, 1, outFile );
		}
	}
}

/*Not tested*/
/*Vegetation discrimination - wavelength[ 0.63-0.69]*/

static void visibleRed(int offset ,FILE *inFile, FILE *outFile, int row, int column ,float value){
	/*Validata value*/
	if( (value < 0.62 ) || ( value > 0.69 ) ){
		printf("Visible Red [ 0.63-0.69]\n");
		exit(1);
	}

	/*SEEK IMAGE DATA*/
	fseek( inFile , 0 , offset );

	/*RED , GREEN , BLUE*/
	unsigned char red,green,blue;

	for( int r = 0 ; r < row ; r++){
		for( int c = 0 ; c < column ; c++ ){

				fread( &red , 1, 1 , inFile );
				fread( &green , 1, 1 , inFile );
				fread( &blue , 1, 1 , inFile );

				/*Calculate rgb channel values*/
				red = red * value;
				green = green * value;
				blue = blue * value;

				/*Write pixel to new Image*/
				fwrite( &blue , 1, 1, outFile );			
				fwrite( &green , 1, 1, outFile );
				fwrite( &red , 1, 1, outFile );
		}
	}
}
/*Not tested*/
/*Biomass and shoreline mapping - wavelength[0.76-0.90]*/
static void NearInfrared(int offset ,FILE *inFile, FILE *outFile, int row, int column ,float value){
	/*Validata value*/
	if( (value < 0.76 ) || ( value > 0.90 ) ){
		printf("Near Infrared [ 0.76-0.90]\n");
		exit(1);
	}

	/*SEEK IMAGE DATA*/
	fseek( inFile , 0 , offset );

	/*RED , GREEN , BLUE*/
	unsigned char red,green,blue;

	for( int r = 0 ; r < row ; r++){
		for( int c = 0 ; c < column ; c++ ){

				fread( &red , 1, 1 , inFile );
				fread( &green , 1, 1 , inFile );
				fread( &blue , 1, 1 , inFile );

				/*Calculate rgb channel values*/
				red = red * value;
				green = green * value;
				blue = blue * value;

				/*Write pixel to new Image*/
				fwrite( &blue , 1, 1, outFile );			
				fwrite( &green , 1, 1, outFile );
				fwrite( &red , 1, 1, outFile );
		}
	}
}


/*Moisture content of soil and vegetation - wavelength [1.55-1.75*/
static void MiddleInfrared(int offset ,FILE *inFile, FILE *outFile, int row, int column ,float value){
	/*Validata value*/
	if( (value < 1.55 ) || ( value > 1.75 ) ){
		printf("Middle Infrared [ 1.55-1.75]\n");
		exit(1);
	}

	/*SEEK IMAGE DATA*/
	fseek( inFile , 0 , offset );

	/*RED , GREEN , BLUE*/
	unsigned char red,green,blue;

	for( int r = 0 ; r < row ; r++){
		for( int c = 0 ; c < column ; c++ ){

				fread( &red , 1, 1 , inFile );
				fread( &green , 1, 1 , inFile );
				fread( &blue , 1, 1 , inFile );

				/*Calculate rgb channel values*/
				red = red * value;
				green = green * value;
				blue = blue * value;

				/*Write pixel to new Image*/
				fwrite( &blue , 1, 1, outFile );			
				fwrite( &green , 1, 1, outFile );
				fwrite( &red , 1, 1, outFile );
		}
	}
}

/*Not tested*/
/*Soil moisture ;thermal mapping - wavelength [10.4-12.5]*/

static void ThermalInfrared(int offset ,FILE *inFile, FILE *outFile, int row, int column ,float value){
	/*Validata  value*/
	if( (value < 10.4 ) || ( value > 12.5 ) ){
		printf("Thermal Infrared [ 10.4-12.5]\n");
		exit(1);
	}

	/*SEEK IMAGE DATA*/
	fseek( inFile , 0 , offset );

	/*RED , GREEN , BLUE*/
	unsigned char red,green,blue;

	for( int r = 0 ; r < row ; r++){
		for( int c = 0 ; c < column ; c++ ){

				fread( &red , 1, 1 , inFile );
				fread( &green , 1, 1 , inFile );
				fread( &blue , 1, 1 , inFile );

				/*Calculate rgb channel values*/
				red = red * value;
				green = green * value;
				blue = blue * value;

				/*Write pixel to new Image*/
				fwrite( &blue , 1, 1, outFile );			
				fwrite( &green , 1, 1, outFile );
				fwrite( &red , 1, 1, outFile );
		}
	}
}

/*Not tested*/
/*Mineral mapping - wavelength [ 2.08 - 2.35]*/

static void MMiddleInfrared(int offset ,FILE *inFile, FILE *outFile, int row, int column ,float value){
	/*Validata value*/
	if( (value < 2.08 ) || ( value > 2.35 ) ){
		printf("MMiddle Infrared [ 1.55-1.75]\n");
		exit(1);
	}

	/*SEEK IMAGE DATA*/
	fseek( inFile , 0 , offset );

	/*RED , GREEN , BLUE*/
	unsigned char red,green,blue;

	for( int r = 0 ; r < row ; r++){
		for( int c = 0 ; c < column ; c++ ){

				fread( &red , 1, 1 , inFile );
				fread( &green , 1, 1 , inFile );
				fread( &blue , 1, 1 , inFile );

				/*Calculate rgb channel values*/
				red = red * value;
				green = green * value;
				blue = blue * value;

				/*Write pixel to new Image*/
				fwrite( &blue , 1, 1, outFile );			
				fwrite( &green , 1, 1, outFile );
				fwrite( &red , 1, 1, outFile );
		}
	}
}


/*DEFINATELY WORKS*/
/*Increase brightness by adding bright value to each pixel channel*/
static void	brightness(int offset , FILE *input ,FILE *output , int bright,int row,int column ){
	/*SEEK IMAGE DATA*/
	fseek( input , 0 , offset );

	/*RED , GREEN , BLUE*/
	unsigned char red,green,blue;

	for( int r = 0 ; r < row ; r++){
		for( int c = 0 ; c < column ; c++ ){

				fread( &blue , 1, 1 , input );
				fread( &green , 1, 1 , input );
				fread( &red , 1, 1 , input );

				/*Increase brightness of pixel by the value of bright*/
				blue = (blue * bright) / 255;
				green= (green* bright)  / 255;
				red  = (red  * bright) / 255;
				

				fwrite( &blue , 1, 1, output );			
				fwrite( &green , 1, 1, output );
				fwrite( &red, 1, 1, output );
		}
	}
}


/*DEFINATELY WORKS*/
static void	resize(int offset , FILE *input ,FILE *output , int rsize,int row,int column ){
	/*SEEK IMAGE DATA*/
	fseek( input , 0 , offset );

	/*RED , GREEN , BLUE*/
	unsigned char red,green,blue;
	unsigned char color[row][column];

	for( int r = 0 ; r < row ; r++){
			for( int c = 0 ; c < column ; c++ ){

				fread( &blue , 1, 1 , input );
				fread( &green , 1, 1 , input );
				fread( &red , 1, 1 , input );

				color[r][c] = red;
				color[r][c] = green;
				color[r][c] = blue;
		}
	}
	fwrite( color , sizeof(color) , 1 , output );
}


/*Not tested*/
static void GammaCorrection(int offset , FILE *input ,FILE *output , double y,int row,int column ){
	/*SEEK IMAGE DATA*/
	fseek( input , 0 , offset );

	/*RED , GREEN , BLUE*/
	unsigned char red,green,blue;

	for( int r = 0 ; r < row ; r++){
		for(int c = 0 ; c < column ; c++ ){		
			fread( &blue , 1, 1 , input );
			fread( &green , 1, 1 , input );
			fread( &red , 1, 1 , input );

			blue = (int) blue * y;
			green= (int) green* y;
			red  = (int) red * y;

			fwrite( &blue , 1, 1, output );			
			fwrite( &green , 1, 1, output );
			fwrite( &red, 1, 1, output );
		}
	}
}

/*Not tested*/

static void restore(int offset , FILE *input ,FILE *output , int row,int column ){
	/*SEEK IMAGE DATA*/
	fseek( input , 0 , offset );

	/*RED , GREEN , BLUE*/
	unsigned char red,green,blue;

	for( int r = 0 ; r < row ; r++){
		for( int c = 0 ; c < column ; c++ ){

				fread( &blue , 1, 1 , input );
				fread( &green , 1, 1 , input );
				fread( &red , 1, 1 , input );

				blue = blue * 0.45;
				green= green * 0.60;
				red  = red * 0.63;


				fwrite( &blue , 1, 1, output );			
				fwrite( &green , 1, 1, output );
				fwrite( &red, 1, 1, output );


		}
	}
}

/*Not tested*/
/*Switch row with colomn to rotate clockwise*/
			
static void rotate(int offset ,FILE *input , FILE *output, int row, int column){
	/*SEEK IMAGE DATA*/
	fseek( input , 0 , offset );

	struct Win2xPaletteElement image[row][column];
	struct Win2xPaletteElement Newimage[row][column];

	unsigned char red,green,blue;


	for( int h = 0 ; h < row ; h++){
			for( int w = 0 ; w < column; w++ ){

				fread( &blue , 1, 1 , input );
				fread( &green , 1, 1 , input );
				fread( &red , 1, 1 , input );

				image[h][w].Red = blue;
				image[h][w].Green= green;
				image[h][w].Blue = red;

			}
	}

	//Reflect
//	reflect:
	int index = column ;
	for(int i = 0 ; i < row ; i++ ){
		for(int j = 0 ; j < column ; j++ )
			Newimage[i][index-j] = image[i][j];
	}
	fwrite( Newimage , sizeof(Newimage) , 1, output );



/*	flip:
	int index = row ;
	for(int i = 0 ; i < row ; i++ ){
		for(int j = 0 ; j < column ; j++ )
			Newimage[row-i][j] = image[i][j];
	}
	fwrite( Newimage , sizeof(Newimage) , 1, output );
*/
}

/*			
static void rotate(int offset ,FILE *input , FILE *output, int row, int column ){
	/*SEEK IMAGE DATA* /
	fseek( input , 0 , offset );

	struct Win2xPaletteElement image[row][column];
	struct Win2xPaletteElement Newimage[row][column];

	unsigned char red,green,blue;

	for( int h = 0 ; h < row ; h++){
			for( int w = 0 ; w < column; w++ ){

				fread( &blue , 1, 1 , input );
				fread( &green , 1, 1 , input );
				fread( &red , 1, 1 , input );

				image[h][w].Red = blue;
				image[h][w].Green= green;
				image[h][w].Blue = red;

			}
	}

	for(int i = 0 ; i < row ; i++ ){
		for(int j = 0 ; j < column ; j++ )
			Newimage[j][i] = image[i][j];
	}
	fwrite( Newimage , sizeof(Newimage) , 1, output );
}
*/

int main(){
	/*Image Data */
	FILE *file = fopen("first.bmp" , "rb" );
	FILE *second = fopen("second.bmp", "wb" );
	struct Win2xFileHeader *head;
	struct Win3xBitmapHeader   *info;

	//memory allocation
	head = malloc( sizeof(*head) );
	info = malloc( sizeof(*info) );

	//bitmap file header
	readHeader( head , file );
	dumpHeader( head );


	//bitmap bitmap information
	readInfo(info , file );
	dumpInfo( info );


	/*Write Newly created Image*/
	//info->width = 320 * 2;
//	info->height= 240 * 2;

	writeHeader(head , second );
	writeInfo( info , second );


	//seek pixel data offset
//	fseek(file , 0,head->offset );

	//file size
	struct stat metadata;
	int result = stat( "first.bmp" , &metadata );
	if( result ){
		printf("cannot get metadata\n");
		exit(EXIT_FAILURE );
	}

//	rotate(head->offset,file,second,abs(info->height),info->width);
	//rgb2gray(head->offset,file ,second,abs(info->height),info->width);
//static void rotate(int offset ,FILE *input , FILE *output, int row, int column){
	rotate(head->offset,file ,second,abs(info->height),info->width);
//rgb2gray(int offset ,FILE *inFile, FILE *outFile, int row, int column ){

	fclose(file);
	fclose(second);

}

