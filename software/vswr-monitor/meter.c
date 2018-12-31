#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "meter.h"


static GtkWidget *parent_window;
static GtkWidget *meter;
static cairo_surface_t *meter_surface = NULL;

static int meter_width;
static int meter_height;
static int max_level=-200;
static int max_count=0;
static int max_reverse=0;

static void
meter_clear_surface (void)
{
  cairo_t *cr;
  cr = cairo_create (meter_surface);

  cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
  cairo_fill (cr);

  cairo_destroy (cr);
}

static gboolean
meter_configure_event_cb (GtkWidget *widget,
            GdkEventConfigure *event,
            gpointer           data)
{
	if (meter_surface) cairo_surface_destroy (meter_surface);

	meter_surface = gdk_window_create_similar_surface (
										gtk_widget_get_window (widget),
										CAIRO_CONTENT_COLOR,
										gtk_widget_get_allocated_width (widget),
										gtk_widget_get_allocated_height (widget));

	/* Initialize the surface to black */
	cairo_t *cr;
	cr = cairo_create (meter_surface);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_paint (cr);
	cairo_destroy (cr);

	return TRUE;
}

static gboolean
meter_draw_cb (GtkWidget *widget, cairo_t   *cr, gpointer   data) {
  cairo_set_source_surface (cr, meter_surface, 0.0, 0.0);
  cairo_paint (cr);
  return FALSE;
}


GtkWidget* meter_init(int width,int height,GtkWidget *parent) {
  GError *error;

  meter_width=width;
  meter_height=height;
  parent_window=parent;

  meter = gtk_drawing_area_new ();
  gtk_widget_set_size_request (meter, width, height);
 
  /* Signals used to handle the backing surface */
  g_signal_connect (meter, "draw",
            G_CALLBACK (meter_draw_cb), NULL);
  g_signal_connect (meter,"configure-event",
            G_CALLBACK (meter_configure_event_cb), NULL);

  /* Event signals */
  gtk_widget_set_events (meter, gtk_widget_get_events (meter)
                     | GDK_BUTTON_PRESS_MASK);

  return meter;
}

void meter_update(double value,double reverse) {
  
	char sf[32];
	double offset;
	cairo_t *cr;
	
	cr = cairo_create (meter_surface);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_paint (cr);

	cairo_set_font_size(cr, 12);

	double level=value;
	offset=220.0;

	int i;
	double x;
	double y;
	double angle;
	double radians;
	double cx=(double)meter_width/2.0;
	double cy=(double)meter_width/2.0;
	double radius=cy-20.0;

	cairo_set_line_width(cr, 1.0);
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_arc(cr, cx, cy, radius, 220.0*M_PI/180.0, 320.0*M_PI/180.0);
	cairo_stroke(cr);

	cairo_set_line_width(cr, 1.0);
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

	for(i=0;i<=100;i++) {
		angle=(double)i+offset;
		radians=angle*M_PI/180.0;

		switch(i) {
		  //case 5:
		  case 0:
		  case 25:
		  case 50:
		  case 75:
		  case 100:
			cairo_arc(cr, cx, cy, radius+4, radians, radians);
			cairo_get_current_point(cr, &x, &y);
			cairo_arc(cr, cx, cy, radius, radians, radians);
			cairo_line_to(cr, x, y);
			cairo_stroke(cr);

			sprintf(sf,"%d",i*2);
			cairo_arc(cr, cx, cy, radius+5, radians, radians);
			cairo_get_current_point(cr, &x, &y);
			cairo_new_path(cr);
			x-=6.0;
			cairo_move_to(cr, x, y);
			cairo_show_text(cr, sf);
			break;
		  default:
			if((i%5)==0) {
			  cairo_arc(cr, cx, cy, radius+2, radians, radians);
			  cairo_get_current_point(cr, &x, &y);
			  cairo_arc(cr, cx, cy, radius, radians, radians);
			  cairo_line_to(cr, x, y);
			  cairo_stroke(cr);
			}
			break;
		}
		cairo_new_path(cr);
	}

	cairo_set_line_width(cr, 1.0);
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

	if((int)value>max_level || max_count==10) {
	  max_level=(int)value;
	  max_count=0;
	}
	max_count++;

	angle=(max_level/2.0)+offset;
	radians=angle*M_PI/180.0;
	cairo_arc(cr, cx, cy, radius+8, radians, radians);
	cairo_line_to(cr, cx, cy);
	cairo_stroke(cr);

	cairo_select_font_face(cr, "FreeMono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 16);
	cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
	sprintf(sf,"%d W",(int)max_level);
	cairo_move_to(cr, 220, meter_height-6);
	cairo_show_text(cr, sf);

	double swr=(max_level+reverse)/(max_level-reverse);
	if(swr<0.0) swr=1.0;
	sprintf(sf,"SWR: %1.1f:1",swr);
	cairo_move_to(cr, 60, meter_height-6);
	cairo_show_text(cr, sf);
	
	cairo_destroy(cr);
	gtk_widget_queue_draw (meter);
}
