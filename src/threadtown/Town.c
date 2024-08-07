#include "Town.h"

int _testForCollision(TownContext* context, COISprite* actorSprite, int changeX, int changeY) {
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

  // Check for collision against NPCs
  for (i = 0; i < TOWN_NUM_NPCS; i++) {
    currentSprite = context->npcs[i]->sprite;
    // Don't check for collision against self
    if (currentSprite == actorSprite) {
      continue;
    }
    collisionResult = COISpriteCollision(currentSprite,
					 actorSprite->_x + changeX,
					 actorSprite->_y + changeY,
					 actorSprite->_width,
					 actorSprite->_height);
    if (collisionResult != COI_NO_COLLISION) {
      return collisionResult;
    }
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
void _animateTentacles(TownContext* context) {
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

Actor* _facingNPC(Actor* player, Actor** npcs) {
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
  
  for (int i = 0; i < TOWN_NUM_NPCS; i++) {
    if (npcs[i]->sprite->_x == facingX &&
	npcs[i]->sprite->_y == facingY) {
      return npcs[i];
    }
  }

  return NULL;
}

void _createNPCs(TownContext* context) {
  context->npcs[0] = actorCreateOfType(ACTOR_CHAGGAI,
				       1408,
				       1856,
				       COI_GLOBAL_LOADER,
				       COI_GLOBAL_WINDOW);
  COIBoardAddDynamicSprite(context->board, context->npcs[0]->sprite);
  actorFaceRight(context->npcs[0]);

  context->npcs[1] = actorCreateOfType(ACTOR_CHAGGAI,
				       1664,
				       2176,
				       COI_GLOBAL_LOADER,
				       COI_GLOBAL_WINDOW);
  COIBoardAddDynamicSprite(context->board, context->npcs[1]->sprite);
  actorFaceRight(context->npcs[1]);

  context->npcs[2] = actorCreateOfType(ACTOR_CHAGGAI,
				       2240,
				       2048,
				       COI_GLOBAL_LOADER,
				       COI_GLOBAL_WINDOW);
  COIBoardAddDynamicSprite(context->board, context->npcs[2]->sprite);
  actorFaceRight(context->npcs[2]);

  context->npcs[3] = actorCreateOfType(ACTOR_LANDLORD,
				       2176,
				       2528,
				       COI_GLOBAL_LOADER,
				       COI_GLOBAL_WINDOW);
  COIBoardAddDynamicSprite(context->board, context->npcs[3]->sprite);
  actorFaceDown(context->npcs[3]);

  context->npcs[4] = actorCreateOfType(ACTOR_MERCHANT,
				       2080,
				       2144,
				       COI_GLOBAL_LOADER,
				       COI_GLOBAL_WINDOW);
  COIBoardAddDynamicSprite(context->board, context->npcs[4]->sprite);
  actorFaceDown(context->npcs[4]);
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
int _getNextCollision(TownContext* context, Actor* actor, int direction) {
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

void _talkToLandlord(TownContext* context) {
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

void _talkToMerchant(TownContext* context) {
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

void _confirmMenuSelect(TownContext* context) {
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
        playerCheckForEviction(context->pInfo);
	      COITransitionInit(&COI_GLOBAL_WINDOW->transition,
			      COI_TRANSITION_ENCLOSE,
			      COI_GLOBAL_WINDOW);
      } else {
	      context->pInfo->working = true;
	      _talkToMerchant(context);
        // printf("talked to merchant 2\n");
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
  if (context->talkingActorType == ACTOR_MERCHANT) {
    printf("SPEAKING TO MERCHANT\n");
  }
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
  } else if (!context->textBox->box->_visible) {
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
    Actor* talkingNPC = _facingNPC(player, context->npcs);
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
      for (int i = 0; i < TOWN_NUM_NPCS; i++) {
	// 30% chance they move
	if (context->npcs[i]->actorType == ACTOR_CHAGGAI && generateRandomBoolWeighted(0.3)) {
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
  for (int i = 0; i < TOWN_NUM_NPCS; i++) {
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
  if (player->movementDirection == MOVING_NONE && !context->confirmMenu->_frame->_visible) {
    PauseOverlaySetVisible(context->pauseOverlay, !context->pauseOverlay->visible);
    COIBoardQueueDraw(context->board);
  }
}



void townDestroyBoard(TownContext* context) {
  PauseOverlayDestroy(context->pauseOverlay, context->board);
  TextBoxDestroy(context->textBox);
  LinkedListDestroy(context->topTentacles);
  for (int i = 0; i < TOWN_NUM_NPCS; i++) {
    COISprite* sprite = context->npcs[i]->sprite;
    COIBoardRemoveDynamicSprite(context->board, sprite);
    COISpriteDestroy(sprite);
    actorDestroy(context->npcs[i]);
  }
  COIMenuDestroyAndFreeComponents(context->confirmMenu, context->board);
  COIBoardDestroy(context->board);
  free(context);
}


