#include "Town.h"
#include "../special.h"

#define PAY_PER_SHIFT 120

static int _testForCollision(TownContext* context, COISprite* actorSprite, int changeX, int changeY) {
  int collisionResult = COI_NO_COLLISION;;
  int newX = actorSprite->_x + changeX;
  int newY = actorSprite->_y + changeY;
  int xGridPos = newX / COIBOARD_GRID_SIZE;
  int yGridPos = newY / COIBOARD_GRID_SIZE;
  int index = yGridPos * context->board->spriteGridWidth + xGridPos;
  COISprite* collidingSprite = context->board->spriteGrid[index];
  if (collidingSprite) { // NULL if no sprite is there
    if (collidingSprite->_extraCollision) {
      collisionResult = collidingSprite->_extraCollision->returnValue;
    } else {
      collisionResult = COI_COLLISION;
    }
    if (collisionResult != COI_NO_COLLISION) {
      return collisionResult;
    }
  }

  LinkedListResetCursor(context->allActors);
  Actor* currentActor = (Actor*)LinkedListNext(context->allActors);
  while (currentActor) {
    COISprite* theSprite = currentActor->sprite;
    if (theSprite != actorSprite) {
      collisionResult = COISpriteCollision(theSprite,
					 actorSprite->_x + changeX,
					 actorSprite->_y + changeY,
					 actorSprite->_width,
					 actorSprite->_height);
      if (collisionResult != COI_NO_COLLISION) {
        return collisionResult;
      }
    }

    currentActor = (Actor*)LinkedListNext(context->allActors);
  }


  // Check for collision against player
  COISprite* currentSprite = context->pInfo->party[0]->sprite;
  if (currentSprite != actorSprite) {
    collisionResult = COISpriteCollision(currentSprite,
					 actorSprite->_x + changeX,
					 actorSprite->_y + changeY,
					 actorSprite->_width,
					 actorSprite->_height);
  }

  return collisionResult;
}

// Need to make only run when tentacles in viewframe.
// Move to COISprite. Add "isAnimated" field in COISprite
// CBB can flag sprites as animated
static void _animateTentacles(TownContext* context) {
  LinkedListResetCursor(context->topTentacles);
  COISprite* tentacle = (COISprite*)LinkedListNext(context->topTentacles);
  while (tentacle) {
    tentacle->_animationTicks++;
    if (tentacle->_animationTicks > TOWN_TENTACLE_MOVEMENT_TICKS) {
      // JNW: cleanup - pull this behavior out into a function
      // in COISprite
      int oldCol = tentacle->_srcRect->x / tentacle->_srcRect->w;
      COISpriteSetSheetIndex(tentacle, 0, (oldCol + 1) % 3);
      tentacle->_animationTicks = 0;
      COIBoardAdjustSprite(context->board, tentacle);
      if (tentacle->_visible) {
	      COIBoardQueueDraw(context->board);
      }
    }

    tentacle = (COISprite*)LinkedListNext(context->topTentacles);
  }
}

// True when coord is immediately north, south, east, or west of player sprite
static bool _actorWillMoveNextToPlayer(Actor* actor, Actor* player, int direction) {
  int x, y;
  switch (direction) {
  case MOVING_LEFT:
    x = actor->sprite->_x - 1;
    y = actor->sprite->_y;
    break;
  case MOVING_RIGHT:
    x = actor->sprite->_x + 1;
    y = actor->sprite->_y;
    break;
  case MOVING_UP:
    x = actor->sprite->_x;
    y = actor->sprite->_y - 1;
    break;
  case MOVING_DOWN:
    x = actor->sprite->_x;
    y = actor->sprite->_y + 1;
    break;
  default:
    x = -1;
    y = -1;
  }

  if (x == player->sprite->_x && ABS_DIFF(y, player->sprite->_y) <= 1) {
    return true;
  } else if (y == player->sprite->_y && ABS_DIFF(x, player->sprite->_x) <= 1) {
    return true;
  }
  return false;
}

static Actor* _facingNPC(Actor* player, LinkedList* actors) {
  int facingX = player->sprite->_x;
  int facingY = player->sprite->_y;
  switch (player->_spriteSheetRow) {
  case 0:
    facingX += COIBOARD_GRID_SIZE;
    break;
  case 1:
    facingX -= COIBOARD_GRID_SIZE;
    break;
  case 2:
    facingY -= COIBOARD_GRID_SIZE;
    break;
  case 3:
    facingY += COIBOARD_GRID_SIZE;
    break;
  default:
    printf("Problem when checking player direction.\n");
    break;
  }
  LinkedListResetCursor(actors);
  Actor* currentActor = LinkedListNext(actors);
  while (currentActor) {
    if (currentActor->sprite->_x == facingX &&
        currentActor->sprite->_y == facingY) {
        return currentActor;
    } 

    currentActor = LinkedListNext(actors);
  }

  return NULL;
}

static void _createSwordChest(TownContext* context) {
  if (context->swordChest) {
    LinkedListRemove(context->allActors, (void*)context->swordChest);
    COIBoardRemoveDynamicSprite(context->board, context->swordChest->sprite, 0);
    COISpriteDestroy(context->swordChest->sprite);
    actorDestroy(context->swordChest);
  }

  if (context->pInfo->foundMythicalSword) {
    context->swordChest = actorCreateOfType(ACTOR_CHEST_OPEN,
              3008,
              480,
              COI_GLOBAL_LOADER,
              COI_GLOBAL_WINDOW);
  } else {
    context->swordChest = actorCreateOfType(ACTOR_CHEST,
              3008,
              480,
              COI_GLOBAL_LOADER,
              COI_GLOBAL_WINDOW);
  }
  COIBoardAddDynamicSprite(context->board, context->swordChest->sprite, 0);
  LinkedListResetCursor(context->allActors);
  LinkedListAdd(context->allActors, (void*)context->swordChest);
}

