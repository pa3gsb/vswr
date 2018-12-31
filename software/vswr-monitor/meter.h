#ifndef _METER_H
#define _METER_H

#include <gtk/gtk.h>

extern GtkWidget* meter_init(int width,int height,GtkWidget *parent);
extern void meter_update(double value,double reverse);

#endif
