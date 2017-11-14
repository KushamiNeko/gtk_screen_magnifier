#include <gtk/gtk.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FULL_WIDTH 1920
#define FULL_HEIGHT 1080

/*#define WINDOW_FACTOR 25*/
/*#define WINDOW_WIDTH 16 * WINDOW_FACTOR*/
/*#define WINDOW_HEIGHT 9 * WINDOW_FACTOR*/
#define WINDOW_WIDTH 330
#define WINDOW_HEIGHT 200

#define STEP_FINE 5
#define STEP 20

#define INTERVAL 100

/*#define START_X 0*/
/*#define START_Y 505*/
/*#define START_GRAVITY GDK_GRAVITY_NORTH_WEST*/

#define START_X FULL_WIDTH
/*#define START_Y 625*/
#define START_Y FULL_HEIGHT
#define START_GRAVITY GDK_GRAVITY_SOUTH_EAST

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static GdkPixbuf* SCREENSHOT = NULL;

static gboolean FINE_STEP = FALSE;

static gint ZOOM_FACTOR;
static gint ZOOM_WIDTH;
static gint ZOOM_HEIGHT;

static gint SRCX;
static gint SRCY;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void init() {
  ZOOM_FACTOR = 12;
  ZOOM_WIDTH = 16 * ZOOM_FACTOR;
  ZOOM_HEIGHT = 9 * ZOOM_FACTOR;

  SRCX = FULL_WIDTH - ZOOM_WIDTH;
  SRCY = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void screenshotTake() {
  gint originX;
  gint originY;

  GdkWindow* rootWindow = gdk_get_default_root_window();

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

static GdkPixbuf* screenshotZoom() {
  GdkPixbuf* screenshotSub =
      gdk_pixbuf_new_subpixbuf(SCREENSHOT, SRCX, SRCY, ZOOM_WIDTH, ZOOM_HEIGHT);

  GdkPixbuf* screenshot = gdk_pixbuf_scale_simple(
      screenshotSub, WINDOW_WIDTH, WINDOW_HEIGHT, GDK_INTERP_BILINEAR);

  g_object_unref(screenshotSub);

  return screenshot;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static gboolean cbKeyPressed(GtkWidget* widget,
                             GdkEventKey* event,
                             void* data) {
  guint key = event->keyval;

  if (key == gdk_keyval_from_name("w")) {
    if (FINE_STEP) {
      SRCY -= STEP_FINE;
    } else {
      SRCY -= STEP;
    }
  }

  if (key == gdk_keyval_from_name("s")) {
    if (FINE_STEP) {
      SRCY += STEP_FINE;
    } else {
      SRCY += STEP;
    }
  }

  if (key == gdk_keyval_from_name("a")) {
    if (FINE_STEP) {
      SRCX -= STEP_FINE;
    } else {
      SRCX -= STEP;
    }
  }

  if (key == gdk_keyval_from_name("d")) {
    if (FINE_STEP) {
      SRCX += STEP_FINE;
    } else {
      SRCX += STEP;
    }
  }

  if (key == gdk_keyval_from_name("f")) {
    FINE_STEP = !FINE_STEP;
  }

  if (key == gdk_keyval_from_name("z")) {
    ZOOM_FACTOR -= 1;
    ZOOM_WIDTH = 16 * ZOOM_FACTOR;
    ZOOM_HEIGHT = 9 * ZOOM_FACTOR;
  }

  if (key == gdk_keyval_from_name("c")) {
    ZOOM_FACTOR += 1;
    ZOOM_WIDTH = 16 * ZOOM_FACTOR;
    ZOOM_HEIGHT = 9 * ZOOM_FACTOR;
  }

  if (SRCY < 0) {
    SRCY = 0;
  }

  if (SRCY > (FULL_HEIGHT - ZOOM_HEIGHT)) {
    SRCY = FULL_HEIGHT - ZOOM_HEIGHT;
  }

  if (SRCX < 0) {
    SRCX = 0;
  }

  if (SRCX > (FULL_WIDTH - ZOOM_WIDTH)) {
    SRCX = FULL_WIDTH - ZOOM_WIDTH;
  }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static gboolean cbRender(void* data) {
  GtkImage* mainImage = (GtkImage*)data;

  GdkPixbuf* screenOld = gtk_image_get_pixbuf(mainImage);

  gtk_image_clear(mainImage);

  if (screenOld != NULL) {
    g_object_unref(screenOld);
  }

  screenshotTake();
  GdkPixbuf* screen = screenshotZoom();

  gtk_image_set_from_pixbuf(mainImage, screen);

  // return FALSE to remove the timeout
  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void activate() {
  GtkWidget* mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(mainWindow), "Screen Magnifier");
  gtk_window_set_resizable(GTK_WINDOW(mainWindow), FALSE);

  gtk_window_set_gravity(GTK_WINDOW(mainWindow), START_GRAVITY);
  gtk_window_move(GTK_WINDOW(mainWindow), START_X, START_Y);
  gtk_window_set_keep_above(GTK_WINDOW(mainWindow), TRUE);

  GtkWidget* mainImage = gtk_image_new_from_pixbuf(NULL);

  gtk_container_add(GTK_CONTAINER(mainWindow), mainImage);

  g_signal_connect(GTK_WIDGET(mainWindow), "destroy", G_CALLBACK(gtk_main_quit),
                   NULL);

  g_signal_connect(mainWindow, "key-release-event", G_CALLBACK(cbKeyPressed),
                   NULL);

  g_timeout_add(INTERVAL, cbRender, mainImage);

  gtk_widget_show_all(GTK_WIDGET(mainWindow));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
  gtk_init(&argc, &argv);

  init();

  activate();

  gtk_main();

  return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