static void _createNPCs(TownContext* context) {
  context->allActors = LinkedListCreate();

  int xPositions[TOWN_NUM_NPC_CITIZENS] = { 1408, 2304, 1568, 1600, 2080, 2016, 2976, 2080, 1568, 2912, 2048, 672, 1696, 3840, 1056 };
  int yPositions[TOWN_NUM_NPC_CITIZENS] = { 1856, 4352, 3968, 4256, 2144, 1888, 3232, 1120, 3392, 1760, 2464, 4096, 2368, 2112, 4576 };
  int actorIDs[TOWN_NUM_NPC_CITIZENS] = { ACTOR_TIME_GUY, ACTOR_TOWNS_GUY, ACTOR_ENCOURAGEMENT_GUY, ACTOR_LANDLORD, ACTOR_MERCHANT, ACTOR_TAGNESSE_GUY, ACTOR_TREE_GUY, ACTOR_HAVONVALE_GUY, ACTOR_CHESTS_GUY,
                                          ACTOR_MONSTER_COUNT_GUY, ACTOR_GEM_OF_TIME_GUY, ACTOR_CLASS_GUY, ACTOR_POTION_GUY, ACTOR_FLEE_GUY, ACTOR_PARTY_GUY };

  for (int i = 0; i < TOWN_NUM_NPC_CITIZENS; i++) {
    context->npcs[i] = actorCreateOfType(actorIDs[i],
                xPositions[i],
                yPositions[i],
                COI_GLOBAL_LOADER,
                COI_GLOBAL_WINDOW);
    COIBoardAddDynamicSprite(context->board, context->npcs[i]->sprite, 0);
    actorFaceRight(context->npcs[i]);
  }
  _createSwordChest(context);

  for (int i = 0; i < TOWN_NUM_NPC_CITIZENS; i++) {
    LinkedListAdd(context->allActors, (void*)context->npcs[i]);
  }
}


