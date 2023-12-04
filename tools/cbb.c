#include "../src/engine/util.h"
#include <gtk/gtk.h>
#include <string.h>
#include <stdbool.h>

#define STRING_SIZE 100

static char** assetStrings = NULL;
static int currentAsset = 0;
static int currentAssetWidth = 32;
static int currentAssetHeight = 32;
static GtkWidget* grid;

// Returns true if square is occupied by an asset originating in another square.
// Used when the size of the asset is bigger than the size of a square.
static bool squareIsOccupied(GtkWidget* square) {
  return gtk_widget_get_css_classes(square)[0] != NULL;
}

// Return true if the square has an asset associated with it.
// We can only remove assets from squares that have an asset.
static bool squareHasAsset(GtkWidget* square) {
  return (gtk_picture_get_file(GTK_PICTURE(square)) != NULL);
}

static void updateSquare(GtkWidget* square, int assetIndex, int width, int height) {
  // Store the index of the asset - used when we write to file
  // Also store the width and height of the asset.
  // Pretty bad way of doing this. We're storing it in the accessibility text.
  char indexString[100];
  gtk_picture_set_filename(GTK_PICTURE(square), assetStrings[assetIndex]);
  sprintf(indexString, "%i %i %i", assetIndex, width, height);
  gtk_picture_set_alternative_text(GTK_PICTURE(square), indexString);
}

static void occupyNearbySquares(int baseX, int baseY, int width, int height) {
  // We also need to account for the case where the size of the asset is greater
  // than the size of the grid square. We want to indicate that nearby squares
  // "belong to" the sprite. So, we add the "occupied" class to the square.
  int squareCountX = width / 32;
  int squareCountY = height / 32;
  for (int x = 0; x < squareCountX; x++) {
    for (int y = 0; y < squareCountY; y++) {
      if (x == 0 && y == 0) {
	continue;
      }
      GtkWidget* square = gtk_grid_get_child_at(GTK_GRID(grid), baseX + x, baseY + y);
      gtk_widget_add_css_class(square, "occupied");
    }
  }
}

static void loadSpritemap(char* filename) {
  FILE* fp = fopen(filename, "r");
  int spriteCount = countLines(filename);
  size_t len = 0;
  char* line = NULL;
  int assetIndex;
  int x, y, w, h;
  int col, row;
  int i = 0;

  // Iterate through entire file. Insert sprites into grid.
  while (getline(&line, &len, fp) != -1) {
    line[strcspn(line, "\n")] = '\0';

    assetIndex = atoi(strtok(line, " "));
    x = atoi(strtok(NULL, " "));
    y = atoi(strtok(NULL, " "));
    w = atoi(strtok(NULL, " "));
    h = atoi(strtok(NULL, " "));
    col = x / 32;
    row = y / 32;
    GtkWidget* square = gtk_grid_get_child_at(GTK_GRID(grid), col, row);
    updateSquare(square, assetIndex, w, h);
    occupyNearbySquares(col, row, w, h);
  }
}


static char** readAssetStrings(const char* filename) {
  char* line = NULL;
  FILE* fp = NULL;
  size_t len;
  int numStrings = countLines(filename) + 1;
  char** strings = malloc(sizeof(char*) * numStrings);
  fp = fopen(filename, "r");
  int i = 0;
  while(getline(&line, &len, fp) != -1) {
    strings[i] = malloc(sizeof(char) * STRING_SIZE);
    line[strcspn(line, "\n")] = '\0';
    strcpy(strings[i], line);
    i++;
  }
  strings[i] = NULL;

  fclose(fp);
  return strings;
}

static void selectAsset(GtkWidget* dropDown, GParamSpec* pspec, gpointer data) {
  //GtkWidget* currentSprite = (GtkWidget*)data;
  guint assetIndex = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropDown));
  gtk_picture_set_filename(GTK_PICTURE(data), assetStrings[assetIndex]);
  currentAsset = assetIndex;
}

static void addAssetToSquare (GtkGestureClick *gesture,
			      int              n_press,
			      double           mouseX,
			      double           mouseY,
			      GtkWidget       *area)
{
  if (!squareIsOccupied(area)) {
    updateSquare(area, currentAsset, currentAssetWidth, currentAssetHeight);
    // Get position of 'area' in grid
    int areaX, areaY;
    gtk_grid_query_child(GTK_GRID(grid), area, &areaX, &areaY, NULL, NULL);
    occupyNearbySquares(areaX, areaY, currentAssetWidth, currentAssetHeight);
  }
}

