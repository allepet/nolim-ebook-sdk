#include <asm/types.h>
#include <fcntl.h>
#include <termios.h>

typedef struct { int x,y; } pt;
typedef struct{ __u32 sel, fb_id,mode, coordinate[] } eink;

int touch_dev;
unsigned char* data ;
int n = 0;
pt pos;

int dev;
eink args;

void touch_init(){
	touch_dev = open("/dev/cyio", O_RDONLY);
	data = malloc(1024);
	sleep(2);
	tcflush(touch_dev,TCIOFLUSH);
}

pt get_touch_position(unsigned char* data){
	pt pos;
	pos.x = 255*data[4]+data[3];
	pos.y = 255*data[6]+data[5];
	return pos;
}

pt touch_pos(){
	n = read(touch_dev,(void*)data,1024);
	if (n > 0)
           pos = get_touch_position(data);
	return pos;
}

void eink_init(){
	dev = open("/dev/disp", O_RDWR);
	args.sel = 0;
	args.fb_id = 0;
	args.mode = 0x80;
	__u32 coordinates[4] = {0};
	memcpy(args.coordinate, coordinates, 4);
}

void eink_force_update(){
	__u32 bc = args.mode;
	args.mode = 0x04;
    	ioctl(dev, 0x402, &args);
	args.mode = bc;
}

void eink_update(){
    	ioctl(dev, 0x402, &args);
}
