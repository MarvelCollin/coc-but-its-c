#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "./lib/views.h"
#include "./lib/player_avl.h"

#define wall 219        
#define RESET "\x1b[0m"
#define BLACK "\x1b[30m"

#define WIDTH 31
#define HEIGHT 35
	
enum Pages {
	HOMEPAGE,
	PREGAME,
	INGAME,
	TESTCASE,
	TRAININGTROOP,
	TESTER
};

struct Building {
	char name[100];
	int health;
};

typedef struct {
    char name[100];
    int trophy;
} Leaderboard;

void cls(){
	system("cls");
}

enum Pages currentPage = HOMEPAGE;

void views();
void click();
void sortTest();

void clickController();
bool isInitializing = false;
int yMouse, xMouse;

// prettifier function
void fullscreen() {
    keybd_event(VK_MENU, 0x38, 0, 0);
    keybd_event(VK_RETURN, 0x1c, 0, 0);
    keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
    Sleep(100);
}

void redirect(enum Pages curr){
	currentPage = curr;
	cls();
	views();
}

// PRIM ALGORITHM FOR PLAYER MAP
char playerMap[WIDTH][HEIGHT];

typedef struct {
    int x, y; 
} PrimCell;

int isValid_prim(int x, int y) {
    return x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT; 
}

void changePlayerMap(int yPos, int xPos, char value){
    playerMap[yPos][xPos] = value;
}

void addNeighbors_prim(PrimCell* neighbors, int* neighborCount, int x, int y) {
    PrimCell directions[] = {{0, 2}, {2, 0}, {0, -2}, {-2, 0}}; // atas, kanan, bawah, kiri
    for (int i = 0; i < 4; ++i) {
        int nx = x + directions[i].x; // cari koordinat neighbor
        int ny = y + directions[i].y;
        if (isValid_prim(nx, ny) && playerMap[nx][ny] == '#') { // kalo dinding atau diluar
            neighbors[*neighborCount].x = nx; // push
            neighbors[*neighborCount].y = ny;
            (*neighborCount)++; // tambahin sizeny
        }
    }
}

void generatePlayerMap() {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) { 
            playerMap[x][y] = '#';
        }
    }

    PrimCell neighbors[WIDTH * HEIGHT]; 
    int neighborCount = 0;

    playerMap[1][1] = ' '; 
    addNeighbors_prim(neighbors, &neighborCount, 1, 1); // add neighbor ke titik awal

    while (neighborCount > 0) { 
        int idx = rand() % neighborCount;
        PrimCell current = neighbors[idx];
        neighbors[idx] = neighbors[--neighborCount]; 

        int x = current.x; 
        int y = current.y; 

        if (playerMap[x][y] == '#') { // Kalo # 
            PrimCell directions[] = {{0, 2}, {2, 0}, {0, -2}, {-2, 0}}; // atas, kanan, bawah, kiri
            for (int i = 0; i < 4; ++i) { 
                int nx = x + directions[i].x; 
                int ny = y + directions[i].y;
                if (isValid_prim(nx, ny) && playerMap[nx][ny] == ' ') { 
                    playerMap[x][y] = ' '; // jadi path
                    playerMap[(x + nx) / 2][(y + ny) / 2] = ' '; // hapus dinding 
                    addNeighbors_prim(neighbors, &neighborCount, x, y); // tamabbhin neighbor baru
                    break; 
                }
            }
        }
    }
    // karna kanan sama bawah bukan dinding jadi mau inisialisasi lagi 
    for (int i = 0; i < WIDTH; i++) {
        playerMap[HEIGHT - 1][i] = '#'; 
    }
    for (int i = 0; i < HEIGHT; i++) { 
        playerMap[HEIGHT - 1][i] = '#'; 
    }
}

void printPlayerMap() {
    for (int y = 0; y < HEIGHT; y++) { 
        for (int x = 0; x < WIDTH; x++) { 
            if (playerMap[y][x] == ' ') { 
                printf(" "); 
            } else if(playerMap[y][x] == '#'){
                printf("%c", wall);
            } else {
                printf("%c", playerMap[y][x]);
            }
        }
        printf("\n"); 
    }
}

