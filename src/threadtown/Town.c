#include "Town.h"

int _testForCollision(COIBoard* board, COISprite* player, int changeX, int changeY) {
  // Probably want this to only look  at visible sprites
  int maxSpriteIndex = board->_spriteCount;
  COISprite* currentSprite = NULL;
  int i;
  int collisionResult;
  for (i = 0; i < maxSpriteIndex; i++) {
    currentSprite = board->_sprites[i];
    collisionResult = COISpriteCollision(currentSprite,
					 player->_x + changeX,
					 player->_y + changeY,
					 player->_width,
					 player->_height);
    if (collisionResult != COI_NO_COLLISION) {
      return collisionResult;
    }
  }

  return COI_NO_COLLISION;
}


COIBoard* townCreateBoard(COIWindow* window, COIAssetLoader* loader, PlayerInfo* pInfo) {
  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 5000, 5000, loader);
  COIBoardLoadSpriteMap(board, COIWindowGetRenderer(window), "src/threadtown/spritemap.dat");
  TownContext* context = malloc(sizeof(TownContext));
  context->window = window;
  context->pInfo = pInfo;
  context->direction = MOVING_NONE;
  context->terrain = TT_SAFE;
  context->terrainTicks = 0;
  context->board = board;
  context->willEnterBattle = false;
  context->textType = COITextTypeCreate(25, 255, 255, 255, COIWindowGetRenderer(window));
  context->pauseOverlay = PauseOverlayCreate(pInfo, context->textType, context->board); 
  // Only 1 persistent sprite: the player
  COISprite** perSprites = actorGetSpriteList(context->pInfo->party, 1);
  COIBoardSetPersistentSprites(board, perSprites, 1);

  Actor* player = context->pInfo->party[0];
  // Adjust board player is in the center of the screen.
  int playerCenterX = player->sprite->_x - board->_frameX + (player->sprite->_width / 2);
  COIBoardShiftFrameX(board, playerCenterX);
  int playerCenterY = player->sprite->_y - board->_frameY + (player->sprite->_height / 2);
  COIBoardShiftFrameY(board, playerCenterY);
  
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
    context->board->_shouldDraw = true;
  } else {
    actor->nextMovementDirection = direction;
    //actor->nextMovementDirection = direction;
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
  context->terrainTicks = 0;
  switch (context->terrain) {
  case TT_THICK_GRASS:
    context->willEnterBattle = generateRandomBoolWeighted(0.05);
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
  return  _testForCollision(context->board, actor->sprite, changeX, changeY);
}


void townProcessDirectionalInput(TownContext* context, int direction) {
  if (context->pauseOverlay->visible) {
    PauseOverlayProcessInput(context->pauseOverlay, direction);
    context->board->_shouldDraw = true;
  } else {
    Actor* player = context->pInfo->party[0];
    //bool canAcceptInput = player->_stepsLeft == 0;
    _queueMovement(context, player, direction, TOWN_MOVE_SPEED);
  }
}

void townProcessSelectionInput(TownContext* context) {
  Actor* player = context->pInfo->party[0];
  if (context->pauseOverlay->visible) {
    PauseOverlaySelect(context->pauseOverlay);
    context->board->_shouldDraw = true;
  }
}


void townProcessCollisionType(TownContext* context, int collision) {
  Actor* player = context->pInfo->party[0];
  switch (collision) {
  case COI_COLLISION:
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


int townCheckForCollision(TownContext* context) {
  Actor* player = context->pInfo->party[0];
  // We haven't started moving from our current square yet if the x and y of the sprite
  // are cleanly divisible by the size of a grid square.
  if (player->sprite->_x % COIBOARD_GRID_SIZE == 0 &&
      player->sprite->_y % COIBOARD_GRID_SIZE == 0) {
    return _getNextCollision(context, player, player->movementDirection);
  }

  return COI_NO_COLLISION;
}

void townMovePlayer(TownContext* context) {
  Actor* player = context->pInfo->party[0];
  COIBoard* board = context->board;
  int playerCenterX, playerCenterY;
  bool inNextGridCell;
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
    player->nextMovementDirection = MOVING_NONE;
    player->movementDirection = MOVING_NONE;
  }
}

void townTogglePauseOverlay(TownContext* context) {
  Actor* player = context->pInfo->party[0];
  if (player->movementDirection == MOVING_NONE) {
    PauseOverlaySetVisible(context->pauseOverlay, !context->pauseOverlay->visible);
    context->board->_shouldDraw = true;
  }
}



void townDestroyBoard(TownContext* context) {
  PauseOverlayDestroy(context->pauseOverlay, context->board);
  COIBoardDestroy(context->board);
  free(context);
}


