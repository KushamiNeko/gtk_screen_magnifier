#include <cairo.h>
#include <gtk/gtk.h>
#include <pango/pangocairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FULL_WIDTH 1920
#define FULL_HEIGHT 1080

#define START_X 0
#define START_Y 505

#define WINDOW_FACTOR 28
#define WINDOW_WIDTH 16 * WINDOW_FACTOR
#define WINDOW_HEIGHT 9 * WINDOW_FACTOR

#define ZOOM_FACTOR 13
#define ZOOM_WIDTH 16 * ZOOM_FACTOR
#define ZOOM_HEIGHT 9 * ZOOM_FACTOR

#define STEP 5
#define INTERVAL 150

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static GdkPixbuf *SCREENSHOT = NULL;

static gint srcX = FULL_WIDTH - ZOOM_WIDTH - 160;
static gint srcY = 250;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void screenshotTake() {
  gint originX;
  gint originY;

  GdkWindow *rootWindow = gdk_get_default_root_window();

  gint width = gdk_window_get_width(rootWindow);
  gint height = gdk_window_get_height(rootWindow);

  gdk_window_get_origin(rootWindow, &originX, &originY);

  if (SCREENSHOT != NULL) {
    g_object_unref(SCREENSHOT);
  }

  SCREENSHOT =
      gdk_pixbuf_get_from_window(rootWindow, originX, originY, width, height);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static GdkPixbuf *screenshotZoom() {

  GdkPixbuf *screenshotSub =
      gdk_pixbuf_new_subpixbuf(SCREENSHOT, srcX, srcY, ZOOM_WIDTH, ZOOM_HEIGHT);

  GdkPixbuf *screenshot = gdk_pixbuf_scale_simple(
      screenshotSub, WINDOW_WIDTH, WINDOW_HEIGHT, GDK_INTERP_BILINEAR);

  g_object_unref(screenshotSub);

  return screenshot;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static gboolean cbKeyPressed(GtkWidget *widget, GdkEventKey *event,
                             void *data) {

  guint key = event->keyval;

  if (key == gdk_keyval_from_name("Up")) {
    srcY -= STEP;
    if (srcY < 0) {
      srcY = 0;
    }
  }

  if (key == gdk_keyval_from_name("Down")) {
    srcY += STEP;

    if (srcY > (FULL_HEIGHT - ZOOM_HEIGHT)) {
      srcY = FULL_HEIGHT - ZOOM_HEIGHT;
    }
  }

  if (key == gdk_keyval_from_name("Left")) {
    srcX -= STEP;

    if (srcX < 0) {
      srcX = 0;
    }
  }

  if (key == gdk_keyval_from_name("Right")) {
    srcX += STEP;

    if (srcX > (FULL_WIDTH - ZOOM_WIDTH)) {
      srcX = FULL_WIDTH - ZOOM_WIDTH;
    }
  }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static gboolean cbRender(void *data) {

  GtkImage *mainImage = (GtkImage *)data;

  GdkPixbuf *screenOld = gtk_image_get_pixbuf(mainImage);

  gtk_image_clear(mainImage);

  if (screenOld != NULL) {
    g_object_unref(screenOld);
  }

  screenshotTake();
  GdkPixbuf *screen = screenshotZoom();

  gtk_image_set_from_pixbuf(mainImage, screen);

  // return FALSE to remove the timeout
  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void activate() {

  GtkWidget *mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(mainWindow), "Screen Magnifier");
  gtk_window_set_resizable(GTK_WINDOW(mainWindow), FALSE);

  gtk_window_set_gravity(GTK_WINDOW(mainWindow), GDK_GRAVITY_NORTH_WEST);
  gtk_window_move(GTK_WINDOW(mainWindow), START_X, START_Y);
  gtk_window_set_keep_above(GTK_WINDOW(mainWindow), TRUE);

  GtkWidget *mainImage = gtk_image_new_from_pixbuf(NULL);

  gtk_container_add(GTK_CONTAINER(mainWindow), mainImage);

  g_signal_connect(GTK_WIDGET(mainWindow), "destroy", G_CALLBACK(gtk_main_quit),
                   NULL);

  g_signal_connect(mainWindow, "key-release-event", G_CALLBACK(cbKeyPressed),
                   NULL);

  g_timeout_add(INTERVAL, cbRender, mainImage);

  gtk_widget_show_all(GTK_WIDGET(mainWindow));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {

  gtk_init(&argc, &argv);

  activate();

  gtk_main();

  return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
