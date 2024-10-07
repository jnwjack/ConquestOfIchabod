#include "Town.h"
#include "../special.h"

#define PAY_PER_SHIFT 100

static int _testForCollision(TownContext* context, COISprite* actorSprite, int changeX, int changeY) {
  // Probably want this to only look at visible sprites
  COIBoard* board = context->board;
  int maxSpriteIndex = board->_spriteCount;
  COISprite* currentSprite = NULL;
  int i;
  int collisionResult = COI_NO_COLLISION;;
  for (i = 0; i < maxSpriteIndex; i++) {
    currentSprite = board->_sprites[i];
    collisionResult = COISpriteCollision(currentSprite,
					 actorSprite->_x + changeX,
					 actorSprite->_y + changeY,
					 actorSprite->_width,
					 actorSprite->_height);
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
  currentSprite = context->pInfo->party[0]->sprite;
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
      if (tentacle->_visible) {
	COIBoardQueueDraw(context->board);
      }
    }

    tentacle = (COISprite*)LinkedListNext(context->topTentacles);
  }
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
    COIBoardRemoveDynamicSprite(context->board, context->swordChest->sprite);
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
  COIBoardAddDynamicSprite(context->board, context->swordChest->sprite);
  LinkedListResetCursor(context->allActors);
  LinkedListAdd(context->allActors, (void*)context->swordChest);
}

static void _createNPCs(TownContext* context) {
  context->allActors = LinkedListCreate();

  int xPositions[TOWN_NUM_NPC_CITIZENS] = { 1408, 1664, 2240, 2176, 2080, 2176, 2976, 3872 };
  int yPositions[TOWN_NUM_NPC_CITIZENS] = { 1856, 2176, 2048, 2528, 2144, 2656, 3232, 928 };
  int actorIDs[TOWN_NUM_NPC_CITIZENS] = { ACTOR_CHAGGAI, ACTOR_CHAGGAI, ACTOR_CHAGGAI, ACTOR_LANDLORD, ACTOR_MERCHANT, ACTOR_TAGNESSE_GUY, ACTOR_TREE_GUY, ACTOR_HAVONVALE_GUY };

  for (int i = 0; i < TOWN_NUM_NPC_CITIZENS; i++) {
    context->npcs[i] = actorCreateOfType(actorIDs[i],
                xPositions[i],
                yPositions[i],
                COI_GLOBAL_LOADER,
                COI_GLOBAL_WINDOW);
    COIBoardAddDynamicSprite(context->board, context->npcs[i]->sprite);
    actorFaceRight(context->npcs[i]);
  }
  _createSwordChest(context);

  for (int i = 0; i < TOWN_NUM_NPC_CITIZENS; i++) {
    LinkedListAdd(context->allActors, (void*)context->npcs[i]);
  }
}


