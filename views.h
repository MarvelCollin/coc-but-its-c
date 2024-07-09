void cursorTop(int x){
	printf("\033[%dA", x);
}

void cursorRight(int x){
	printf("\033[%dC", x);
}

void cursorLeft(int x){
	printf("\033[%dD", x);
}

void cursorDown(int x){
	printf("\033[%dB", x);
}

void getView(Pages *currentPage){
	switch(currentPage){
		case HOMEPAGE:
			homeController();
			break;
		case PREGAME:
			initializeBuilding();
		case TESTCASE:
			testCaseView();
		default: 
			break;
	}
}

void homePageView(){
	printf("###############\n");
	printf("#  Play Game  #\n");
	printf("###############\n");
	printf("\n");
	printf("###############\n");
	printf("#  Load Game  #\n");
	printf("###############\n");
	printf("\n");
	printf("###############\n");
	printf("#  Exit Game  #\n");
	printf("###############\n");
	printf("\n");
	printf("###############\n");
	printf("#  Test Case  #\n");
	printf("###############\n");
	printf("\n");
}

void authenticationView(){
	cursorDown(10);
	cursorRight(50);
	char villageName[100];
	printf("Input your village's name : ");
	scanf("%[^\n]", villageName);
	printf("%s", villageName);
}
