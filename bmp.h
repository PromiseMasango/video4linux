#include <stdint.h>



/*	BITMAP FILE HEADER */
struct Win1xFileHeader{
	int16_t Type;
	int16_t Width;
	int16_t Height;
	int16_t Planes;
	int8_t BitsPerPixel;
};
typedef struct Win1xFileHeader Win1xFileHeader;

struct Win2xFileHeader{
	int16_t type;	// 4D42h = "BM"
	int32_t size;	//size of file in bytes
	int16_t res1;	/* Always 0 */
	int16_t res2;  /* Always 0 */
	int32_t offset; //starting position of image data
};
typedef struct Win2xFileHeader Win2xFileHeader;


//////////////////////////////////////////////////////

/* BITMAP BITMAP HEADER*/
struct Win2xBitmapHeader{
	int32_t Size;
	int16_t Width;
	int16_t Height;
	int16_t Planes;
	int16_t BitsPerPixel;
};
typedef struct Win2xBitmapHeader Win2xBitmapHeader;

struct Win3xBitmapHeader{
	int32_t size; // 40 int8_ts
	int32_t width;
	int32_t height;
	int16_t planes;
	int16_t bpp;
	int32_t comp;
	int32_t imagesize;
	int32_t hres;
	int32_t vres;
	int32_t color;
	int32_t icolor;
};
typedef struct Win3xBitmapHeader Win3xBitmapHeader;


////////////////////////////////////////////////////
	/*BITMAP COLOR TABLE*/


struct Win2xPaletteElement{	//range from 0 to 255
	int8_t Red;
	int8_t Green;
	int8_t Blue;
};
typedef struct Win2xPaletteElement Win2xPaletteElement;


struct Win3xPaletteElement{	//range from 0 to 255
	int8_t Red;
	int8_t Green;
	int8_t Blue;
	int8_t Res; //always 0
};
typedef struct Win3xPaletteElement Win3xPaletteElement;


/////////////////////////////////////////////////////


struct BitmapBitfieldMasks{
	int32_t RedMask;
	int32_t GreenMask;
	int32_t BlueMask;
};
typedef struct BitmapBitfieldMasks BitmapBitfieldMasks;