COIBoard* townCreateBoard(COIWindow* window, COIAssetLoader* loader, PlayerInfo* pInfo) {
  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 5000, 5000, loader);
  
  COIBoardLoadSpriteMap(board, COIWindowGetRenderer(window), "src/threadtown/spritemap.dat");
  TownContext* context = malloc(sizeof(TownContext));
  context->window = window;
  context->pInfo = pInfo;
  context->direction = MOVING_NONE;
  context->terrain = TT_SAFE;
  context->battleActorType = ACTOR_SKELETON;
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
  COIBoardShiftFrameX(board, playerCenterX - COI_GLOBAL_WINDOW->_width / 2);
  int playerCenterY = player->sprite->_y - board->_frameY + (player->sprite->_height / 2);
  COIBoardShiftFrameY(board, playerCenterY - COI_GLOBAL_WINDOW->_height / 2);

  _createNPCs(context);
  context->talkingActorType = ACTOR_NONE;
  
  // Yes/No dialog
  COISprite* frame = COISpriteCreateFromAssetID(70, 250, 150, 80,
						COI_GLOBAL_LOADER,
						5,
						COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  frame->_autoHandle = false;
  COIBoardAddDynamicSprite(context->board, frame);
  COISprite* pointer = COISpriteCreateFromAssetID(0, 0, 32, 32,
						  COI_GLOBAL_LOADER,
						  6,
						  COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  pointer->_autoHandle = false;
  COIBoardAddDynamicSprite(context->board, pointer);
  context->confirmMenu = COIMenuCreateWithCapacity(frame, pointer, BACKPACK_SIZE);
  COIString* yes = COIStringCreate("Yes", 0, 0, context->textType);
  COIBoardAddString(context->board, yes);
  COIString* no = COIStringCreate("No", 0, 0, context->textType);
  COIBoardAddString(context->board, no);
  COIMenuAddString(context->confirmMenu, yes, 0);
  COIMenuAddString(context->confirmMenu, no, 1);
  COIMenuSetInvisible(context->confirmMenu);

  context->pauseOverlay = PauseOverlayCreate(pInfo, context->textType, context->board);

  context->textBox = TextBoxCreate(context->board, context->textType);

  // Gather animated tentacle sprites
  context->topTentacles = LinkedListCreate();
  int startingCol = 0;
  for (int i = 0; i < board->_spriteCount; i++) {
    if (board->_sprites[i]->_assetID == 21) {
      COISpriteSetSheetIndex(board->_sprites[i], 0, startingCol);
      LinkedListAdd(context->topTentacles,
		    (void*)board->_sprites[i]);
      startingCol = (startingCol + 1) % 3;
    }
    
  }
  COIBoardSetContext(board, (void*)context);

  return board;
}

// If we're not currently in motion, start moving in the provided direction.
// Otherwise, just set which direction we want to move next.
void _queueMovement(TownContext* context, Actor* actor, int direction, int speed) {
  if (actor->movementDirection == MOVING_NONE) {
    actor->_stepsLeft = COIBOARD_GRID_SIZE;
    actor->movementDirection = direction;
    actor->nextMovementDirection = direction;
    actor->_speed = speed;
    actorFaceDirection(actor, direction);
    COIBoardQueueDraw(context->board);
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
  case TT_THICK_GRASS:
    context->willEnterBattle = generateRandomBoolWeighted(0.05);
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
    context->battleActorType = ACTOR_BOOWOW;
    break;
  case TENTACLE:
    context->terrain = TT_TENTACLE;
    context->battleActorType = ACTOR_TENTACLE;
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

static void _talkToLandlord(TownContext* context) {
  if (context->pInfo->renting == RS_RENTING) {
    unsigned long daysLeft = context->pInfo->nextRentDate - GLOBAL_TIME.day;
    if (daysLeft == 1) {
      TextBoxSetStrings(context->textBox,
        "Rent's due tomorrow.",
        NULL);
    } else if (daysLeft == 0) {
      TextBoxSetStrings(context->textBox,
        "Rent's due today.",
        NULL);
    } else {
      char temp[MAX_STRING_SIZE];
      snprintf(temp, MAX_STRING_SIZE, "Rent's due in %lu days.", context->pInfo->nextRentDate - GLOBAL_TIME.day);
      TextBoxSetStrings(context->textBox,
            temp,
            NULL);
    }
  } else if (context->pInfo->renting == RS_NOT_RENTING) {
    TextBoxSetStrings(context->textBox,
		      "I'm renting out a room. Interested?",
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
		    "I have an opening for a clerk in my shop. Need a job?",
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
        "Oh...you sold it?",
        "I guess if you need to.",
        NULL);
  } else {
    TextBoxSetStrings(context->textBox,
        "No way! I can't believe it's real.",
        "You should head to Havonvale in the east. There's an adventurer like you seeking other strong warriors.",
        "Maybe he could join your party!",
        NULL);
  }
}

static void _talkToTreeGuy(TownContext* context) {
  if (GLOBAL_TIME.day > 50) {
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
  TextBoxSetStrings(context->textBox,
      "Havonvale lies past here.",
      "There's a group of monsters blocking the way, though. I'd be careful.",
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
	      _talkToMerchant(context);
      }
      break;
    default:
      printf("Error: confirm menu appeared for invalid actor type\n");
    }
  }
  COIMenuSetInvisible(context->confirmMenu);
  COIBoardQueueDraw(context->board);
}

bool _shouldPromptForAnswer(TownContext* context) {
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

void townProcessDirectionalInput(TownContext* context, int direction) {
  if (context->pauseOverlay->visible) {
    // COISoundPlay(COI_SOUND_BLIP);
    PauseOverlayProcessInput(context->pauseOverlay, direction);
    COIBoardQueueDraw(context->board);
  } else if (context->confirmMenu->_frame->_visible) {
    COIMenuHandleInput(context->confirmMenu, direction);
    COIBoardQueueDraw(context->board);
  } else if (!context->textBox->box->_visible && inputIsDirection(direction)) {
    Actor* player = context->pInfo->party[0];
    _queueMovement(context, player, direction, TOWN_MOVE_SPEED);
  }
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
      switch (context->talkingActorType) {
      case ACTOR_LANDLORD:
	      _talkToLandlord(context);
	      break;
      case ACTOR_MERCHANT:
	      _talkToMerchant(context);
        // printf("talked to merchant 1\n");
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
      default:
	      TextBoxSetStrings(context->textBox,
			      "I saw something scary in the northeast.",
			      "I'd take shelter if I were you.",
			      NULL);
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
	if (context->npcs[i]->actorType != ACTOR_MERCHANT && context->npcs[i]->actorType != ACTOR_LANDLORD && generateRandomBoolWeighted(0.3)) {
	  // If we do move, pick 1 of the 4 directions
	  _queueMovement(context, context->npcs[i], generateRandomDirectionalMovement(), TOWN_MOVE_SPEED);
	  // Cancel movement if there's a collision
	  if (townCheckForCollision(context, context->npcs[i]) != COI_NO_COLLISION) {
	    context->npcs[i]->movementDirection = MOVING_NONE;
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
    break;
  case MOVING_RIGHT:
    inNextGridCell = townContinueMovement(player, context->board);
    playerCenterX = player->sprite->_x - board->_frameX + (player->sprite->_width / 2);
    if (playerCenterX >= board->_frameWidth / 2) {
      COIBoardShiftFrameX(board, TOWN_MOVE_SPEED);
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
  }
}

void townTogglePauseOverlay(TownContext* context) {
  Actor* player = context->pInfo->party[0];
  if (player->movementDirection == MOVING_NONE && !context->confirmMenu->_frame->_visible && !context->textBox->box->_visible) {
    PauseOverlaySetVisible(context->pauseOverlay, !context->pauseOverlay->visible);
    COIBoardQueueDraw(context->board);
  }
}

// Depending on the time, we should change the layout.
// Ex: The merchant should only appear during the day.
// Run this whenever we change the active board to the town.
void townApplyTimeChanges(TownContext* context) {
  if (GLOBAL_TIME.phase < TS_EVENING) {
    COISpriteSetPos(context->npcs[4]->sprite, 2080, 2144);
  } else {
    // Move merchant off screen.
    COISpriteSetPos(context->npcs[4]->sprite, -50, -50);
  }

  if (GLOBAL_TIME.day - context->pInfo->lastXPGain.day > 3 &&
      context->pInfo->level > 1) {
    int numAbilities = context->pInfo->party[0]->specials.length + context->pInfo->party[0]->techList->count;
    if (numAbilities > 0) { // We're going to lose an ability when we level down.
      TextBoxSetStrings(context->textBox,
        "You have not gained experience in some time. Your skills are beginning to atrophy.",
        "You have lost a level.",
        "You have forgotten one of your abilities.",
        NULL);
    } else {
      TextBoxSetStrings(context->textBox,
        "You have not gained experience for some time. Your skills are beginning to atrophy.",
        "You have lost a level.",
        NULL);
    }
    playerLevelDown(context->pInfo);
  }

  // Change player's sprite
  SpriteAge nextSpriteAge = playerSpriteAgeFromGlobalTime();
  if (nextSpriteAge != context->pInfo->spriteAge) {
    context->pInfo->spriteAge = nextSpriteAge;
    actorChangeSprite(context->pInfo->party[0], playerSpriteIndexFromSpriteAge(context->pInfo->spriteAge));
    COISprite** perSprites = actorGetSpriteList(context->pInfo->party, context->pInfo->partySize);
    COIBoardSetPersistentSprites(context->board, perSprites, 1);
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
    COIBoardRemoveDynamicSprite(context->board, currentActor->sprite);
    COISpriteDestroy(currentActor->sprite);
    actorDestroy(currentActor);
    currentActor = LinkedListNext(context->allActors);
  }
  LinkedListDestroy(context->allActors);
  // for (int i = 0; i < TOWN_NUM_NPC_CITIZENS; i++) {
  //   COISprite* sprite = context->npcs[i]->sprite;
  //   COIBoardRemoveDynamicSprite(context->board, sprite);
  //   COISpriteDestroy(sprite);
  //   actorDestroy(context->npcs[i]);
  // }
  COIMenuDestroyAndFreeComponents(context->confirmMenu, context->board);
  COIBoardDestroy(context->board);
  free(context);
}