void initializeBuilding(){
	generatePlayerMap();
	char buildings[3] = {'C', 'T', 'G'};
	
	for(int i = 0; i < 3; i++){
		printPlayerMap();
		click();
		changePlayerMap(yMouse, xMouse, buildings[i]);
		cls();
	}
	printPlayerMap();
}

// SORTING 
void mergeBuilding(Building arr[], int start, int mid, int end) {
    int len = end - start + 1;
    Building temp[len];
    int i = start, j = mid + 1, k = 0;

    while (i <= mid && j <= end) {
        if (arr[i].health <= arr[j].health) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    while (i <= mid) {
        temp[k++] = arr[i++];
    }
    while (j <= end) {
        temp[k++] = arr[j++];
    }

    for (int x = 0; x < len; x++) {
        arr[start + x] = temp[x];
    }
}

// Merge sort function for Building array
void mergesortBuilding(Building arr[], int start, int end) {
    if (start < end) {
        int mid = (start + end) / 2;
        mergesortBuilding(arr, start, mid);
        mergesortBuilding(arr, mid + 1, end);
        mergeBuilding(arr, start, mid, end);
    }
}

void swap_building(struct Building* a, struct Building* b) {
    struct Building temp = *a;
    *a = *b;
    *b = temp;
}

int partition_building(struct Building arr[], int low, int high) {
    int pivot = arr[high].health;
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j].health < pivot) {
            i++;
            swap_building(&arr[i], &arr[j]);
        }
    }
    swap_building(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort_building(struct Building arr[], int low, int high) {
    if (low < high) {
        int pi = partition_building(arr, low, high);

        quickSort_building(arr, low, pi - 1);
        quickSort_building(arr, pi + 1, high);
    }
}

void printSortedBuilding(struct Building arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("Name: %s, Health: %d\n", arr[i].name, arr[i].health);
}

// Max Heap for Leaderboard
void swap_leaderboard(Leaderboard *a, Leaderboard *b) {
    Leaderboard temp = *a;
    *a = *b;
    *b = temp;
}

void insertMaxHeap_leaderboard(Leaderboard arr[], int *n, Leaderboard newEntry) {
    *n = *n + 1;
    int i = *n - 1;
    arr[i] = newEntry;

    while (i != 0 && arr[(i - 1) / 2].trophy < arr[i].trophy) {
        swap_leaderboard(&arr[i], &arr[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

void printMaxHeap_leaderboard(Leaderboard arr[], int n) {
    printf("Max Heap:\n");
    for (int i = 0; i < n; ++i)
        printf("%s - %d\n", arr[i].name, arr[i].trophy);
    printf("\n");
}

// CLICK HANDLER 
void click() {
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE); // declare handle dari input standard
    DWORD prevMode;
    INPUT_RECORD ir[128]; // buffer utk store input
    DWORD numEvents; // event listener
    BOOL clickDetected = FALSE; 

    GetConsoleMode(hInput, &prevMode); // dapatin mode console skrg
    SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT); // active input mousenya

    while (!clickDetected) {
        // tunggu event dari input
        if (WaitForSingleObject(hInput, INFINITE) == WAIT_OBJECT_0 &&
            GetNumberOfConsoleInputEvents(hInput, &numEvents) && numEvents > 0 &&
            ReadConsoleInput(hInput, ir, 128, &numEvents)) {

            // iterasi dari event yang tadi didapat
            for (DWORD i = 0; i < numEvents; ++i) {
                // cek kalau ada event terjadi atau click kanan
                if (ir[i].Event.MouseEvent.dwEventFlags == 0 && ir[i].EventType == MOUSE_EVENT && ir[i].Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED ) {

                    COORD pos = ir[i].Event.MouseEvent.dwMousePosition;
//                    printf("Y: %d, X: %d\n", pos.Y, pos.X); 
                    yMouse = pos.Y;
                    xMouse = pos.X;
                    clickController();
                    clickDetected = TRUE; 
                    break; 
                }
            }
        }
    }
    
    // validasi tambahan 
//				
//                    

    SetConsoleMode(hInput, prevMode); // Kembalikan mode konsol sebelumnya
}

void authenticationController(){
	authenticationView();
	redirect(PREGAME);
}

void homeController(){
	homePageView();
	click();
}

void testCaseView(){
	cls();
	sortTest();
	redirect(TESTER);	
}

void testerController(){
	tester();
	int choice;
	scanf("%d", &choice); getchar();
	switch(choice){
		case 1:
			redirect(TRAININGTROOP);
			break;
		default:
			break;
	}
}

// NESTED LINKEDLIST TRAINING TROOP
typedef struct Troop {
    char name[50];
    char category[50];
    int space;
    int levelRequired;
    int attack;
    int health;
    int trainingTime;
    struct Troop* next; 
} Troop;

typedef struct CategoryPlayer {
    char category[50];
    Troop* troops; 
    struct CategoryPlayer* next;
} CategoryPlayer;

CategoryPlayer* categoryList = NULL;

Troop* createTroop(char* name, char* category, int space, int levelRequired, int attack, int health, int trainingTime) {
    Troop* newTroop = (Troop*)malloc(sizeof(Troop));
    strcpy(newTroop->name, name);
    strcpy(newTroop->category, category);
    newTroop->space = space;
    newTroop->levelRequired = levelRequired;
    newTroop->attack = attack;
    newTroop->health = health;
    newTroop->trainingTime = trainingTime;
    newTroop->next = NULL;
    return newTroop;
}

CategoryPlayer* createCategoryPlayer(char* category) {
    CategoryPlayer* newPlayer = (CategoryPlayer*)malloc(sizeof(CategoryPlayer));
    strcpy(newPlayer->category, category);
    newPlayer->troops = NULL;
    newPlayer->next = NULL;
    return newPlayer;
}

void addTroopToCategory(CategoryPlayer* categoryPlayer, Troop* troop) {
    troop->next = categoryPlayer->troops;
    categoryPlayer->troops = troop;
}

CategoryPlayer* findCategoryPlayer(char* category) {
    CategoryPlayer* temp = categoryList;
    while (temp != NULL) {
        if (strcmp(temp->category, category) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void addCategoryPlayer(CategoryPlayer* newPlayer) {
    newPlayer->next = categoryList;
    categoryList = newPlayer;
}

void createAndAddTroop(char* name, char* category, int space, int levelRequired, int attack, int health, int trainingTime) {
    Troop* newTroop = createTroop(name, category, space, levelRequired, attack, health, trainingTime);
    CategoryPlayer* categoryPlayer = findCategoryPlayer(category);
    if (categoryPlayer == NULL) {
        categoryPlayer = createCategoryPlayer(category);
        addCategoryPlayer(categoryPlayer);
    }
    addTroopToCategory(categoryPlayer, newTroop);
}

void sortCategoryList() {
    CategoryPlayer* meleeList = NULL;
    CategoryPlayer* rangedList = NULL;
    CategoryPlayer* pendingList = NULL;
    CategoryPlayer* readyList = NULL;

    CategoryPlayer* current = categoryList;
    while (current != NULL) {
        CategoryPlayer* next = current->next;
        if (strcmp(current->category, "Melee") == 0) {
            current->next = meleeList;
            meleeList = current;
        } else if (strcmp(current->category, "Ranged") == 0) {
            current->next = rangedList;
            rangedList = current;
        } else if (strcmp(current->category, "Pending") == 0) {
            current->next = pendingList;
            pendingList = current;
        } else if (strcmp(current->category, "Ready") == 0) {
            current->next = readyList;
            readyList = current;
        }
        current = next;
    }

    categoryList = meleeList;
    current = categoryList;
    if (current == NULL) {
        categoryList = rangedList;
        current = categoryList;
    } else {
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = rangedList;
        while (current->next != NULL) {
            current = current->next;
        }
    }
    current->next = pendingList;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = readyList;
}


void trainingTroopController(){
	sortCategoryList();
	CategoryPlayer* categoryTemp = categoryList;
	cls();
	printf("Back (click to test)\n");
    while (categoryTemp != NULL) {
        printf("Category: %s\n", categoryTemp->category);
        Troop* troopTemp = categoryTemp->troops;
        while (troopTemp != NULL) {
            printf("  %s\n", troopTemp->name);
            troopTemp = troopTemp->next;
        }
        printf("\n\n");
        categoryTemp = categoryTemp->next;
    }
    click();
	
}

void views(){
	cls();
	switch(currentPage){
		case HOMEPAGE:
			homeController();
			break;
		case PREGAME:
			initializeBuilding();
			break;
		case TESTCASE:
			testCaseView();
			break;
		case TESTER:
			testerController();
			break;
		case TRAININGTROOP:
			trainingTroopController();
			break;
		default: 
			break;
	}
}


bool isThisPosition(int yStart, int xStart, int yEnd, int xEnd, int yPos, int xPos){
	if((yPos >= yStart && yPos <= yEnd) && (xPos >= xStart && xPos <= xEnd)){
		return true;
	}
	
	return false;
}

void clickController(){
	if(currentPage == HOMEPAGE){
		if(isThisPosition(0,0,2,14,yMouse,xMouse)){
//			cls();
			authenticationController();
		} else if (isThisPosition(12, 0, 14, 14, yMouse, xMouse)){
			redirect(TESTCASE);
		}
	} else if(currentPage == TRAININGTROOP){
		if(isThisPosition(2,2,2,10,yMouse, xMouse)){
			createAndAddTroop("Barbarian", "Pending", 3, 4, 50, 100, 60);
			redirect(TRAININGTROOP);
		} else if(isThisPosition(6,2,6,7,yMouse, xMouse)){
			createAndAddTroop("Archer", "Pending", 3, 4, 50, 100, 60);
			redirect(TRAININGTROOP);
		} else if(isThisPosition(0,0,0,3, yMouse, xMouse)){
			redirect(HOMEPAGE);
		}
	}
}

void sortTest(){
	Building buildings[] = {
        {"Town Hall", 300},
        {"Cannon", 150},
        {"Cannon", 250},
        {"Gold Mine", 200},
        {"Gold Mine", 180}
    };
    int n = sizeof(buildings) / sizeof(buildings[0]);
	printf("before sort building\n");
	for (int i = 0; i < n; i++) {
        printf("%s | %d\n", buildings[i].name, buildings[i].health);
    }

	printf("\nafter sort building\n");
    mergesortBuilding(buildings, 0, n - 1);
    for (int i = 0; i < n; i++) {
        printf("%s | %d\n", buildings[i].name, buildings[i].health);
    }
    
//    Building buildingTestForQuick[] {
//        {"Town Hall", 300},
//        {"Cannon", 150},
//        {"Cannon", 250},
//        {"Gold Mine", 200},
//        {"Gold Mine", 180}
//    };
	
	printf("\n\n\n");
	quickSort_building(buildings, 0, n - 1);
	printSortedBuilding(buildings, n);
    printf("\n");
    
    Leaderboard maxHeap[100];
    int heapSize = 0;
    
    insertMaxHeap_leaderboard(maxHeap, &heapSize, (Leaderboard){"Player1", 200});
    insertMaxHeap_leaderboard(maxHeap, &heapSize, (Leaderboard){"Player2", 150});
    insertMaxHeap_leaderboard(maxHeap, &heapSize, (Leaderboard){"Player3", 300});
    insertMaxHeap_leaderboard(maxHeap, &heapSize, (Leaderboard){"Player4", 180});
    insertMaxHeap_leaderboard(maxHeap, &heapSize, (Leaderboard){"Player5", 250});
    
    printMaxHeap_leaderboard(maxHeap, heapSize);
    printf("enter to continue to the training \n");
	getchar();
}

int main(){
	createAndAddTroop("Archer", "Ranged", 1, 2, 25, 20, 30);
	createAndAddTroop("Barbarian", "Melee", 3, 4, 50, 100, 60);

	fullscreen();
	srand(time(NULL));
	redirect(TRAININGTROOP);
	views();	
//	homePageView();
//	views();
	return 0;
}
