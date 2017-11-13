#include <gtk/gtk.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FULL_WIDTH 1920
#define FULL_HEIGHT 1080

#define WINDOW_FACTOR 25
#define WINDOW_WIDTH 16 * WINDOW_FACTOR
#define WINDOW_HEIGHT 9 * WINDOW_FACTOR

#define ZOOM_FACTOR 13
#define ZOOM_WIDTH 16 * ZOOM_FACTOR
#define ZOOM_HEIGHT 9 * ZOOM_FACTOR

#define STEP_FINE 5
#define STEP 15

#define INTERVAL 150

/*#define START_X 0*/
/*#define START_Y 505*/
/*#define START_GRAVITY GDK_GRAVITY_NORTH_WEST*/

#define START_X FULL_WIDTH
/*#define START_Y 625*/
#define START_Y FULL_HEIGHT
#define START_GRAVITY GDK_GRAVITY_SOUTH_EAST

/*#define srcX = FULL_WIDTH - ZOOM_WIDTH - 160;*/
/*#define srcY = 250;*/

/*#define srcX = FULL_WIDTH - ZOOM_WIDTH - 530;*/
/*#define srcY = 220;*/

#define SRC_X FULL_WIDTH - ZOOM_WIDTH
#define SRC_Y 0

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static GdkPixbuf *SCREENSHOT = NULL;

static gint srcX = SRC_X;
static gint srcY = SRC_Y;

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
  }

  if (key == gdk_keyval_from_name("Down")) {
    srcY += STEP;
  }

  if (key == gdk_keyval_from_name("Left")) {
    srcX -= STEP;
  }

  if (key == gdk_keyval_from_name("Right")) {
    srcX += STEP;
  }

  if (key == gdk_keyval_from_name("w")) {
    srcY -= STEP_FINE;
  }

  if (key == gdk_keyval_from_name("s")) {
    srcY += STEP_FINE;
  }

  if (key == gdk_keyval_from_name("a")) {
    srcX -= STEP_FINE;
  }

  if (key == gdk_keyval_from_name("d")) {
    srcX += STEP_FINE;
  }

  if (srcY < 0) {
    srcY = 0;
  }

  if (srcY > (FULL_HEIGHT - ZOOM_HEIGHT)) {
    srcY = FULL_HEIGHT - ZOOM_HEIGHT;
  }

  if (srcX < 0) {
    srcX = 0;
  }

  if (srcX > (FULL_WIDTH - ZOOM_WIDTH)) {
    srcX = FULL_WIDTH - ZOOM_WIDTH;
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

  gtk_window_set_gravity(GTK_WINDOW(mainWindow), START_GRAVITY);
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
