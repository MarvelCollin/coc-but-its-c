#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./player_avl.h"

struct Player* createPlayer(char name[], int gold, int level, int trophy, int troop_capacity) {
    struct Player* newPlayer = (struct Player*)malloc(sizeof(struct Player));
    strcpy(newPlayer->name, name);
    newPlayer->gold = gold;
    newPlayer->level = level;
    newPlayer->trophy = trophy;
    newPlayer->troop_capacity = troop_capacity;
    newPlayer->left = NULL;
    newPlayer->right = NULL;
    newPlayer->height = 1;
    return newPlayer;
}

int getMax(int a, int b) {
    return (a > b) ? a : b;
}

int getHeight(struct Player* node) {
    if (node == NULL) {
        return 0;
    } else {
        return node->height;
    }
}

int calculateBalance(struct Player* node) {
    if (node == NULL) {
        return 0;
    } else {
        return getHeight(node->left) - getHeight(node->right);
    }
}

struct Player* rightRotate(struct Player* node) {
    struct Player* tempLeft = node->left;
    struct Player* subRight = tempLeft->right;

    tempLeft->right = node;
    node->left = subRight;

    node->height = getMax(getHeight(node->left), getHeight(node->right)) + 1;
    tempLeft->height = getMax(getHeight(tempLeft->left), getHeight(tempLeft->right)) + 1;

    return tempLeft;
}

struct Player* leftRotate(struct Player* node) {
    struct Player* tempRight = node->right;
    struct Player* subLeft = tempRight->left;

    tempRight->left = node;
    node->right = subLeft;

    node->height = getMax(getHeight(node->left), getHeight(node->right)) + 1;
    tempRight->height = getMax(getHeight(tempRight->left), getHeight(tempRight->right)) + 1;

    return tempRight;
}

struct Player* insertData(struct Player* root, char name[], int gold, int level, int trophy, int troop_capacity) {
    if (root == NULL) {
        return createPlayer(name, gold, level, trophy, troop_capacity);
    }
    int cmp = strcmp(name, root->name);
    if (cmp < 0) {
        root->left = insertData(root->left, name, gold, level, trophy, troop_capacity);
    } else if (cmp > 0) {
        root->right = insertData(root->right, name, gold, level, trophy, troop_capacity);
    } else {
        return root;
    }

    root->height = 1 + getMax(getHeight(root->left), getHeight(root->right));

    int balance = calculateBalance(root);

    if (balance > 1 && strcmp(name, root->left->name) < 0) {
        return rightRotate(root);
    }
    if (balance < -1 && strcmp(name, root->right->name) > 0) {
        return leftRotate(root);
    }
    if (balance > 1 && strcmp(name, root->left->name) > 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && strcmp(name, root->right->name) < 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

struct Player* minValue(struct Player* node) {
    struct Player* curr = node;
    while (curr->left != NULL) {
        curr = curr->left;
    }
    return curr;
}

struct Player* deletePlayer(struct Player* root, char name[]) {
    if (root == NULL) {
        return NULL;
    }

    int cmp = strcmp(name, root->name);
    if (cmp < 0) {
        root->left = deletePlayer(root->left, name);
    } else if (cmp > 0) {
        root->right = deletePlayer(root->right, name);
    } else {
        if (root->left == NULL || root->right == NULL) {
            struct Player* temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
            }
            free(temp);
        } else {
            struct Player* temp = minValue(root->right);

            strcpy(root->name, temp->name);
            root->gold = temp->gold;
            root->level = temp->level;
            root->trophy = temp->trophy;
            root->troop_capacity = temp->troop_capacity;

            root->right = deletePlayer(root->right, temp->name);
        }
    }

    if (root == NULL) {
        return NULL;
    }

    root->height = 1 + getMax(getHeight(root->left), getHeight(root->right));

    int balance = calculateBalance(root);

    if (balance > 1 && calculateBalance(root->left) >= 0) {
        return rightRotate(root);
    }
    if (balance > 1 && calculateBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && calculateBalance(root->right) <= 0) {
        return leftRotate(root);
    }
    if (balance < -1 && calculateBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

void inOrder(struct Player* root) {
    if (root == NULL) return;

    inOrder(root->left);
    printf("Name: %-100s Gold: %d, Level: %d, Trophy: %d, Troop Capacity: %d, Height: %d\n",
           root->name, root->gold, root->level, root->trophy, root->troop_capacity, root->height);
    inOrder(root->right);
}