static void removeAssetFromSquare (GtkGestureClick *gesture,
				   int              n_press,
				   double           x,
				   double           y,
				   GtkWidget       *area)
{
  if (!squareIsOccupied(area) && squareHasAsset(area)) {
    gtk_picture_set_filename(GTK_PICTURE(area), NULL);
    // We might need to mark nearby squares as unoccupied.
    int areaX, areaY;
    gtk_grid_query_child(GTK_GRID(grid), area, &areaX, &areaY, NULL, NULL);

    const char* altText = gtk_picture_get_alternative_text(GTK_PICTURE(area));
    int index = atoi(strsep(&altText, " "));
    int width = atoi(strsep(&altText, " "));
    int height = atoi(strsep(&altText, " "));
    int squareCountX = width / 32;
    int squareCountY = height / 32;
    for (int x = 0; x < squareCountX; x++) {
      for (int y = 0; y < squareCountY; y++) {
	if (x == 0 && y == 0) {
	  continue;
	}
	GtkWidget* square = gtk_grid_get_child_at(GTK_GRID(grid), areaX + x, areaY + y);
	gtk_widget_remove_css_class(square, "occupied");
      }
    }
  }
}


static void generateSpritemap(GtkWidget* button, gpointer data) {
  FILE* fp = fopen("spritemap.dat", "w");
  for (int x = 0; x < 150; x++) {
    for (int y = 0; y < 150; y++) {
      GtkWidget* picture = gtk_grid_get_child_at(GTK_GRID(data), x, y);
      if (gtk_picture_get_file(GTK_PICTURE(picture)) != NULL) {
	// Get index, width, and height from accessibility text
	const char* altText = gtk_picture_get_alternative_text(GTK_PICTURE(picture));
	int index = atoi(strsep(&altText, " "));
	int w = atoi(strsep(&altText, " "));
	int h = atoi(strsep(&altText, " "));
	
	//const char* index = gtk_picture_get_alternative_text(GTK_PICTURE(picture));
	fprintf(fp, "%i %i %i %i %i\n", index, x*32, y*32, w, h);
      }
    }
  }

  fclose(fp);
}

static void setAssetWidth(GtkSpinButton* widthWidget, gpointer data) {
  currentAssetWidth = gtk_spin_button_get_value_as_int(widthWidget);
}

static void setAssetHeight(GtkSpinButton* heightWidget, gpointer data) {
  currentAssetHeight = gtk_spin_button_get_value_as_int(heightWidget);
}

static void fileSelected(GObject* dialog, GAsyncResult* res, gpointer data) {
  GFile* f = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(dialog), res, NULL);
  // Will be NULL if we cancel out of file dialog
  if (G_IS_FILE(f)) {
    loadSpritemap(g_file_get_path(f));
  }
}

static void fileButtonPressed(GtkWidget* button, gpointer dialog) {
  gtk_file_dialog_open(GTK_FILE_DIALOG(dialog), NULL, NULL, fileSelected, NULL);
}

