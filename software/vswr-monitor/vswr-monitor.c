#include <gtk/gtk.h>
#include <cairo.h>
#include <pigpio.h>
#include <unistd.h>

#include "meter.h"

#define MAX_DISPLAY_WIDTH  400
#define MAX_DISPLAY_HEIGHT 120

#define METER_WIDTH (400)
#define METER_HEIGHT (120)

GtkWidget *top_window;
GtkWidget *fixed;
GtkWidget *canvas;
GtkWidget *grid;
GtkWidget *parent_window;
GtkWidget *meter;
gint display_width;
gint display_height;
gint full_screen;


#define MAX11613_ADDRESS	0x34		
unsigned char data[8];
unsigned int i2c_bus;
int i2c_handler = 0;


int initI2C() {
	if (gpioInitialise() < 0) {
		fprintf(stderr,"gpio could not be initialized. \n");
		return -1;
	}
	
	i2c_bus = 1;
	i2c_handler = i2cOpen(i2c_bus, MAX11613_ADDRESS, 0);
	
	if (i2c_handler < 0)  return -1;
	
	unsigned char config[1];
	config[0] = 0x07;
	int writeResult = i2cWriteDevice(i2c_handler, config, 1);
	if (writeResult != 0 ) {
		printf ("adc config error");
		return -1;
	}
}

gboolean main_delete (GtkWidget *widget) {
	
	i2cClose(i2c_handler);
	gpioTerminate();
	
	_exit(0);
}

int update_meter(void *dummy) {
	
	int result = i2cReadDevice(i2c_handler, data, 8);
	int rev = (int)(((data[0] & 0x0F) <<8) | data[1]);
	int fwv = (int)(((data[6]&0x0F)<<8) | data[7]);

	double constant1=3.3;	//reference voltage
	double constant2=0.09;	//bridge voltage
	double v1;
	v1=((double)fwv/4095.0)*constant1;
	double fw_pwr=(v1*v1)/constant2;

	double rev_pwr = 0.0;
	v1=((double)rev/4095.0)*constant1;
	rev_pwr=(v1*v1)/constant2;

	meter_update(fw_pwr, rev_pwr);
	
	return TRUE;
}

static void activate (GtkApplication *app, gpointer user_data)
{
  top_window = gtk_application_window_new (app);
  gtk_widget_set_size_request(top_window, MAX_DISPLAY_WIDTH, MAX_DISPLAY_HEIGHT);
  gtk_window_set_title (GTK_WINDOW (top_window), "VSWR Monitor");
  gtk_window_set_position(GTK_WINDOW(top_window),GTK_WIN_POS_CENTER_ALWAYS);
  gtk_window_set_resizable(GTK_WINDOW(top_window), FALSE);
  
  g_signal_connect (top_window, "delete-event", G_CALLBACK (main_delete), NULL);
  
  grid = gtk_grid_new();
  gtk_widget_set_size_request(grid, MAX_DISPLAY_WIDTH, MAX_DISPLAY_HEIGHT);
  gtk_grid_set_row_homogeneous(GTK_GRID(grid),FALSE);
  gtk_grid_set_column_homogeneous(GTK_GRID(grid),FALSE);
  gtk_container_add (GTK_CONTAINER (top_window), grid);
  
  gtk_widget_show_all(top_window);
  
  gtk_window_set_title (GTK_WINDOW (top_window), "VSWR-MONITOR");
  
  fixed=gtk_fixed_new();
  gtk_container_remove(GTK_CONTAINER(top_window),grid);
  gtk_container_add(GTK_CONTAINER(top_window), fixed);
  
  meter = meter_init(METER_WIDTH,METER_HEIGHT,top_window);
  gtk_fixed_put(GTK_FIXED(fixed),meter,0,0);
  
  gtk_widget_show_all (fixed);

  gdk_threads_add_timeout_full(G_PRIORITY_HIGH_IDLE,10, update_meter, NULL, NULL);
  
  gdk_window_set_cursor(gtk_widget_get_window(top_window),gdk_cursor_new(GDK_ARROW));

}

int main (int argc, char **argv)
{
	if (initI2C() < 0) return -1;	
	
	GtkApplication *app;
	int status;
	app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);
	
	return status;
}
