#define ITEM_SLOT_WEAPON 0;
#define ITEM_SLOT_HEAD 1;
#define ITEM_SLOT_BODY 2;
#define ITEM_SLOT_LEGS 3;

typedef enum {
  WEAPON,
  ARMOR,
  CONSUMABLE
} ItemType;

typedef struct Item {
  ItemType type;
  int strength;
  int price;
  int slot;
}
