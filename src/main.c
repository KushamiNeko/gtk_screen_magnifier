#include <cairo.h>
#include <gtk/gtk.h>
#include <pango/pangocairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FULL_WIDTH 1920
#define FULL_HEIGHT 1080

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 450

#define FACTOR 15
#define ZOOM_WIDTH 16 * FACTOR
#define ZOOM_HEIGHT 9 * FACTOR

#define STEP 10
#define INTERVAL 100

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static GdkPixbuf *SCREENSHOT = NULL;

static gint srcX = FULL_WIDTH - ZOOM_WIDTH - 70;
static gint srcY = 277;

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

struct KeyPressedData {
  GtkWindow *window;
  GtkImage *image;
};

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

  gtk_image_clear(mainImage);

  screenshotTake();
  GdkPixbuf *screen = screenshotZoom();

  gtk_image_set_from_pixbuf(mainImage, screen);

  // return FALSE to remove the timeout
  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void *activate() {

  GtkWidget *mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(mainWindow), "Screen Magnifier");
  gtk_window_set_resizable(GTK_WINDOW(mainWindow), FALSE);

  gtk_window_set_gravity(GTK_WINDOW(mainWindow), GDK_GRAVITY_SOUTH_EAST);
  gtk_window_move(GTK_WINDOW(mainWindow), FULL_WIDTH, FULL_HEIGHT);
  gtk_window_set_keep_above(GTK_WINDOW(mainWindow), TRUE);

  GtkWidget *mainImage = gtk_image_new_from_pixbuf(NULL);

  gtk_container_add(GTK_CONTAINER(mainWindow), mainImage);

  struct KeyPressedData *data = malloc(sizeof(struct KeyPressedData));

  data->window = GTK_WINDOW(mainWindow);
  data->image = GTK_IMAGE(mainImage);

  g_signal_connect(GTK_WIDGET(mainWindow), "destroy", G_CALLBACK(gtk_main_quit),
                   NULL);

  g_signal_connect(mainWindow, "key-release-event", G_CALLBACK(cbKeyPressed),
                   NULL);

  g_timeout_add(INTERVAL, cbRender, mainImage);

  gtk_widget_show_all(GTK_WIDGET(mainWindow));

  return data;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {

  gtk_init(&argc, &argv);

  void *data = activate();

  gtk_main();

  free(data);

  return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
