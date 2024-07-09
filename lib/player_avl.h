struct Player {
    char name[100];
    int gold;
    int level;
    int trophy;
    int troop_capacity;
    struct Player *left, *right;
    int height;
};

struct Player *createPlayer(char name[], int gold, int level, int trophy, int troop_capacity);
struct Player* insertData(struct Player* root, char name[], int gold, int level, int trophy, int troop_capacity);
struct Player* deletePlayer(struct Player* root, char name[]);
void inOrder(struct Player* root, int space);