static void activate(GtkApplication *app,
		     gpointer userData) {
  GtkWidget* window;
  GtkWidget* scrolledWindow;
  GtkWidget* box;
  GtkWidget* rightBox;
  GtkWidget* button;
  GtkWidget* currentSprite;
  GtkWidget* spriteDropDown;
  GtkWidget* spriteSelectBox;
  GtkWidget* dimensionsBox;
  GtkWidget* assetWidth;
  GtkWidget* widthBox; // For holding label
  GtkWidget* widthLabel;
  GtkWidget* assetHeight;
  GtkWidget* heightBox; // For holding label
  GtkWidget* heightLabel;
  GtkWidget* fileButton;
  GtkFileDialog* fileDialog;
  

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "COI Board Builder");
  gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
  
  grid = gtk_grid_new();
  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 50);
  
  // Left side: Grid
  scrolledWindow = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), grid);
  gtk_widget_set_size_request(scrolledWindow, 700, 700);
  gtk_box_append(GTK_BOX(box), scrolledWindow);

  // Right side: Submit button and Picture
  rightBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
  // Top right: Picture and drop down
  spriteSelectBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  assetStrings = readAssetStrings("src/engine/etc/mapping.cfg");
  spriteDropDown = gtk_drop_down_new_from_strings(assetStrings);
  currentSprite = gtk_picture_new_for_filename(assetStrings[0]);
  gtk_widget_set_size_request(currentSprite, 100, 100);
  gtk_box_append(GTK_BOX(spriteSelectBox), currentSprite);
  gtk_box_append(GTK_BOX(spriteSelectBox), spriteDropDown);
  g_signal_connect(spriteDropDown, "notify::selected-item", G_CALLBACK(selectAsset), currentSprite);

  
  // Middle right: Adjust asset width/height
  dimensionsBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 50);
  widthBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  widthLabel = gtk_label_new("Width");
  assetWidth = gtk_spin_button_new_with_range(32, 3200, 32);
  gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(assetWidth), TRUE);
  g_signal_connect(GTK_SPIN_BUTTON(assetWidth), "value-changed", G_CALLBACK(setAssetWidth), NULL);
  gtk_box_append(GTK_BOX(widthBox), widthLabel);
  gtk_box_append(GTK_BOX(widthBox), assetWidth);
  
  heightBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  heightLabel = gtk_label_new("Height");
  assetHeight = gtk_spin_button_new_with_range(32, 3200, 32);
  gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(assetHeight), TRUE);
  g_signal_connect(GTK_SPIN_BUTTON(assetHeight), "value-changed", G_CALLBACK(setAssetHeight), NULL);
  gtk_box_append(GTK_BOX(heightBox), heightLabel);
  gtk_box_append(GTK_BOX(heightBox), assetHeight);
  
  gtk_box_append(GTK_BOX(dimensionsBox), widthBox);
  gtk_box_append(GTK_BOX(dimensionsBox), heightBox);

  fileDialog = gtk_file_dialog_new();
  fileButton = gtk_button_new_with_label("Load Spritemap");
  g_signal_connect(fileButton, "clicked", G_CALLBACK(fileButtonPressed), fileDialog);
  
  button = gtk_button_new_with_label("Create Board");
  g_signal_connect(button, "clicked", G_CALLBACK(generateSpritemap), grid);

  gtk_box_append(GTK_BOX(rightBox), spriteSelectBox);
  gtk_box_append(GTK_BOX(rightBox), dimensionsBox);
  gtk_box_append(GTK_BOX(rightBox), fileButton);
  gtk_box_append(GTK_BOX(rightBox), button);
  gtk_box_append(GTK_BOX(box), rightBox);

  // Margins
  gtk_widget_set_margin_top(box, 20);
  gtk_widget_set_margin_bottom(box, 20);
  gtk_widget_set_margin_start(box, 20);
  gtk_widget_set_margin_end(box, 20);

  gtk_widget_set_hexpand(scrolledWindow, TRUE);
  gtk_widget_set_halign(rightBox, GTK_ALIGN_FILL);

  // CSS
  GtkCssProvider* cssProvider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(cssProvider, "tools/cbb.css");
  gtk_style_context_add_provider_for_display(gdk_display_get_default(),
					     GTK_STYLE_PROVIDER(cssProvider),
					     GTK_STYLE_PROVIDER_PRIORITY_USER);

  for (int x = 0; x < 150; x++) {
    for (int y = 0; y < 150; y++) {
      GtkWidget* image = gtk_picture_new();
      gtk_widget_set_size_request(image, 20, 20);

      // Left click - set asset
      GtkGesture* gestureLeft = gtk_gesture_click_new();
      gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gestureLeft), 1);
      gtk_widget_add_controller(image, GTK_EVENT_CONTROLLER(gestureLeft));
      g_signal_connect(gestureLeft, "pressed", G_CALLBACK(addAssetToSquare), image);
      // Right click - remove asset
      GtkGesture* gestureRight = gtk_gesture_click_new();
      gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gestureRight), 3);
      gtk_widget_add_controller(image, GTK_EVENT_CONTROLLER(gestureRight));
      g_signal_connect(gestureRight, "pressed", G_CALLBACK(removeAssetFromSquare), image);
      
      //GtkGestureSingle* gesture = gtk_gesture_single
      
      gtk_grid_attach(GTK_GRID(grid), image, x, y, 1, 1);
    }
  }

  gtk_window_set_child(GTK_WINDOW(window), box);
  gtk_window_present(GTK_WINDOW(window));
}

int main (int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  // Clean up malloced strings
  if (assetStrings != NULL) {
    int i = 0;
    char* string = assetStrings[i];
    while (string != NULL) {
      free(string);
      i++;
      string = assetStrings[i];
    }
    free(assetStrings);
  }
  

  return status;
}
