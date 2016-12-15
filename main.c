#include <stdio.h>
#include <unistd.h>
#include <directfb.h>
#include "eink_lib.h"
#include <time.h>

static IDirectFB *dfb = NULL;
static IDirectFBSurface *primary = NULL;
static IDirectFBFont *font = NULL;
static IDirectFBFont *fontSubheader = NULL;

static int screen_width  = 0;
static int screen_height = 0;

static void
init_application( int *argc, char **argv[] )
{
     DFBResult             ret;
     DFBSurfaceDescription desc;
     DFBFontDescription font_dsc;

     /* Initialize DirectFB including command line parsing. */
     ret = DirectFBInit( argc, argv );
     if (ret) {
          DirectFBError( "DirectFBInit() failed", ret );
     }

     /* Create the super interface. */
     ret = DirectFBCreate( &dfb );
     if (ret) {
          DirectFBError( "DirectFBCreate() failed", ret );
     }

     /* Request fullscreen mode. */
     dfb->SetCooperativeLevel( dfb, DFSCL_FULLSCREEN );

     /* Fill the surface description. */
     desc.flags       = DSDESC_CAPS | DSDESC_PIXELFORMAT;
     desc.caps        = DSCAPS_PRIMARY | DSCAPS_DOUBLE;
     desc.pixelformat = DSPF_ARGB;
     
     /* Create an 8 bit palette surface. */
     ret = dfb->CreateSurface( dfb, &desc, &primary );
     if (ret) {
          DirectFBError( "IDirectFB::CreateSurface() failed", ret );
     }

     /* Create the font */
     font_dsc.flags = DFDESC_HEIGHT;
     font_dsc.height = 100;
     dfb->CreateFont (dfb, "/usr/share/directfb-examples/fonts/decker.ttf", &font_dsc, &font);
     font_dsc.height = 50;
     dfb->CreateFont (dfb, "/usr/share/directfb-examples/fonts/decker.ttf", &font_dsc, &fontSubheader);     


     /* Clear with black. */
     primary->Clear( primary, 0x00, 0x00, 0x00, 0xff );
     primary->Flip( primary, NULL, 0 );

}

pt pressed_pos;
bool pressing = false;
bool processed = false;
char * numPadStr = NULL;

bool insideRect(pt pos, int x, int y, int w, int h){
	return ((pos.x >= x ) &&
(pos.x <= x+w) &&
(pos.y >= y ) &&
(pos.y <= y+h));

}

void render_number(int n, int x, int y){
		  int btn_size = 200;
		  int margin = 10;
		  x = x * btn_size;
		  y = y * btn_size;
		  x += ((1024/2)-(btn_size*3)/2) - 150;
		  y += ((765/2)-(btn_size*3)/2) + 30;

		  if (pressing && insideRect(pressed_pos, x,y, btn_size-margin, btn_size-margin)){
			primary->SetColor(primary, 0xff,0xff,0xff,0xff);
			  if (numPadStr == NULL){
				  asprintf(&numPadStr, "%d", n);
			  }else{
				  char *tmpStr;
				  asprintf(&tmpStr, "%s%d",numPadStr, n);
				  free(numPadStr);
				  numPadStr = tmpStr;
			  }
		  } else primary->SetColor(primary, 0x00,0x00,0x00,0xff);

		  primary->FillRectangle(primary, x,y, btn_size-margin, btn_size-margin);
		  char* str;
                  asprintf(&str, "%d", n);

		  if (pressing && insideRect(pressed_pos, x,y, btn_size-margin, btn_size-margin)){
			primary->SetColor(primary, 0x00,0x00,0x00,0x00);
		  } else primary->SetColor(primary, 0xff,0xff,0xff,0xff);
		  
		  primary->DrawString(primary, str, -1, x+((btn_size-margin)/2), y+((btn_size-margin)/2), DSTF_CENTER);
		  free(str);

}

bool releaseNPADCSTR = false;

void call_fct1(){
	numPadStr = NULL;
}

void call_fct2(){
	numPadStr = "/!\\ ERROR";
	releaseNPADCSTR = true;
}

void render_btn(char* str, int x, int y, void (*funcptr)()){
		  int btn_size = 200;
		  int margin = 10;
		  x = x * btn_size;
		  y = y * btn_size;
		  x += ((1024/2)-(btn_size*3)/2) - 150;
		  y += ((765/2)-(btn_size*3)/2) + 30;

		  if (pressing && insideRect(pressed_pos, x,y, btn_size-margin, btn_size-margin)){
			primary->SetColor(primary, 0xff,0xff,0xff,0xff);
		  	funcptr();
		  } else primary->SetColor(primary, 0x00,0x00,0x00,0xff);

		  primary->FillRectangle(primary, x,y, btn_size-margin, btn_size-margin);

		  if (pressing && insideRect(pressed_pos, x,y, btn_size-margin, btn_size-margin)){
			primary->SetColor(primary, 0x00,0x00,0x00,0x00);
		  } else primary->SetColor(primary, 0xff,0xff,0xff,0xff);
		  
		  primary->DrawString(primary, str, -1, x+((btn_size-margin)/2), y+((btn_size-margin)/2), DSTF_CENTER);

}

void render_frame(){
	     	  primary->Clear( primary, 0xff, 0xff, 0xff, 0xff );
		  primary->SetFont (primary, fontSubheader);

		  primary->SetColor(primary, 0x00,0x00,0x00,0xff);
		  primary->FillRectangle(primary, 0,0, 1024, 70);
		  	
		  primary->SetColor(primary, 0xff,0xff,0xff,0xff);
		  if (numPadStr != NULL)
			  primary->DrawString(primary, numPadStr, -1, 1024/2, 50, DSTF_CENTER);
		  if (releaseNPADCSTR) {
			 releaseNPADCSTR = false;
			 numPadStr = NULL;
		  }

		  primary->SetColor(primary, 0x00,0x00,0x00,0x00);
		  primary->DrawString(primary, "Numpad", -1, 1024-200, 200, DSTF_CENTER);

	          int i = 0;
		  for (int y = 0; y < 3; y++)
		  	for (int x = 0; x < 3; x++)
				render_number(i++, x,y);

		  render_btn("C", 3,2, call_fct1);
		  render_btn(">", 3,1, call_fct2);

		  // End of drawing!
		  primary->Flip( primary, NULL, DSFLIP_NONE );
		  // This func clears the screen , but it does WITH artifacts. It's faster
	     	  eink_update();
}

int main (int argc, char **argv)
{
	init_application(&argc,&argv);

	// We init eink and touch
	eink_init();
	touch_init();

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	primary->Clear( primary, 0xff, 0xff, 0xff, 0xff );
	primary->Flip( primary, NULL, 0 );
	// This func clears the screen , but it does without artifacts. It's slow
	eink_force_update();

	primary->SetColor(primary, 0x00,0x00,0x00,0xff);
	pressed_pos.x = 0;
	pressed_pos.y = 0;
	while (1){
		pressing = pressed_pos.x != 0 && pressed_pos.y != 0;
		if (pressing && !processed){
			processed = true;
			printf("Pressed\n", pressed_pos.x, pressed_pos.y);
			render_frame();
		}else if (!pressing) {
			processed = false;
			printf("Released\n");
			render_frame();
		}
		pressed_pos = touch_pos();
	}


	return 23;
}