COIBoard* townCreateBoard(COIWindow* window, COIAssetLoader* loader, PlayerInfo* pInfo) {
  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 5000, 5000, loader);
  TownContext* context = malloc(sizeof(TownContext));

  // The last corruption phase can block the player out if they're not in the right place
  bool playerInTownCenter = pInfo->party[0]->sprite->_x >= 1504 && pInfo->party[0]->sprite->_y >= 3008 && pInfo->party[0]->sprite->_y <= 4288;
  COISprite* playerSprite = pInfo->party[0]->sprite;

  // If we're right outside the rent house
  if (!pInfo->creepySpritemap && GLOBAL_TIME.day >= CREEPY_SPRITEMAP_DAYS && playerSprite->_x == 1504 && playerSprite->_y == 4288) {
    board->_bgColor[0] = 0;
    board->_bgColor[1] = 0;
    board->_bgColor[2] = 0;
    COIBoardLoadSpriteMap(board, COIWindowGetRenderer(window), "src/threadtown/spritemap_creepy.dat");
    context->_creepy = true;
    COISpriteSetPos(pInfo->party[0]->sprite, 544, 1024);
    pInfo->creepySpritemap = true;
  } else {
    char fullFilename[MAX_STRING_SIZE];
    char flattenedPathExtension[10];
    COISprite* playerSprite = pInfo->party[0]->sprite;
    bool outsideLowerGrassSection = playerSprite->_y <= 2944 || playerSprite->_y >= 3968;
    if (pInfo->shiftsWorked >= GRASS_PATH_SHIFTS_WORKED) {
      if (!pInfo->pathRevealed && !outsideLowerGrassSection) {
        snprintf(flattenedPathExtension, 10, "");
      } else {
        snprintf(flattenedPathExtension, 10, "_path");
        pInfo->pathRevealed = true;
      }
    } else {
      snprintf(flattenedPathExtension, 10, "");
    }
    if (GLOBAL_TIME.day >= CORRUPTION_4_DAYS && playerInTownCenter) {
      snprintf(fullFilename, MAX_STRING_SIZE, "src/threadtown/spritemap_corrupt_4%s.dat", flattenedPathExtension);
    } else if (GLOBAL_TIME.day >= CORRUPTION_3_DAYS) {
      snprintf(fullFilename, MAX_STRING_SIZE, "src/threadtown/spritemap_corrupt_3%s.dat", flattenedPathExtension);
    } else if (GLOBAL_TIME.day >= CORRUPTION_2_DAYS) {
      snprintf(fullFilename, MAX_STRING_SIZE, "src/threadtown/spritemap_corrupt_2%s.dat", flattenedPathExtension);
    } else if (GLOBAL_TIME.day >= CORRUPTION_1_DAYS) {
      snprintf(fullFilename, MAX_STRING_SIZE, "src/threadtown/spritemap_corrupt_1%s.dat", flattenedPathExtension);
    } else {
      snprintf(fullFilename, MAX_STRING_SIZE, "src/threadtown/spritemap%s.dat", flattenedPathExtension);
    }
    COIBoardLoadSpriteMap(board, COIWindowGetRenderer(window), fullFilename);
    context->_creepy = false;
  }
  context->pInfo = pInfo;
  context->direction = MOVING_NONE;
  context->terrain = TT_SAFE;
  context->board = board;
  context->willEnterBattle = false;
  context->_npcTicks = 0;
  context->swordChest = NULL;
  context->textType = COITextTypeCreate(15, 255, 255, 255, COIWindowGetRenderer(window));
  
  
  // Only 1 persistent sprite: the player
  COISprite** perSprites = actorGetSpriteList(context->pInfo->party, 1);
  COIBoardSetPersistentSprites(board, perSprites, 1);

  Actor* player = context->pInfo->party[0];
  // Adjust board player is in the center of the screen.
  int playerCenterX = player->sprite->_x - board->_frameX + (player->sprite->_width / 2);
  COIBoardShiftFrameX(board, playerCenterX - WINDOW_BASE_WIDTH / 2);
  int playerCenterY = player->sprite->_y - board->_frameY + (player->sprite->_height / 2);
  COIBoardShiftFrameY(board, playerCenterY - WINDOW_BASE_HEIGHT / 2);

  _createNPCs(context);
  context->talkingActorType = ACTOR_NONE;
  
  // Yes/No dialog
  COISprite* frame = COISpriteCreateFromAssetID(70, 250, 100, 60,
						COI_GLOBAL_LOADER,
						5,
						COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  frame->_autoHandle = false;
  COIBoardAddDynamicSprite(context->board, frame, 0);
  COISprite* pointer = COISpriteCreateFromAssetID(0, 0, 32, 32,
						  COI_GLOBAL_LOADER,
						  6,
						  COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  pointer->_autoHandle = false;
  COIBoardAddDynamicSprite(context->board, pointer, 0);
  context->confirmMenu = COIMenuCreateWithCapacity(frame, pointer, BACKPACK_SIZE);
  COIString* yes = COIStringCreate("Yes", 0, 0, context->textType);
  COIBoardAddString(context->board, yes, 0);
  COIString* no = COIStringCreate("No", 0, 0, context->textType);
  COIBoardAddString(context->board, no, 0);
  COIMenuAddString(context->confirmMenu, yes, 0);
  COIMenuAddString(context->confirmMenu, no, 1);
  COIMenuSetInvisible(context->confirmMenu);

  context->pauseOverlay = PauseOverlayCreate(pInfo, context->textType, context->board);

  context->textBox = TextBoxCreate(context->board, context->textType);

  if (!pInfo->corruptionHasSpread && GLOBAL_TIME.day >= CORRUPTION_1_DAYS) {
    TextBoxSetStrings(context->textBox,
        "The corruption has spread.",
        "A cold pang of anxiety spreads through your body.",
        "You sense that some looming presence will soon make itself known in this town.",
        "You need to act fast.",
        NULL);
    pInfo->corruptionHasSpread = true;
  }

  // Gather animated tentacle sprites
  context->topTentacles = LinkedListCreate();
  int startingCol = 0;
  int numSprites = board->spriteGridHeight * board->spriteGridWidth;
  for (int i = 0; i < numSprites; i++) {
    if (board->spriteGrid[i] && board->spriteGrid[i]->_assetID == 21) {
      COISpriteSetSheetIndex(board->spriteGrid[i], 0, startingCol);
      LinkedListAdd(context->topTentacles, (void*)board->spriteGrid[i]);
      startingCol = (startingCol + 1) % 3;
    }
  }

  COIBoardSetContext(board, (void*)context);

  return board;
}

// If we're not currently in motion, start moving in the provided direction.
// Otherwise, just set which direction we want to move next.
static void _queueMovement(TownContext* context, Actor* actor, int direction, int speed) {
  if (actor->movementDirection == MOVING_NONE) {
    actor->_stepsLeft = COIBOARD_GRID_SIZE;
    actor->movementDirection = direction;
    actor->nextMovementDirection = direction;
    actor->_speed = speed;
    actorFaceDirection(actor, direction);
    if (actor->sprite->_visible) {
      COIBoardQueueDraw(context->board);
    }
  } else {
    actor->nextMovementDirection = direction;
  }
}

// Continue moving in current direction while we still have steps left to go.
// Return true when movement has completed.
bool townContinueMovement(Actor* actor, COIBoard* board) {
  int xOffset, yOffset;
  switch (actor->movementDirection) {
  case MOVING_LEFT:
    xOffset = actor->_speed * -1;
    yOffset = 0;
    break;
  case MOVING_RIGHT:
    xOffset = actor->_speed;
    yOffset = 0;
    break;
  case MOVING_UP:
    xOffset = 0;
    yOffset = actor->_speed * -1;
    break;
  case MOVING_DOWN:
    xOffset = 0;
    yOffset = actor->_speed;
    break;
  default:
    return false;
  }

  actorMove(actor, xOffset, yOffset, board);

  actor->_stepsLeft -= actor->_speed;
  if (actor->_stepsLeft <= 0) {
    if (actor->nextMovementDirection == MOVING_NONE) {
      actorStandStill(actor);
    } else {
      actor->_stepsLeft = COIBOARD_GRID_SIZE;
    }
    actor->movementDirection = actor->nextMovementDirection;

    return true;
  }

  return false;
}


// After a certain amount of ticks, check if we should enter a battle (random encounter).
// The likelihood of entering a battle is based off of the current terrain.
void townCheckForBattle(TownContext* context) {
  switch (context->terrain) {
  case TT_THICK_GRASS_CORRUPT:
  case TT_BROWN_GRASS:
  case TT_BROWN_GRASS_CORRUPT:
  case TT_THICK_GRASS:
  #ifdef __COI_DEBUG__
    context->willEnterBattle = generateRandomBoolWeighted(0);
  #else
    context->willEnterBattle = generateRandomBoolWeighted(0.05);
  #endif
    break;
  case TT_TENTACLE:
    context->willEnterBattle = true;
    break;
  default:
    context->willEnterBattle = false;
  }
}

void townUpdateTerrain(TownContext* context, int collisionResult) {
  switch (collisionResult) {
  case THICK_GRASS:
    context->terrain = TT_THICK_GRASS;
    break;
  case BROWN_GRASS:
    context->terrain = TT_BROWN_GRASS;
    break;
  case THICK_GRASS_CORRUPT:
    context->terrain = TT_THICK_GRASS_CORRUPT;
    break;
  case BROWN_GRASS_CORRUPT:
    context->terrain = TT_BROWN_GRASS_CORRUPT;
    break;
  case TENTACLE:
    context->terrain = TT_TENTACLE;
    break;
  case COI_NO_COLLISION:
    context->terrain = TT_SAFE;
    break;
  default:
    break;
    // Do nothing
  }
}

// If we move to the next grid, what kind of collision do we get?
static int _getNextCollision(TownContext* context, Actor* actor, int direction) {
  int changeX = 0;
  int changeY = 0;
  switch (direction) {
  case MOVING_LEFT:
    changeX = COIBOARD_GRID_SIZE * -1;
    break;
 case MOVING_RIGHT:
    changeX = COIBOARD_GRID_SIZE;
    break;
  case MOVING_UP:
    changeY = COIBOARD_GRID_SIZE * -1;
    break;
  case MOVING_DOWN:
    changeY = COIBOARD_GRID_SIZE;
    break;
  default:
    return COI_NO_COLLISION;
  }
  return  _testForCollision(context, actor->sprite, changeX, changeY);
}

static void _talkToChestsGuy(TownContext* context) {
  if (GLOBAL_TIME.day > SA_DAYS_OLDEST) {
    TextBoxSetStrings(context->textBox,
                      "Sorry old timer, can't let you in here.",
                      "I'm guarding this gear for my buddy while he's away in Havonvale.",
                      "These are his prized possessions. He won't let anyone else touch them.",
                      "Haven't heard from him in a while though...",
                      NULL);
  } else if (GLOBAL_TIME.day > SA_DAYS_OLDER) {
    TextBoxSetStrings(context->textBox,
                      "Sorry pal, can't let you in here.",
                      "I'm guarding this gear for my buddy while he's away in Havonvale.",
                      "These are his prized possessions. He won't let anyone else touch them.",
                      "Haven't heard from him in a while though...",
                      NULL);
  } else {
    TextBoxSetStrings(context->textBox,
                  "Sorry kid, can't let you in here.",
                  "I'm guarding this gear for my buddy while he's away in Havonvale.",
                  "These are his prized possessions. He won't let anyone else touch them.",
                  "Haven't heard from him in a while though...",
                  NULL);
  }
}

static void _talkToLandlord(TownContext* context) {
  if (context->pInfo->renting == RS_RENTING) {
    unsigned long daysLeft = context->pInfo->nextRentDate - GLOBAL_TIME.day;
    if (daysLeft == 1) {
      TextBoxSetStrings(context->textBox,
        "Payment's due tomorrow.",
        NULL);
    } else if (daysLeft == 0) {
      TextBoxSetStrings(context->textBox,
        "Payment's due today.",
        NULL);
    } else {
      char temp[MAX_STRING_SIZE];
      snprintf(temp, MAX_STRING_SIZE, "Payment's due in %lu days.", context->pInfo->nextRentDate - GLOBAL_TIME.day);
      TextBoxSetStrings(context->textBox,
            temp,
            NULL);
    }
  } else if (context->pInfo->renting == RS_NOT_RENTING) {
    TextBoxSetStrings(context->textBox,
		      "I've got a spare room in my place. Interested? It's 1400 for two weeks.",
          "An adventurer needs a place to rest and recover, right?",
		      NULL);
  } else {
    TextBoxSetStrings(context->textBox,
          "Get out of my face, freeloader.",
          NULL);
  }
}

static void _talkToMerchant(TownContext* context) {
  if (context->pInfo->working) {
    TextBoxSetStrings(context->textBox,
		      "Ready to work?",
		      NULL);
  } else {
    TextBoxSetStrings(context->textBox,
          "I have an opening for a clerk in my shop. Need some extra cash?",
          NULL);
  }
}

static void _talkToTagnesseGuy(TownContext* context) {
  if (!context->pInfo->foundMythicalSword) {
    TextBoxSetStrings(context->textBox,
        "Want to hear a local legend?",
        "There's supposedly a magical shield that lies deep in the forest.",
        "Anyone who wields it can travel through time!",
        NULL);
  } else if (!inventoryHasItem(context->pInfo->inventory, ITEM_ID_TAGNESSE, ITEM_SLOT_OFFHAND)) {
    TextBoxSetStrings(context->textBox,
        "Oh...you sold Tagnesse?",
        "I guess if you need to.",
        NULL);
  } else {
    TextBoxSetStrings(context->textBox,
        "No way! I can't believe Tagnesse is real.",
        "You should head to Havonvale in the east. There's an adventurer like you seeking other strong warriors.",
        "Maybe he could join your party!",
        NULL);
  }
}

static void _talkToEncouragementGuy(TownContext* context) {
  if (GLOBAL_TIME.day > ENCOURAGEMENT_TEXT_2_DAYS) {
    TextBoxSetStrings(context->textBox,
        "Jesus, you're still here?",
        NULL);
  } else if (GLOBAL_TIME.day > ENCOURAGEMENT_TEXT_1_DAYS) {
    TextBoxSetStrings(context->textBox,
        "I told you. Check out Havonvale. It's not here.",
        NULL);
  } else {
    TextBoxSetStrings(context->textBox,
        "Huh, a 'corruption core'. Can't say I know what that is.",
        "There's been a lot of commotion around Havonvale though.",
        "Maybe you should check there?",
        NULL);
  }
}

static void _talkToNostalgicGuy(TownContext* context) {
  if (GLOBAL_TIME.day >= NOSTALGIC_TEXT_1_DAYS) {
    TextBoxSetStrings(context->textBox,
      "Ah, to be a young adventurer. Remember those days?",
    NULL);
  } else {
    TextBoxSetStrings(context->textBox,
      "Ah, to be a young adventurer. I remember those days.",
      "Enjoy them!",
      NULL);
  }
}

static void _talkToTownsGuy(TownContext* context) {
  TextBoxSetStrings(context->textBox,
    "This is Thread Town.",
    "To the northeast, there's Havonvale.",
    "If you head north from there, you'll find Denburg. It lies on the river.",
    "Follow that river and you'll end up in Starshire, the capital city!",
    NULL);
}

static void _talkToTimeGuy(TownContext* context) {
  char temp[MAX_STRING_SIZE];
  const char* daysString = GLOBAL_TIME.day == 1 ? "day" : "days";
  snprintf(temp, MAX_STRING_SIZE, "You've been here %lu %s!", GLOBAL_TIME.day, daysString);
  if (GLOBAL_TIME.day >= TIME_GUY_TEXT_1_DAYS) {
    // Playtime
    time_t endTime = time(NULL) - context->pInfo->startTime;
    time_t minutes = endTime / 60;
    time_t hours = minutes / 60;
    time_t minutesLeftOver = minutes - (hours * 60);

    unsigned int totalHours = (unsigned int)context->pInfo->hours + (unsigned int)hours;
    unsigned int totalMinutes = (unsigned int)context->pInfo->minutes + (unsigned int)minutesLeftOver;

    char tempRealTime[MAX_STRING_SIZE];
    const char* hoursString = totalHours == 1 ? "hour" : "hours";
    const char* minutesString = totalMinutes == 1 ? "minute" : "minutes";
    snprintf(tempRealTime, MAX_STRING_SIZE, "Actually, I think it's been %u %s and %u %s.", totalHours, hoursString, totalMinutes, minutesString);
    TextBoxSetStrings(context->textBox,
      temp,
      tempRealTime,
      NULL);
  } else {
    TextBoxSetStrings(context->textBox,
      temp,
      NULL);
  }
}

static void _talkToPartyGuy(TownContext* context) {
  TextBoxSetStrings(context->textBox,
    "The more enemies you fight, the more dangerous a battle is.",
    "If you want to succeed, you should find some people to join you.",
    "You can have up to five members in your party.",
    "A good party is balanced. Make sure to have a variety of roles.",
    NULL);
}

static void _talkToTreeGuy(TownContext* context) {
  if (GLOBAL_TIME.day >= TREE_GUY_CONVO_3_DAYS && context->pInfo->renting != RS_NOT_RENTING) {
    TextBoxSetStrings(context->textBox,
        "I've been watching you walk up this path every day for a while.",
        "I think you've given it your best shot.",
        "Maybe it's time to settle down and figure out what else you want to do.",
        "Time's finite, after all.",
        "Not all branches have to bear fruit.",
        NULL);
  } else if (GLOBAL_TIME.day >= TREE_GUY_CONVO_2_DAYS) {
    TextBoxSetStrings(context->textBox,
        "It's funny how a town moves over time.",
        "Different buildings go up. Other fall down. But there's always shops. Always homes.",
        "People change faces and bodies but otherwise stay the same.",
        "Overall, the town feels the same.",
        "But I'm different. Older, wiser, I guess.",
        "Definitely fatter. Heh.",
        "The town stays suspended in time, but I keep moving forward.",
        NULL);
  } else if (GLOBAL_TIME.day >= TREE_GUY_CONVO_1_DAYS) {
    TextBoxSetStrings(context->textBox,
        "There's an interesting quirk about this tree.",
        "Every fruit tastes different!",
        "When I was a kid, I'd try to guess what each fruit would taste like before picking which branch to climb.",
        "I was never be able to choose. I'd end up just sitting at the trunk all afternoon. Ha!",
        NULL);
  } else {
    TextBoxSetStrings(context->textBox,
        "You're a new face! Welcome to Thread Town.",
        NULL);
  }
}

static void _talkToHavonvaleGuy(TownContext* context) {
  if (GLOBAL_TIME.day >= HAVONVALE_TEXT_2_DAYS) {
    TextBoxSetStrings(context->textBox,
        "What do you mean you haven't gotten rid of them yet?",
        "Just work harder.",
        NULL);
  } else if (GLOBAL_TIME.day >= HAVONVALE_TEXT_1_DAYS) {
    TextBoxSetStrings(context->textBox,
        "Have you tried taking out those monsters yet?",
        "I tried myself. They're pretty strong.",
        "Train hard first. I believe in you!",
        NULL);
  } else {
    TextBoxSetStrings(context->textBox,
        "Havonvale is down through here.",
        "There's a group of monsters blocking the way, though. I'd be careful.",
        NULL);
  }
}

static void _talkToMonsterCountGuy(TownContext* context) {
  TextBoxSetStrings(context->textBox,
      "Be careful walking through the thick grass at night.",
      "Monsters come out in larger numbers.",
      NULL);
}

static void _talkToGemOfTimeGuy(TownContext* context) {
  if (!context->pInfo->breakFromWorking && context->pInfo->shiftsWorked >= BREAK_FROM_WORKING_SHIFTS_WORKED) {

    context->pInfo->breakFromWorking = true;
    TextBoxSetStrings(context->textBox,
        "Want to take a break from working all the time?",
        "Stop by the general store to pick up a Gem of Time.",
        NULL);
  } else {
    TextBoxSetStrings(context->textBox,
        "Want to take a break from your adventure for a while?",
        "Stop by the general store to pick up a Gem of Time.",
        NULL);
  }
}

static void _talkToClassGuy(TownContext* context) {
  if (context->pInfo->shiftsWorked >= CLERK_CLASS_CHANGE_SHIFTS_WORKED) {
    TextBoxSetStrings(context->textBox,
        "Oh, you're a Clerk?",
        "Yeah, me too.",
        NULL);
  } else {
    // char* className = playerGetClass(context->pInfo);
    if (context->pInfo->class == PLAYER_CLASS_FIGHTER) {
      TextBoxSetStrings(context->textBox,
          "Oh, you're a Fighter?",
          "You must be pretty strong!",
          "You can eventually become a Ranger, a Barbarian, a Blademaster, a Duelist, or a MageFist!",
          NULL);
    } else if (context->pInfo->class == PLAYER_CLASS_ROGUE) {
      TextBoxSetStrings(context->textBox,
          "Oh, you're a Rogue?",
          "You're probably really nimble!",
          "You can eventually become a Thief, a Swashbuckler, an Assassin, a Duelist, or a Spellsword!",
          NULL);
    } else if (context->pInfo->class == PLAYER_CLASS_WIZARD) {
      TextBoxSetStrings(context->textBox,
          "Oh, you're a Mage?",
          "You've got to be really smart!",
          "You can eventually become a Healer, an Enchanter, an Archmage, a MageFist, or a Spellsword!",
          NULL);   
    }
  }
}

static void _talkToPotionGuy(TownContext* context) {
  TextBoxSetStrings(context->textBox,
      "There's a potion shop through the forest. It's north of here.",
      NULL);   
}

static void _talkToFleeGuy(TownContext* context) {
  TextBoxSetStrings(context->textBox,
      "Here's a tip from my fighting days.",
      "If you want to flee a fight, you're more likely to succeed if you're faster than your opponent.",
      "Of course, I never needed to do that...",
      NULL);
}

static void _ominousMessage(TownContext* context) {
  TextBoxSetStrings(context->textBox,
      "I think you made a mistake coming here.",
      NULL);
}

static void _visitingMessage(TownContext* context) {
  char temp[MAX_STRING_SIZE];
  snprintf(temp, MAX_STRING_SIZE, "Huh, you've been here %lu days.", GLOBAL_TIME.day);
  TextBoxSetStrings(context->textBox,
      temp,
      "I thought you were just passing through.",
      NULL);
}

static void _mockingMessage(TownContext* context) {
  TextBoxSetStrings(context->textBox,
      "Hey pal!",
      "How'd you enjoy your first couple weeks here?",
      NULL);
}

static void _talkToChest(TownContext* context) {
  TextBoxSetStrings(context->textBox,
      "Unbelievable!",
      "You found Tagnesse, the mythical shield of Thread Town!",
      NULL);
  context->pInfo->foundMythicalSword = true;
  _createSwordChest(context); // Change to 'opened' sprite

  // We need to delete and recreate the overlay because we're need it to be on top of everything 
  // (and so its sprites must be the last ones in the list).
  // It'd be better if our sprites just had a notion of a Z-index.
  PauseOverlayDestroy(context->pauseOverlay, context->board);
  context->pauseOverlay = PauseOverlayCreate(context->pInfo, context->textType, context->board);

  inventoryAddItem(context->pInfo->inventory, ITEM_ID_TAGNESSE);
  context->pauseOverlay->dirty = true;
}

static void _confirmMenuSelect(TownContext* context) {
  COISoundPlay(COI_SOUND_SELECT);
  TextBoxNextString(context->textBox);
  bool choseYes = COIMenuGetCurrentValue(context->confirmMenu) == 0;
  if (choseYes) {
    switch (context->talkingActorType) {
    case ACTOR_LANDLORD:
      context->pInfo->renting = RS_RENTING;
      _talkToLandlord(context);
      break;
    case ACTOR_MERCHANT:
      if (context->pInfo->working) {
	      TimeStateIncrement(12);
        context->pInfo->shiftsWorked++;
        context->pInfo->inventory->money = (MAX_MONEY, context->pInfo->inventory->money + PAY_PER_SHIFT);
        context->pauseOverlay->dirty = true; // Reset gold counter
        playerCheckForEviction(context->pInfo);
	      COITransitionInit(&COI_GLOBAL_WINDOW->transition,
			      COI_TRANSITION_ENCLOSE,
			      COI_GLOBAL_WINDOW);
        townApplyTimeChanges(context);
      } else {
	      context->pInfo->working = true;
        context->talkingActorType = ACTOR_MERCHANT;
        TextBoxSetStrings(context->textBox,
                          "Talk to me when you're ready to work a shift.",
                          NULL);
      }
      break;
    default:
      printf("Error: confirm menu appeared for invalid actor type\n");
    }
  }
  COIMenuSetInvisible(context->confirmMenu);
  COIBoardQueueDraw(context->board);
  context->talkingActorType = ACTOR_NONE;
}

static bool _shouldPromptForAnswer(TownContext* context) {
  if (context->talkingActorType == ACTOR_LANDLORD &&
      context->pInfo->renting == RS_NOT_RENTING &&
      context->textBox->currentStringDone) {
    return true;
  } else if (context->talkingActorType == ACTOR_MERCHANT &&
	    context->textBox->currentStringDone) {
    return true;
  }
  return false;
}

static bool _npcCanMove(Actor* npc) {
  switch (npc->actorType) {
    case ACTOR_LANDLORD:
    case ACTOR_MERCHANT:
    case ACTOR_CHESTS_GUY:
      return false;
    default:
      return true;
  }
}

void townProcessDirectionalInput(TownContext* context, int direction) {
  if (context->pauseOverlay->visible) {
    PauseOverlayProcessInput(context->pauseOverlay, direction);
    COIBoardQueueDraw(context->board);
  } else if (context->confirmMenu->_frame->_visible) {
    COIMenuHandleInput(context->confirmMenu, direction, true);
    COIBoardQueueDraw(context->board);
  } else if (!context->textBox->box->_visible && inputIsDirection(direction)) {
    Actor* player = context->pInfo->party[0];
    _queueMovement(context, player, direction, TOWN_MOVE_SPEED);
  }
}

static bool _genericGagText(TownContext* context) {
  bool talkingToPerson = context->talkingActorType != ACTOR_CHEST && context->talkingActorType != ACTOR_CHEST_OPEN;
  if (context->pInfo->welcomeHome && talkingToPerson) {
    TextBoxSetStrings(context->textBox,
        "Welcome home.",
        NULL); 
    return true;
  } else if (!context->pInfo->visitingMessage && GLOBAL_TIME.day >= LEAVING_THIS_PLACE_DAYS) {
    TextBoxSetStrings(context->textBox,
        "At one point, I planned on leaving this place.",
        NULL); 
    context->pInfo->leavingThisPlace = true;
  } else if (!context->pInfo->visitingMessage && GLOBAL_TIME.day >= GOTTEN_FATTER_DAYS) {
    TextBoxSetStrings(context->textBox,
        "Woof. You've gotten fat.",
        NULL); 
    context->pInfo->gottenFatter = true;
  } else if (!context->pInfo->visitingMessage && GLOBAL_TIME.day >= VISITING_MESSAGE_DAYS &&
      talkingToPerson) {
    _visitingMessage(context);
    context->pInfo->visitingMessage = true;
    return true;
  } else if (!context->pInfo->feltDifferent && GLOBAL_TIME.day >= FELT_DIFFERENT_DAYS && talkingToPerson) {
    TextBoxSetStrings(context->textBox,
        "Have you felt...different, recently?",
        NULL); 
    context->pInfo->feltDifferent = true;
    return true;
  } else if (!context->pInfo->mockingMessage && GLOBAL_TIME.day >= MOCKING_MESSAGE_DAYS &&
      talkingToPerson) {
    _mockingMessage(context);
    context->pInfo->mockingMessage = true;
    return true;
  } 
  else if (!context->pInfo->ominousMessage && GLOBAL_TIME.day >= OMINOUS_MESSAGE_DAYS &&
      talkingToPerson) {
    _ominousMessage(context);
    context->pInfo->ominousMessage = true;
    return true;
  }

  return false;
}

void townProcessSelectionInput(TownContext* context) {
  Actor* player = context->pInfo->party[0];
  if (context->pauseOverlay->visible) {
    PauseOverlaySelect(context->pauseOverlay, context->textBox);
    COIBoardQueueDraw(context->board);
  } else if (context->confirmMenu->_frame->_visible) {
    _confirmMenuSelect(context);
  } else if (context->textBox->box->_visible) {
    if (_shouldPromptForAnswer(context)) {
      COIMenuSetVisible(context->confirmMenu);
    } else {
      // Advance text box
      TextBoxNextString(context->textBox);
    }
    COIBoardQueueDraw(context->board);
  } else {
    Actor* talkingNPC = _facingNPC(player, context->allActors);
    if (talkingNPC != NULL) {
      actorMeetGaze(talkingNPC, player);
      context->talkingActorType = talkingNPC->actorType;
      
      if (_genericGagText(context)) {
        context->talkingActorType = ACTOR_CHAGGAI;
      } else {
        switch (context->talkingActorType) {
        case ACTOR_LANDLORD:
          _talkToLandlord(context);
          break;
        case ACTOR_CHESTS_GUY:
          _talkToChestsGuy(context);
          break;
        case ACTOR_MERCHANT:
          _talkToMerchant(context);
          break;
        case ACTOR_CHEST_OPEN:
          TextBoxSetStrings(context->textBox,
              "Unbelievable!",
              "It's empty.",
              NULL);
          break;
        case ACTOR_CHEST:
          _talkToChest(context);
          break;
        case ACTOR_TAGNESSE_GUY:
          _talkToTagnesseGuy(context);
          break;
        case ACTOR_TREE_GUY:
          _talkToTreeGuy(context);
          break;
        case ACTOR_HAVONVALE_GUY:
          _talkToHavonvaleGuy(context);
          break;
        case ACTOR_GEM_OF_TIME_GUY:
          _talkToGemOfTimeGuy(context);
          break;
        case ACTOR_MONSTER_COUNT_GUY:
          _talkToMonsterCountGuy(context);
          break;
        case ACTOR_CLASS_GUY:
          _talkToClassGuy(context);
          break;
        case ACTOR_POTION_GUY:
          _talkToPotionGuy(context);
          break;
        case ACTOR_FLEE_GUY:
          _talkToFleeGuy(context);
          break;
        case ACTOR_ENCOURAGEMENT_GUY:
          _talkToEncouragementGuy(context);
          break;
        case ACTOR_TOWNS_GUY:
          _talkToTownsGuy(context);
          break;
        case ACTOR_NOSTALGIC_GUY:
          _talkToNostalgicGuy(context);
          break;
        case ACTOR_TIME_GUY:
          _talkToTimeGuy(context);
          break;
        case ACTOR_PARTY_GUY:
          _talkToPartyGuy(context);
          break;
        default:
          TextBoxSetStrings(context->textBox,
              "I saw something scary in the northeast.",
              "I'd take shelter if I were you.",
              NULL);
        }
      }
      COIBoardQueueDraw(context->board);
    }
  }
}

void townProcessCollisionType(TownContext* context, int collision) {
  Actor* player = context->pInfo->party[0];
  switch (collision) {
  case COI_COLLISION:
    actorFaceDirection(player, player->movementDirection);
    player->movementDirection = MOVING_NONE;
    actorStandStill(player);
    break;
  default:
    townUpdateTerrain(context, collision);
    break;
  }
}

// When we're about to move to another square (but haven't started moving yet),
// we want to check if there's a collision.
bool townShouldCheckForCollision(TownContext* context) {
  Actor* player = context->pInfo->party[0];
  return (player->movementDirection != MOVING_NONE &&
	  player->sprite->_x % COIBOARD_GRID_SIZE == 0 &&
	  player->sprite->_y % COIBOARD_GRID_SIZE == 0);
}


int townCheckForCollision(TownContext* context, Actor* actor) {
  // We haven't started moving from our current square yet if the x and y of the sprite
  // are cleanly divisible by the size of a grid square.
  if (actor->sprite->_x % COIBOARD_GRID_SIZE == 0 &&
      actor->sprite->_y % COIBOARD_GRID_SIZE == 0) {
    return _getNextCollision(context, actor, actor->movementDirection);
  }

  return COI_NO_COLLISION;
}

void townTick(TownContext* context) {
  // Text box
  if (context->textBox->box->_visible && !context->textBox->currentStringDone) {
    TextBoxAnimate(context->textBox);
    COIBoardQueueDraw(context->board);
  }

  // NPCs
  if (!context->textBox->box->_visible) {
    if (context->_npcTicks >= TOWN_NPC_MOVEMENT_TICKS) {
      for (int i = 0; i < TOWN_NUM_NPC_CITIZENS; i++) {
	// 30% chance they move
	if (_npcCanMove(context->npcs[i]) && generateRandomBoolWeighted(0.3) && !context->_creepy) {
	  // If we do move, pick 1 of the 4 directions
    int movement = generateRandomDirectionalMovement();

    // Fix to a problem where npc will move through player if both try to move into the same square at the same time.
    // Just don't allow movement into a square directly next to the player.
    if (!_actorWillMoveNextToPlayer(context->npcs[i], context->pInfo->party[0], movement)) {
	    _queueMovement(context, context->npcs[i], generateRandomDirectionalMovement(), TOWN_MOVE_SPEED);

      // Cancel movement if there's a collision
      if (townCheckForCollision(context, context->npcs[i]) != COI_NO_COLLISION) {
        context->npcs[i]->movementDirection = MOVING_NONE;
      }
    }
	} else {
	  //	_queueMovement(context, context->npcs[i], MOVING_NONE, TOWN_MOVE_SPEED);
	}
      }
      context->_npcTicks = 0;
    } else {
      context->_npcTicks++;
    }
  }

  // Animated sprites
  _animateTentacles(context);

}

void townMoveNPCs(TownContext* context) {
  for (int i = 0; i < TOWN_NUM_NPC_CITIZENS; i++) {
    Actor* npc = context->npcs[i];
    bool inNextGridCell = townContinueMovement(npc, context->board);
    if (inNextGridCell) {
      npc->nextMovementDirection = MOVING_NONE;
      npc->movementDirection = MOVING_NONE;
      actorStandStill(npc);
    }
  }
}

void townMovePlayer(TownContext* context) {
  Actor* player = context->pInfo->party[0];
  COIBoard* board = context->board;
  int playerCenterX, playerCenterY;
  bool inNextGridCell = false;
  switch (player->movementDirection) {
  case MOVING_LEFT:
    inNextGridCell = townContinueMovement(player, context->board);
    playerCenterX = player->sprite->_x - board->_frameX + (player->sprite->_width / 2);
    if (playerCenterX <= board->_frameWidth / 2) {
      COIBoardShiftFrameX(board, -1 * TOWN_MOVE_SPEED);
    }
    if (inNextGridCell && player->sprite->_x == TOWN_MUSIC_X_TRIGGER) {
      COISoundPlay(COI_SOUND_THREADTOWN);
    }
    break;
  case MOVING_RIGHT:
    inNextGridCell = townContinueMovement(player, context->board);
    playerCenterX = player->sprite->_x - board->_frameX + (player->sprite->_width / 2);
    if (playerCenterX >= board->_frameWidth / 2) {
      COIBoardShiftFrameX(board, TOWN_MOVE_SPEED);
    }
    if (inNextGridCell && player->sprite->_x == TOWN_MUSIC_X_TRIGGER) {
      COISoundPlay(COI_SOUND_FAIRY);
    }
    break;
  case MOVING_UP:
    inNextGridCell = townContinueMovement(player, context->board);
    playerCenterY = player->sprite->_y - board->_frameY + (player->sprite->_height / 2);
    if (playerCenterY <= board->_frameHeight / 2) {
      COIBoardShiftFrameY(board, -1 * TOWN_MOVE_SPEED);
    }
    break;
  case MOVING_DOWN:
    inNextGridCell = townContinueMovement(player, context->board);
    playerCenterY = player->sprite->_y - board->_frameY + (player->sprite->_height / 2);
    if (playerCenterY >= board->_frameHeight / 2) {
      COIBoardShiftFrameY(board, TOWN_MOVE_SPEED);
    }
    break;
  default:
    break;
  }

#ifdef __COI_DEBUG__
  printf("coords: x = %i, y = %i\n", player->sprite->_x, player->sprite->_y);
#endif

  // Position trigger for tree guy callout
  if (!context->pInfo->treeGuyCallout && GLOBAL_TIME.day >= TREE_GUY_CONVO_3_DAYS && 
      (player->sprite->_x >= 2688 && player->sprite->_x <= 2976 && player->sprite->_y <= 3392 && player->sprite->_y >= 3232)) {
      TextBoxSetStrings(context->textBox,
          "'Hey.'",
          "The man by the tree calls out to you.",
          NULL);
      context->pInfo->treeGuyCallout = true;
      player->movementDirection = MOVING_NONE;
      return;
  }
  
  if (inNextGridCell) {
    townCheckForBattle(context);
  }

  if (context->willEnterBattle) {
    // Readjust pause overlay after battle. HP, SP, etc. will have changed
    context->pauseOverlay->dirty = true;
    player->nextMovementDirection = MOVING_NONE;
    player->movementDirection = MOVING_NONE;
    COITransitionInit(&COI_GLOBAL_WINDOW->transition,
		      COI_TRANSITION_SNAKE,
		      COI_GLOBAL_WINDOW);
    COISoundPlay(COI_SOUND_BATTLE);
  }
}

void townTogglePauseOverlay(TownContext* context) {
  Actor* player = context->pInfo->party[0];
  if (player->movementDirection == MOVING_NONE && !context->confirmMenu->_frame->_visible && !context->textBox->box->_visible) {
    PauseOverlayToggleVisibility(context->pauseOverlay);
    COIBoardQueueDraw(context->board);
  }
}

// Depending on the time, we should change the layout.
// Ex: The merchant should only appear during the day.
// Run this whenever we change the active board to the town.
void townApplyTimeChanges(TownContext* context) {
  // COIBoardUpdateBGColor(context->board);

  if (GLOBAL_TIME.phase < TS_EVENING) {
    COISpriteSetPos(context->npcs[4]->sprite, 2080, 2144);
  } else {
    // Move merchant off screen.
    COISpriteSetPos(context->npcs[4]->sprite, -50, -50);
  }

  if (GLOBAL_TIME.day - context->pInfo->lastXPGain.day > LAST_XP_GAIN_DAYS &&
      context->pInfo->level > 1) {
    int numAbilities = context->pInfo->party[0]->specials.length + context->pInfo->party[0]->techList->count;
    if (numAbilities > 0) { // We're going to lose an ability when we level down.
      TextBoxSetStrings(context->textBox,
        "You have not increased your level in some time. Your skills are beginning to atrophy.",
        "You have lost a level.",
        "You have forgotten one of your abilities.",
        NULL);
    } else {
      TextBoxSetStrings(context->textBox,
        "You have not increased your level in some time. Your skills are beginning to atrophy.",
        "You have lost a level.",
        NULL);
    }
    playerLevelDown(context->pInfo);
    // Reflect this change in the pause overlay
    PauseOverlayDestroy(context->pauseOverlay, context->board);
    context->pauseOverlay = PauseOverlayCreate(context->pInfo, context->textType, context->board);
  } else if (!context->pInfo->asNightFalls && GLOBAL_TIME.phase >= TS_EVENING) {
      TextBoxSetStrings(context->textBox,
        "As night falls, the air feels cold.",
        "A miserable, forlorn aura fills the dark night sky.",
        "Undoubtedly, Izra has infected this small town.",
        NULL);
      context->pInfo->asNightFalls = true;
  }

  if (GLOBAL_TIME.day >= WELCOME_HOME_DAYS) {
    context->pInfo->welcomeHome = true;
  }

  // Change player's sprite
  SpriteAge nextSpriteAge = playerSpriteAgeFromGlobalTime();
  if (nextSpriteAge != context->pInfo->spriteAge) {
    context->pInfo->spriteAge = nextSpriteAge;
    actorChangeSprite(context->pInfo->party[0], playerSpriteIndexFromSpriteAge(context->pInfo->spriteAge));
    COISprite** perSprites = actorGetSpriteList(context->pInfo->party, context->pInfo->partySize);
    COIBoardSetPersistentSprites(context->board, perSprites, 1);
  }

  if (!context->pInfo->agiReduced && nextSpriteAge == SA_OLDEST) {
    context->pInfo->party[0]->agi.base /= 2;
    context->pInfo->agiReduced = true;
  }
}

void townCheckForTiredMessage(TownContext* context) {
  if (!context->pInfo->youFeelTired && GLOBAL_TIME.day >= TIRED_MESSAGE_DAYS) {
    TextBoxSetStrings(context->textBox,
            "For some reason, you feel incredibly tired.",
            NULL);
    context->pInfo->youFeelTired = true;
  }
}

bool townShopIsClosed() {
  return GLOBAL_TIME.phase > TS_DAY;
}

void townDestroyBoard(TownContext* context) {
  PauseOverlayDestroy(context->pauseOverlay, context->board);
  TextBoxDestroy(context->textBox);
  LinkedListDestroy(context->topTentacles);

  LinkedListResetCursor(context->allActors);
  Actor* currentActor = LinkedListNext(context->allActors);
  while (currentActor) {
    COIBoardRemoveDynamicSprite(context->board, currentActor->sprite, 0);
    COISpriteDestroy(currentActor->sprite);
    actorDestroy(currentActor);
    currentActor = LinkedListNext(context->allActors);
  }
  LinkedListDestroy(context->allActors);
  COIMenuDestroyAndFreeComponents(context->confirmMenu, context->board);
  COIBoardDestroy(context->board);
  free(context);
}


