#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <linux/videodev2.h>
#include <stdint.h>
#include <time.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include "bmp.h"

struct bmpHeader{
	int16_t type;	// 4D42h 
	int32_t size;
	int16_t res1;	/* Always 0 */
	int16_t res2;  /* Always 0 */
	int32_t offset; //starting position of image data
};

struct Win1xHeader{
	int16_t Type;
	int16_t Width;
	int16_t Height;
	int16_t Size;
	int16_t Planes;
	int8_t BitsPerPixel;
};



struct palette{	//range from 0 to 255
	int8_t Blue;
	int8_t Green;
	int8_t Red;
//	int8_t Res; //always 0
};

struct bmpInfo{
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

static void writeHeader(struct bmpHeader *h ,FILE *file ){
	fwrite( &h->type , sizeof(int16_t) , 1 , file );
	fwrite( &h->size , sizeof(int32_t), 1 , file );
	fwrite( &h->res1 , sizeof(int16_t) , 1 , file );
	fwrite( &h->res2 , sizeof(int16_t) , 1 , file );
	fwrite( &h->offset,sizeof(int32_t), 1 , file );
}

static void writeInfo(struct bmpInfo *i , FILE *file ){
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

struct buffer{
	void *start;
	ssize_t length;
};



static int fd;
static FILE *file;
static struct buffer *buffers;
static unsigned int n_buffers;
static unsigned int times ;

/*Xlib Image constants
static XImage *image;
static Display *display;
static Window window;
static int screen;
static GC gc;
*/

static void die( char *err ){
	printf("%s\n" , err );
	exit( EXIT_FAILURE );
}
/*
static void Vision(){

	display = XOpenDisplay("");
	screen  = DefaultScreen(display);
	window  = XCreateSimpleWindow(display,RootWindow(display,screen),5,5,500,500,1,BlackPixel(display,screen),WhitePixel(display,screen));
	gc = XDefaultGC(display,screen);
	XSelectInput( display , window , ExposureMask | ButtonPressMask | KeyPressMask);

/*	//set window name
	XTextProperty win_property;
	char *win_name = "First.bmp";
	int rc = XStringListToTextProperty(&win_name,1,&win_property);
	if( rc == 0 )
		printf("Cannot set window name\n");
	else{
		XSetWMName(display,window,&win_property );
		XSetWMIconName(display,window,&win_property);
	}

	XMapWindow( display , window );
	XFlush( display );
	XSync( display , 0 );

	sleep( 4 );
	XUnmapWindow(display,window);
	
	/*Resize* /
	XResizeWindow( display , window , 600, 600 );

	XMapWindow( display , window );
	XFlush( display );
	XSync( display , 0 );

	sleep( 4 );

	/*Move* /
	XMoveResizeWindow( display, window ,200,200,600 ,600 );

	XMapWindow( display , window );
	XFlush( display );
	XSync( display , 0 );

	sleep( 4 );

	/*Window stack* /
	XLowerWindow(display,window);

	XMapWindow( display , window );
	XFlush( display );
	XSync( display , 0 );

	sleep( 4 );
	XUnmapWindow(display,window);	
	
	XRaiseWindow(display,window);

	XMapWindow( display , window );
	XFlush( display );
	XSync( display , 0 );

	sleep( 4 );
	XUnmapWindow(display,window);	


	/*Iconify * /
	XIconifyWindow(display,window,DefaultScreen(display));
	sleep(4);

	XMapWindow( display , window );
	XFlush( display );
	XSync( display , 0 );

	sleep(4);


	Visual *visual = DefaultVisual(display,DefaultScreen(display));
	Colormap colormap = XCreateColormap(display,window,visual,AllocNone);

	//colors
	XColor red,blue,exact;
	int status = XAllocNamedColor(display,colormap,"red",&red,&exact);

	if( status == 0 )
		printf("color red is not supported\n");

	status = XAllocNamedColor(display,colormap,"blue",&blue,&exact);
	if( status == 0 )
		printf("color blue is not supported\n");


	//window background
//	XSetWindowBackground(display,window, blue.pixel );

	XMapWindow( display , window );
	XFlush( display );
	XSync( display , 0 );


	//draw
	while(1){
		XSetForeground(display,gc,blue.pixel);
		XDrawRectangle(display,window,gc,10,10,150,300);

		XSetForeground(display,gc,red.pixel);
		XDrawRectangle(display,window,gc,160,10,150,300);
	}
* /

}
*/

static void open_dev(){
	time_t t = time(NULL);
	char *filename = ctime(&t);

	strcat(filename,".bmp");

	fd = open("/dev/video0" , O_RDWR | O_NONBLOCK , 0 );
	file = fopen( filename , "wb" );
	if( (fd == -1 ) || ( file == NULL ))
		die("cannot open or create file");

	struct bmpHeader *head;
	struct bmpInfo	 *info;

	head = malloc( sizeof ( *head ));
	head->type = 0x4D42;
	head->size = 0;
	head->res1 = 0;
	head->res2 = 0;
	head->offset= 54;

	info = malloc( sizeof(*info ));
	info->size = 40;
	info->width= 640 ;
	info->height=480 ;
	info->planes=1 ;
	info->bpp = 24;
	info->comp  =0 ;
	info->imagesize= 0;
	info->hres = 0;
	info->vres = 0;
	info->color = 0;
	info->icolor= 0;

	head->offset = sizeof(*head) + sizeof(*info);

	writeHeader( head , file );
	writeInfo( info , file );

}

static void stop_capture(){
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl( fd , VIDIOC_STREAMOFF , &type );
}

static void uninit_dev(){
	unsigned int i;
	for( i = 0 ; i < n_buffers ; i++ )
		munmap( buffers[i].start , buffers[i].length );
}

static void handler( int signo ){
	stop_capture();
	uninit_dev();
	exit( EXIT_FAILURE );
}



static void start_capture(){
	unsigned int i;
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	for( i = 0 ; i < n_buffers ; i++ ){
		struct v4l2_buffer buf;

		memset( &buf , 0 , sizeof( buf ));

		buf.type   = type;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = i;

		if( ioctl( fd , VIDIOC_QBUF , &buf ) < 0 )
			die( "cannot queue buffers" );
	}
	ioctl( fd , VIDIOC_STREAMON , &type );
	sleep(1);
}


static void init_mmap(){
	struct v4l2_requestbuffers reqbuf;

	memset( &reqbuf , 0 , sizeof( reqbuf ));

	reqbuf.count = 4;
	reqbuf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory= V4L2_MEMORY_MMAP;

	if( ioctl( fd , VIDIOC_REQBUFS , &reqbuf ) < 0 )
		die("cannot query buffers");

	if( reqbuf.count < 2 )
		die("insufficient memory\n");

	buffers = calloc( reqbuf.count , sizeof( *buffers ));

	for( n_buffers = 0 ; n_buffers < reqbuf.count; ++n_buffers){
		struct v4l2_buffer buff;

		memset( &buff , 0 , sizeof( buff ));

		buff.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buff.memory= V4L2_MEMORY_MMAP;
		buff.index = n_buffers;

		if( ioctl( fd , VIDIOC_QUERYBUF , &buff ) == -1 )
			die("cannot query buffers");


		buffers[n_buffers].start = mmap(NULL ,buff.length,
							  PROT_READ | PROT_WRITE,
							  MAP_SHARED,
							  fd, buff.m.offset);

		if (MAP_FAILED == buffers[n_buffers].start)
			die("memory mapping failed\n");

	}
}

static void init_dev(){
	struct v4l2_format format;
	struct v4l2_cropcap cropcap;

	memset( &format , 0 , sizeof( format ));
	memset( &cropcap, 0 , sizeof( cropcap));

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if( ioctl( fd , VIDIOC_CROPCAP , &cropcap ) < 0 )
		printf("cannot set crop capabilities\n");

/*FIX ME*/
//////////////////////////////////////////////////////////
/*
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;

	int array[] = {V4L2_CID_CONTRAST,V4L2_CID_GAMMA,V4L2_CID_HUE,V4L2_CID_BASE,V4L2_CID_EXPOSURE};
	for( int j = 2 ; j < 5; j++ ){

		memset(&queryctrl, 0, sizeof(queryctrl));
		queryctrl.id = array[j];

		if (-1 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
			if (errno != EINVAL) {
				perror("VIDIOC_QUERYCTRL");
	//			exit(EXIT_FAILURE);
		}else {
			printf("V4L2_CID_BRIGHTNESS is not supportedn");
		}
		}else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
			printf("V4L2_CID_BRIGHTNESS is not supportedn");
		} else {

				memset(&control, 0, sizeof (control));
				control.id = array[j];
				control.value = 50;
				printf("value :%d\n" , queryctrl.default_value );
			if (-1 == ioctl(fd, VIDIOC_S_CTRL, &control)) {
				perror("VIDIOC_S_CTRL");
//				exit(EXIT_FAILURE);
			}
		}
	}
*/
/////////////////////////////////////////////////////



	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width = 640;
	format.fmt.pix.height= 480;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
//	format.fmt.pix.field = V4L2_FIELD_INTERLACED;
	//format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
//	format.fmt.pix.sizeimage = 640 * 480 ;
//	format.fmt.pix.flags = V4L2_PIX_FMT_FLAG_PREMUL_ALPHA;
//	format.fmt.pix.flags = V4L2_PIX_FMT_PRIV_MAGIC;

	if( ioctl( fd , VIDIOC_S_FMT , &format ) < 0 )
		die("cannot set format");
	init_mmap();
}

static int min( int a , int b , int c ){
	int m = a;
	if( ( b < a ) && ( b < c ))
		m = b;
	if( ( c < a ) && ( c < b ))
		m = b;
	return m;
}

static int max( int a , int b , int c ){
	int m = a;
	if( ( b > a ) && ( b > c ))
		m = b;
	if( ( c > a ) && ( c > b ))
		m = b;
	return m;
}












static void readframe(){
	static int value = 0;
	times = 0;
	struct v4l2_buffer buf;

	memset( &buf , 0 , sizeof( buf ));

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if( ioctl( fd , VIDIOC_DQBUF , &buf ) < 0 )
		die( "cannot dequeue buffers" );



/*
	XEvent *event;
	XImage *image;
	int test = 0;

	image = malloc( sizeof(*image) );

	while(1){
		XNextEvent( display , event );
		switch( event->type ){
			case Expose:
				printf("Exposed\n");break;
			case ButtonPress:
				printf("Reading image buffer\n");
				image = XGetImage(display,window,0,0,300,300,AllPlanes,ZPixmap);
				if( image == NULL )
					printf("cannot read image\n");

				image->data = (char *) buffers[buf.index].start;

				XPutImage(display,window,gc,image,0,0,0,0,300,300);				



				test = 1;break;
			case KeyPress:
				printf("Next Frame\n");
				test = 1;break;
		}
		if( test ) break;

	}
*/


//	sleep( 1 );
	if( value > 3 ){
		printf("Writing image data ... Please Wait\n");
		char *data = (char *) buffers[buf.index].start;


		unsigned int j,red,green,blue,mn,mx;
	
		unsigned int dataLen = strlen( data );
		int len = dataLen;

		while( len >=0 ){
				red = ((char) data[len])   ;len--;
				green = ((char) data[len]) ;len--;
				blue = ((char) data[len])  ;len--;

				red = red + 30;
				blue = blue + 30;
				green = green + 30;



				for( j=0 ; j < 2 ; j++ ){
					fwrite( &blue , 1, 1, file );
					fwrite( &green , 1, 1, file );			
					fwrite( &red , 1, 1, file );
				}

				len--;

			
		}

		exit( EXIT_FAILURE );

	}

	if( ioctl( fd , VIDIOC_QBUF,   &buf ) < 0 )
		die( "cannot queue buffers" );
	value++;

}

static void mainloop(){
	int r;
	for( ; ; ){
		fd_set fds;
		struct timeval tv;

		FD_ZERO( &fds );
		FD_SET( fd , &fds );

		tv.tv_sec = 2;
		tv.tv_usec= 0;

		r = select( fd + 1 , &fds , NULL , NULL , &tv );
		if( r == -1 ){
			if( EINTR == errno )
				continue;
		}else if ( r == 0 )
			die("select timeout");
		else
			readframe();
	}
}


int main(){
	signal( SIGINT , handler );


	open_dev(  );
	init_dev(  );
	start_capture();
//	Vision();
	mainloop();


}
