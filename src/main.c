// Should have a better system for includes
#include "inputloops.h"
#include "armory/Armory.h"
#include "items.h"
#include "tests.h"
#include "player.h"
#include "threadtown/Town.h"

#if defined(__NATIVE__) && !defined(__WINDOWS__)
#include <emscripten.h>
#include <emscripten/html5.h>
#endif


// Our "main loop" function. This callback receives the current time as
// reported by the browser, and the user data we provide in the call to
// emscripten_request_animation_frame_loop().
/*
EM_BOOL one_iter(double time, void* userData) {
  // Can render to the screen here, etc.
  puts("one iteration");
  // Return true to keep the loop running.
  return EM_TRUE;
}
*/


void _init() {
  printf("in the thing\n");
  
  // Initialize global window and asset loader
  COIWindowInit();
  printf("window inited\n");
  COIAssetLoaderInit();
  
  // Global item data
  /*
  ItemList* itemList = loadItems();

  // Test inventory
  Inventory* inventory = createTestInventory(itemList);

  // Initialize player data
  COISprite* playerSprite = COISpriteCreateFromAssetID(2240, 1984, 32, 32, loader, 1, COIWindowGetRenderer(window));
  PlayerInfo* pInfo = playerInfoCreate("Wique", playerSprite, inventory);
  actorFaceDown(pInfo->party[0]);
  */

  printf("before title board\n");
  COIBoard* titleBoard = titleCreateBoard();
  COIWindowSetBoard(COI_GLOBAL_WINDOW, titleBoard, title);
  
  //COIBoard* townBoard = townCreateBoard(window, loader, pInfo);
  //COILoop threadTownLoop = &threadTown;
  //COIWindowSetBoard(window, townBoard, threadTownLoop);
  /*#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(COIWindowLoop, (void*)window, 0, 0);
  #else
  COIWindowLoop(window);
  #endif
  */
}

void _cleanup() {
    // Cleanup
  COIAssetLoaderDestroy(COI_GLOBAL_LOADER);
  //COIBoardDestroy(townBoard);
  //titleDestroyBoard((TitleContext*)titleBoard->context); // jnw: cleanup - leaking here bc it's commenbted out
  COIWindowDestroy(COI_GLOBAL_WINDOW);
  //ItemListDestroy(itemList);
  //inventoryDestroy(inventory);
}

void _mainLoop() {
  COIWindowLoop(COI_GLOBAL_WINDOW, true);
}

void _mainLoopEMCC() {
  // If we're using emscripten, we can't do the looping. Let emscripten do it (see call to this function)
  COIWindowLoop(COI_GLOBAL_WINDOW, false);
}


int main(int argc, char** argv) {
  /*EmscriptenWebGLContextAttributes attrs;
  attrs.antialias = true;
  attrs.majorVersion = 3;
  attrs.minorVersion = 2;
  attrs.alpha = true;
  attrs.powerPreference = EM_WEBGL_POWER_PREFERENCE_DEFAULT;
  
  // The following lines must be done in exact order, or it will break!
  emscripten_webgl_init_context_attributes(&attrs); // you MUST init the attributes before creating the context
  attrs.majorVersion = 3; // you MUST set the version AFTER the above line
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webgl_context = emscripten_webgl_create_context("#canvas", &attrs);
  emscripten_webgl_make_context_current(webgl_context);*/
  _init();

  #ifdef __NATIVE__
  _mainLoop();
  #else
  emscripten_set_main_loop(_mainLoopEMCC, -1, 1);
  #endif

  _cleanup();
}
