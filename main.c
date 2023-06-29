#define _CRT_SECURE_NO_WARNINGS
#define _USE_INIT_WINDOW_
#include <stdio.h>
#include <conio.h> // 콘솔 환경에서 키 입력처리
#include <Windows.h>
#include <time.h>
#include <mmsystem.h>
#include <process.h>
#include <io.h>
#pragma comment(lib,"winmm.lib")// 색상 정의

// 색상 정의
#define BLACK	0
#define BLUE1	1
#define GREEN1	2
#define CYAN1	3
#define RED1	4
#define MAGENTA1 5
#define YELLOW1	6
#define GRAY1	7
#define GRAY2	8
#define BLUE2	9
#define GREEN2	10
#define CYAN2	11
#define RED2	12
#define MAGENTA2 13
#define YELLOW2	14
#define WHITE	15

#define ESC 0x1b
#define ENTER 0x0d
#define BACKSPACE 0x08

// 방향키 정의
#define SPECIAL1 0xe0 // 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00 // keypad 경우 0x00 + key 로 구성된다.

#define UP  0x48 // Up key는 0xe0 + 0x48 두개의 값이 들어온다.
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d
#define EMPTY ' '

// 게임스크린 가로 세로 길이 정의
#define SCREEN_WIDTH 130
#define SCREEN_HEIGHT 30
#define MAX_WORD_LENGTH 20
#define MAX_NUM_WORD 120

typedef struct Word {
	char value[MAX_WORD_LENGTH]; // 단어 문자열
	int length; // 단어 길이
	int x, y; // 단어 위치
	int spawn; // 단어 생성 여부
	int alive; // 단어 생존 여부
	int colorNum; // 단어 색깔 번호
	int blink;
} Word;

// 랭킹 저장 구조체
typedef struct Info {
	int score;
	int level;
	int min;
	int sec;
	struct Info* next;
} RinkInfo;

// 연결리스트 
typedef struct List {
	RinkInfo* cur;
	RinkInfo* head;
	RinkInfo* tail;
} LinkedList;

void gotoxy(int x, int y);
void printScore(LinkedList* lnk);
void insertData(LinkedList* lnk, int score, int level, int min, int sec);
void rankingShow(void);


void init() {
	system("mode con cols=170 lines=50 | title 타자연습");
}

void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}

// 화면 배경색상
void cls(int text_color, int bg_color)
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);
}

void gotoxy(int x, int y) //내가 원하는 위치로 커서 이동
{
	COORD pos; // Windows.h 에 정의
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void removeCursor(void) { // 커서를 안보이게 한다
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void showCursor(void) { // 커서를 보이게 한다
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void drawBox(int x1, int y1, int x2, int y2) {
	int x, y;
	for (x = x1; x <= x2; x += 1) {
		gotoxy(x, y1);
		textcolor(WHITE, CYAN1);
		printf("%s", " ");
		gotoxy(x, y2);
		textcolor(WHITE, CYAN1);
		printf("%s", " ");
	}
	for (y = y1; y <= y2; y++) {
		gotoxy(x1, y);
		textcolor(WHITE, CYAN1);
		printf("%s", "  ");
		gotoxy(x2, y);
		textcolor(WHITE, CYAN1);
		printf("%s", "  ");
	}
}
void drawCloud(int x, int y);
void clearCloud(int x, int y);

char title1[7][62] = {
	"       11       11    111111111    11             111111111",
	"       11       11    11           11             11     11",
	"       11       11    11           11             11     11",
	"       11111111111    111111111    11             111111111",
	"       11       11    11           11             11",
	"       11       11    11           11             11",
	"       11       11    1111111111   111111111111   11",
};
char title2[7][76] = {
	"       111111111    11111111111   111111  11    111111111111   11        11",
	"       11     11        111       11  11  11    11             11        11",
	"       11     11        111       11  11  11    11             11        11",
	"       111111111        111       11  11  11    11   1111111   11        11",
	"       11               111       11  11  11    11   11   11   11        11",
	"       11               111       11  11  11    11        11   11        11",
	"       11           11111111111   11  111111    111111111111   111111111111",
};
char pingu[4][14] = {
	"   11111111  ",
	" 111011101111",
	" 111122211111",
	"   11111111",
};

void drawPingu(int x, int y, int color1, int color2) {
	for (int i = 0; i < 4; i++) {
		gotoxy(x, y + i);
		for (int j = 0; j < 14; j++) {
			if (pingu[i][j] == '1') {
				gotoxy(x + j, y + i);
				textcolor(WHITE, color1);
				printf(" ");
			}
			else if (pingu[i][j] == '2') {
				gotoxy(x + j, y + i);
				textcolor(WHITE, color2);
				printf(" ");
			}
		}
	}
}

void drawTitleText(int x, int y, int color1, int color2)
{
	for (int i = 0; i < 7; i++) {
		gotoxy(x, y + i);
		for (int j = 0; j < 62; j++) {
			if (title1[i][j] == '1') {
				gotoxy(x + j, y + i);
				textcolor(WHITE, color1);
				printf(" ");
			}
		}
	}
	for (int i = 0; i < 7; i++) {
		gotoxy(x + 30, y + 9 + i);
		for (int j = 0; j < 76; j++) {
			if (title2[i][j] == '1') {
				gotoxy(x + 30 + j, y + 9 + i);
				textcolor(WHITE, color2);
				printf(" ");
			}
		}
	}
}

void titleDraw(int x, int y) {
	drawBox(15, 5, SCREEN_WIDTH + 15, SCREEN_HEIGHT + 8);

	drawTitleText(x + 1, y, RED1, YELLOW1);
	drawTitleText(x, y, RED2, YELLOW2);

	drawCloud(30, 22);
	drawPingu(SCREEN_WIDTH / 2 + 30 + 1, 10, GRAY2, RED2);
	drawPingu(SCREEN_WIDTH / 2 + 30, 10, BLACK, RED1);

	gotoxy(SCREEN_WIDTH / 2 + 13, 15);
	textcolor(BLACK, WHITE);
	printf("  타자 연습 게임");
	gotoxy(SCREEN_WIDTH / 2 + 13, 16);
	printf("- 핑구를 구해라! -");
}

void clearCharacter(int x, int y) {
	for (int i = 0; i < 10; i++) {
		gotoxy(x, y + i);
		for (int j = 0; j < 20; j++) {
			gotoxy(x + j, y + i);
			printf(" ");  // 공백으로 대체하여 펭귄을 지움
		}
	}
}

char penguin[10][20] = {
	"0000000000111111110",
	"0000000000022111111",
	"0000000003311111111",
	"0000000000111111111",
	"0000000000022221111",
	"0000000000221111111",
	"0000000000211121111",
	"0000000002222221111",
	"0000000004444441110",
};
void drawCharacter(int x, int y) {
	for (int i = 0; i < 10; i++) {
		gotoxy(x, y + i);
		for (int j = 0; j < 20; j++) {
			if (penguin[i][j] == '0') {
				textcolor(WHITE, WHITE);
				printf(" ");
			}
			if (penguin[i][j] == '1') {
				textcolor(WHITE, BLACK);
				printf(" ");
			}
			if (penguin[i][j] == '2') {
				textcolor(WHITE, GRAY1);
				printf(" ");
			}
			if (penguin[i][j] == '3') {
				textcolor(WHITE, RED1);
				printf(" ");
			}
			if (penguin[i][j] == '4') {
				textcolor(WHITE, YELLOW1);
				printf(" ");
			}

		}
		printf("\n");
	};
}
void selectedOp(int selectedOption);

int menuDraw() {
	int x = 75;
	int y = 28;
	int selectedMenu = 1; // 선택된 메뉴 번호
	gotoxy(x - 2, y); // (48,28)
	printf("> 게임시작");
	gotoxy(x, y + 2); // (48,30)
	printf("기록 보기");
	gotoxy(x, y + 4); // (48,32)
	printf("도움말");
	gotoxy(x, y + 6); // (48,34)
	printf("종료");
	while (1) {
		unsigned char ch;
		if (kbhit() == 1) {
			ch = getch();
			if (ch == SPECIAL1 || ch == SPECIAL2) { // 만약 특수키
				ch = getch();
				switch (ch) {
				case UP:
					if (y > 28) {
						gotoxy(x - 2, y);
						printf(" ");
						gotoxy(x - 2, y -= 2);
						printf(">");
						selectedMenu--;
					}
					break;
				case DOWN:
					if (y < 34) {
						gotoxy(x - 2, y);
						printf(" ");
						gotoxy(x - 2, y += 2);
						printf(">");
						selectedMenu++;
					}
					break;
				}
			}
			if (ch == EMPTY)
				return selectedMenu;
		}

	}
}

void rankingShow(void) {
	system("title 랭킹 확인");
	if (access("record.txt", 0) == -1) {
		MessageBox(NULL, TEXT("The file does not exist."), NULL, NULL);
		return;
	}

	FILE* fp = fopen("record.txt", "r");
	if (fp == NULL) {
		printf("Failed to open file for reading.\n");
		return;
	}

	LinkedList* lnk = (LinkedList*)malloc(sizeof(LinkedList));
	lnk->cur = NULL;
	lnk->head = NULL;
	lnk->tail = NULL;

	int tempScore = 0;
	int tempLevel = 0;
	int tempMin = 0;
	int tempSec = 0;

	while (fscanf(fp, "%d / %d : %d / %d", &tempScore, &tempMin, &tempSec, &tempLevel) == 4) {
		insertData(lnk, tempScore, tempLevel, tempMin, tempSec);
	}
	gotoxy(SCREEN_WIDTH / 2 +5, 7);
	printf("*** RANKING ***");
	drawBox(20, 5, SCREEN_WIDTH + 5, SCREEN_HEIGHT + 13);
	gotoxy(SCREEN_WIDTH / 2 - 10, 10);
	textcolor(BLACK, WHITE); 
	printf("SCORE               TIME               LEVEL");
	printScore(lnk);

	fclose(fp);
	free(lnk); // 동적 할당 해제

	Sleep(1000);
	return;
}

void printScore(LinkedList* lnk) {
	RinkInfo* cur, ** ptr;
	int cnt = 0;  // 개수 초기값을 0으로 설정
	int i, j;
	RinkInfo* temp;
	cur = lnk->head;
	while (cur != NULL) {
		cnt++;
		cur = cur->next;
	}

	ptr = (RinkInfo**)malloc(sizeof(RinkInfo*) * cnt);
	for (i = 0, cur = lnk->head; i < cnt; i++) {
		ptr[i] = cur;
		if (cur->next == NULL) break;
		cur = cur->next;
	}

	for (i = 0; i < cnt - 1; i++) {
		for (j = i + 1; j < cnt; j++) {
			if (ptr[i]->score < ptr[j]->score) {  // 점수를 내림차순으로 비교
				temp = ptr[i];
				ptr[i] = ptr[j];
				ptr[j] = temp;
			}
		}
	}
	for (i = 0; i < cnt - 1; i++) {
		for (j = i + 1; j < cnt; j++) {
			if (ptr[i]->min < ptr[j]->min || (ptr[i]->min == ptr[j]->min && ptr[i]->sec < ptr[j]->sec)) {
				temp = ptr[i];
				ptr[i] = ptr[j];
				ptr[j] = temp;
			}
		}
	}
	for (i = 0; i < cnt - 1; i++) {
		for (j = i + 1; j < cnt; j++) {
			if (ptr[i]->level < ptr[j]->level) {  // 레벨을 내림차순으로 비교
				temp = ptr[i];
				ptr[i] = ptr[j];
				ptr[j] = temp;
			}
		}
	}

	for (i = 0; i < cnt; i++) {
		gotoxy(SCREEN_WIDTH / 2 - 10, 12 + i);
		printf("%-20d%02d:%02d               %2d\n", ptr[i]->score, ptr[i]->min, ptr[i]->sec, ptr[i]->level);
	}
	textcolor(BLACK, WHITE);
	gotoxy(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 + 24);
	printf("메인화면으로 돌아가려면 스페이스바를 누르세요\n");
	char ch = getch();
	if (ch == EMPTY) {
		cls(BLACK, WHITE);
		titleDraw(20, 10);
		int selectedOption = menuDraw();
		selectedOp(selectedOption);
	}

	Sleep(10000);
	free(ptr); // 동적 할당 해제
}

void insertData(LinkedList* lnk, int score, int level, int min, int sec) {
	RinkInfo* newRNK = (RinkInfo*)malloc(sizeof(RinkInfo));
	newRNK->score = score;
	newRNK->level = level;
	newRNK->min = min;
	newRNK->sec = sec;
	newRNK->next = NULL;
	if (lnk->head == NULL && lnk->tail == NULL) {
		lnk->head = lnk->tail = newRNK;
	}
	else {
		lnk->tail->next = newRNK;
		lnk->tail = newRNK;
	}
}

Word words[MAX_NUM_WORD];

char dictionary[MAX_NUM_WORD][MAX_WORD_LENGTH] = { // 120개
	"apple", "banana", "culture", "exel", "education", "affair", "effect", "source", "system", "study",
	"glory", "inner", "pollution", "liberty", "object", "section", "rabbit", "cow", "dog", "mango",
	"bear", "dolphin", "pro", "junior", "senior", "class", "university", "score", "grade", "advice",
	"me", "like", "love", "danger", "balance", "climate", "chance", "gallery", "folk", "people",
	"fruit", "hometown", "hunter", "turtle", "report", "holiday", "army", "schedule", "scene", "giraffe",
	"hawk", "however", "monarchy", "avid", "begin", "frenzied", "nippy", "irrational", "elephant", "computer",
	"butterfly", "happiness", "on", "football", "universe", "sun", "earth", "moon", "telephone", "water",
	"soccer", "juice", "chocolate", "beautiful", "confident", "landscape", "language", "excellent", "paradice", "dive",
	"friendship", "exquisite", "you", "snow", "zucchini", "pneumonia", "rhythm", "poppy", "nose", "triangle",
	"coding", "fine", "flower", "guitar", "drum", "piano", "ice cream", "kangaroo", "journey", "lion",
	"rainbow", "tiger", "sunflower", "orange", "mountain", "penguin", "notebook","register", "honey", "koala",
	"off", "octopus", "umbrella", "island", "quokka", "chip", "why", "quinoa","quasar", "do" };

void clearInput(int x, int y);
void gameOver();

void initWords()
{
	for (int i = 0; i < MAX_NUM_WORD; i++) {
		strcpy(words[i].value, dictionary[i]);
		words[i].length = strlen(words[i].value) + 1;
		words[i].x = 14 + rand() % (SCREEN_WIDTH - 23);
		words[i].y = 4;
		words[i].spawn = 0;
		words[i].alive = 1;
		words[i].colorNum = 0;
		words[i].blink = 8;
	}
}

void drawScore(int* score) {
	gotoxy(80, 1);
	textcolor(RED1, WHITE);
	printf("Score : %d", *score);
	textcolor(BLACK, WHITE);
}
void drawLevel(int* level) {
	gotoxy(55, 1);
	textcolor(BLACK, YELLOW2);
	printf("Level : %d", *level);
}

void spawnWords(int* cnt, int* level)
{
	if (*cnt == 100) *level += 1;
	else if (*cnt == 80) *level += 1;
	else if (*cnt == 60) *level += 1;
	else if (*cnt == 30) *level += 1;
	else if (*cnt == 0) return;

	if (*cnt == 0) {
		return;
	}
	int randNum = rand() % 10 + 1; // 1 ~ 10
	int wordIdx = rand() % MAX_NUM_WORD; // 0 ~ 119

	// 중복 단어 생성 제어
	while (words[wordIdx].spawn == 1) {
		wordIdx = rand() % MAX_NUM_WORD;
		randNum = rand() % 10 + 1;
	}

	words[wordIdx].spawn = 1;
	words[wordIdx].colorNum = randNum;
	(*cnt)--;
}

void drawWords()
{
	for (int i = 0; i < MAX_NUM_WORD; i++) {
		if (words[i].spawn == 1 && words[i].alive == 1) {
			if (words[i].colorNum < 8) {
				textcolor(BLACK, WHITE);
				gotoxy(words[i].x, words[i].y);
				printf("%s", words[i].value);
			}
			else if (words[i].colorNum >= 8 && words[i].colorNum < 9) {
				textcolor(BLUE1, WHITE);
				gotoxy(words[i].x, words[i].y);
				printf("%s", words[i].value);
			}
			else if (words[i].colorNum >= 9) {
				textcolor(GREEN1, WHITE);
				gotoxy(words[i].x, words[i].y);
				printf("%s", words[i].value);
			}
		}
	}
}

int cloud = 5;
void drawWord(int idx)
{
	gotoxy(words[idx].x, words[idx].y);
	printf("%s", words[idx].value);
}

void deleteWord(int idx)
{
	gotoxy(words[idx].x, words[idx].y);
	for (int j = 0; j < words[idx].length; j++) {
		printf(" ");
	}
}

void updateWords()
{
	for (int i = 0; i < MAX_NUM_WORD; i++) {
		// 단어가 생성되었고 살아있다면 y축으로 한 칸 이동
		if (words[i].spawn == 1 && words[i].alive == 1) {
			// 단어가 화면 밖으로 벗어난 경우 단어 삭제
			if (words[i].y + 1 >= SCREEN_HEIGHT) {
				// 여기에 캐릭터 구름 하나 없어지는 코드 추가하면 됨
				for (int i = 0; i < cloud; i++) {
					clearCloud((SCREEN_WIDTH / 2) + 50 + 10 * i, 42);
				}
				clearCharacter((SCREEN_WIDTH - 35) + cloud * 10, 33);
				cloud--;
				for (int i = 0; i < cloud; i++) {
					drawCloud((SCREEN_WIDTH / 2) + 50 + 10 * i, 42);
				}
				drawCharacter((SCREEN_WIDTH - 35) + cloud * 10, 33);

				deleteWord(i);
				words[i].alive = 0;

			}
			else {
				// 아직 화면 밖으로 나가지 않고 살아있는 경우 y축으로 이동
				deleteWord(i);
				words[i].y++;
			}
		}
	}
}

int deletedColor = 0;
int checkWord(char buf[])
{
	int inputX = SCREEN_WIDTH / 2 - 46;
	int inputY = 27;

	for (int i = 0; i < MAX_NUM_WORD; i++) {
		if (strcmp(buf, words[i].value) == 0) {
			deleteWord(i);
			words[i].alive = 0;
			if (words[i].colorNum >= 8 && words[i].colorNum < 9) deletedColor = 1; // 파랑색
			else if (words[i].colorNum >= 9) deletedColor = 2; // 초록색

			else if (words[i].colorNum < 8) deletedColor = 0; // 검은색
			return 1;
		}
	}

	return 0;
}

void drawInput(int x, int y, char buf[])
{
	gotoxy(x, y);
	printf("%s", buf);
}

void clearInput(int x, int y)
{
	gotoxy(x, y);
	for (int i = 0; i < MAX_WORD_LENGTH; i++) {
		printf(" ");
	}
}
int cnt;

void inputBox(int x, int y) {
	gotoxy(x, y); printf("┌─────────────────────────────────────────┐\n");
	gotoxy(x, y +1); printf("│                                                                                  │\n");
	gotoxy(x, y +2); printf("└─────────────────────────────────────────┘\n");
}
void keyboardInput(char buf[], int* bufIdx, int* level, int* score)
{
	gotoxy(10, 34);
	printf("입력: ");
	int inputX = SCREEN_WIDTH / 2 - 44;
	int inputY = 34;
	char ch;
	if (kbhit() == 1) {
		gotoxy(3 + cnt++, 27);
		ch = getch();

		if (ch == ENTER) {
			if (checkWord(buf) == 1) {
				strcpy(buf, "");
				*bufIdx = 0;
				switch (*level) {
				case 1:
					*score += 10;
					clearInput(inputX, inputY);
					break;
				case 2:
					*score += 20;
					clearInput(inputX, inputY);
					break;
				case 3:
					*score += 30;
					break;
				case 4:
					*score += 40;
					break;
				case 5:
					*score += 50;
					break;

				}
			}
		}
		else if (ch == BACKSPACE) {
			if (*bufIdx != 0) {
				buf[--(*bufIdx)] = ' ';
				clearInput(inputX, inputY);
			}
		}
		else {
			if (*bufIdx < MAX_WORD_LENGTH - 1) {
				buf[(*bufIdx)++] = ch;
			}
		}

		buf[*bufIdx] = '\0';
		drawInput(inputX, inputY, buf);
	}
}

void clearCloud(int x, int y)
{	
	int i = 0;
	gotoxy(x, y);
	textcolor(WHITE, WHITE);
	for (int j = 0; j < 6; j++) {
		printf(" ");
	}
	textcolor(WHITE, WHITE);
	printf(" ");

	gotoxy(x - 1, y + 1);
	textcolor(WHITE, WHITE);
	for (int j = 0; j < 8; j++) {
		printf(" ");
	}
	textcolor(WHITE, WHITE);
	printf(" ");

	gotoxy(x, y + 2);
	textcolor(WHITE, WHITE);
	for (int j = 0; j < 6; j++) {
		printf(" ");
	}
	textcolor(WHITE, WHITE);
	printf(" ");

	textcolor(WHITE, WHITE);
}

void drawCloud(int x, int y)
{
	int i = 0;
	gotoxy(x, y);
	textcolor(WHITE, BLUE2);
	for (int j = 0; j < 6; j++) {
		printf(" ");
	}
	textcolor(WHITE, BLUE1);
	printf(" ");

	gotoxy(x - 1, y + 1);
	textcolor(WHITE, BLUE2);
	for (int j = 0; j < 8; j++) {
		printf(" ");
	}
	textcolor(WHITE, BLUE1);
	printf(" ");

	gotoxy(x, y + 2);
	textcolor(WHITE, BLUE2);
	for (int j = 0; j < 6; j++) {
		printf(" ");
	}
	textcolor(WHITE, BLUE1);
	printf(" ");

	textcolor(BLACK, WHITE);
}

void drawSlowInfo() {
	gotoxy(140, 13);
	for (int i = 0; i < 2; i++) {
		textcolor(WHITE, BLUE2);
		printf(" ");
	}
	textcolor(BLACK, WHITE);
	printf(" 느리게");
}
void drawAddCloudInfo() {
	gotoxy(140, 16);
	for (int i = 0; i < 2; i++) {
		textcolor(WHITE, GREEN1);
		printf(" ");
	}
	textcolor(BLACK, WHITE);
	printf(" 얼음생성");
}
int isSccess() {
	for (int i = 0; i < MAX_NUM_WORD; i++) {
		if (words[i].spawn != 1 && words[i].alive != 0) {
			return 0;
		}
	}
	return 1;
}
void gameOver1(int* success, int* min, int* sec, int* score) {
	if (success) {
		PlaySound(TEXT("C:\\Users\\User\\Music\\success1.wav"), NULL, SND_FILENAME | SND_ASYNC);
		gotoxy(SCREEN_WIDTH / 2 +5, 10);
		textcolor(BLACK, YELLOW2);
		printf("GAME CLEAR!!");
		gotoxy(SCREEN_WIDTH / 2 - 3, 12);
		textcolor(BLACK, WHITE);
		printf("Time : %02d:%02d     ", *min, *sec);
		printf("score : %d", *score);
		drawCharacter(SCREEN_WIDTH / 2, 17);

		for (int i = 0; i < 10; i++) {
			gotoxy(10, SCREEN_HEIGHT / 2 + 11 + i);
			for (int j = 0; j < 140; j++) {
				textcolor(WHITE, BLUE2);
				printf(" ");
			}
			printf("\n");
		}

	}
}

void gameOver2(int* min, int* sec, int* score, int* level) {
	PlaySound(TEXT("C:\\Users\\User\\Music\\incomplete.wav"), NULL, SND_FILENAME | SND_ASYNC);
	gotoxy(70, 5);
	textcolor(RED2, WHITE);
	printf("LOSE...");
	gotoxy(50, 10);
	textcolor(BLACK, WHITE);
	printf("Time : %d:%d      ", *min, *sec);
	printf("Level : %d       ", *level);
	printf("score : %d\n", *score);
	gotoxy(SCREEN_WIDTH / 2 - 10, 15);
	textcolor(BLACK, WHITE);
	printf("얼음이 녹아 핑구의 집이 없어졌어요ㅠㅠ\n");
	for (int i = 0; i < 4; i++) {
		gotoxy(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 7 + i);
		for (int j = 0; j < 20; j++) {
			if (penguin[i][j] == '0') {
				textcolor(WHITE, WHITE);
				printf(" ");
			}
			if (penguin[i][j] == '1') {
				textcolor(WHITE, BLACK);
				printf(" ");
			}
			if (penguin[i][j] == '2') {
				textcolor(WHITE, GRAY1);
				printf(" ");
			}
			if (penguin[i][j] == '3') {
				textcolor(WHITE, RED1);
				printf(" ");
			}
		}
		printf("\n");
	};
	for (int i = 0; i < 10; i++) {
		gotoxy(10, SCREEN_HEIGHT / 2 + 11 + i);
		for (int j = 0; j < 140; j++) {
			textcolor(WHITE, BLUE2);
			printf(" ");
		}
		printf("\n");
	}
	textcolor(BLACK, WHITE);
	gotoxy(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 + 24);
	printf("메인화면으로 돌아가려면 스페이스바를 누르세요\n");
	char ch = getch();
	if (ch == EMPTY) {
		cls(BLACK, WHITE);
		titleDraw(20,10);
		int selectedOption = menuDraw();
		selectedOp(selectedOption);
	}
}

void saveRec(int* min, int* sec, int* score, int *level) {
	FILE* fp = fopen("record.txt", "a");
	if (fp == NULL) {
		printf("Failed to open file for writing.\n");
		return;
	}
	fprintf(fp, "%d / %d : %d / %d\n", *score, *min, *sec, *level);
	fclose(fp);
}

void setFrame(int* frame_count, int* wordCnt, int* level, int speed)
{
	if (deletedColor == 1) {
		if (*frame_count % 30 == 0) {
			spawnWords(wordCnt, level);
			updateWords();
			drawWords();
		}
	}
	if (deletedColor == 0 && *frame_count % speed == 0) {
		spawnWords(wordCnt, level);
		updateWords();
		drawWords();
	}
	if (deletedColor == 2 && *frame_count % speed == 0) {
		spawnWords(wordCnt, level);
		updateWords();
		drawWords();
	}
}

void gameStart() {
	PlaySound(TEXT("C:\\Users\\User\\Music\\playsound.wav"), NULL, SND_FILENAME | SND_ASYNC);
	clock_t start = 0, now = 0;
	clock_t duration = 0, sec = 0, min = 0;
	start = clock();
	srand(time(NULL));
	initWords();
	int level = 1;
	int score = 0;
	drawSlowInfo();
	drawAddCloudInfo();
	inputBox(SCREEN_WIDTH / 2 - 48, 33);

	drawCharacter(SCREEN_WIDTH +15, 33);
	for (int i = 0; i < cloud; i++) {
		drawCloud((SCREEN_WIDTH / 2) + 50 + 10 * i, 42);
	}

	char buf[MAX_WORD_LENGTH];
	int bufIdx = 0;
	int frame_count = 0;
	int wordCnt = MAX_NUM_WORD;
	while (1)
	{
		now = clock();
		duration = now - start;
		sec = (duration / CLOCKS_PER_SEC);
		min = (sec % 3600) / 60;
		sec %= 60;
		gotoxy(14, 1);
		textcolor(MAGENTA1, WHITE);
		printf("TIME ");
		textcolor(BLACK, WHITE);
		printf("%02d:%02d\r", min, sec);
		drawLevel(&level);
		drawScore(&score);
		keyboardInput(buf, &bufIdx, &level, &score, &wordCnt);

		if (level == 1) {
			setFrame(&frame_count, &wordCnt, &level, 5);
		}
		else if (level == 2) {
			setFrame(&frame_count, &wordCnt, &level, 10);
		}
		else if (level == 3) {
			setFrame(&frame_count, &wordCnt, &level, 15);
		}
		else if (level == 4) {
			setFrame(&frame_count, &wordCnt, &level, 12);
		}
		else if (level == 5) {
			setFrame(&frame_count, &wordCnt, &level, 8);
		}
		if (deletedColor == 2 && cloud != 5 && frame_count % 20 ==0) {
			for (int i = 0; i < cloud; i++) {
				clearCloud((SCREEN_WIDTH / 2) + 50 + 10 * i, 42);
				clearCharacter((SCREEN_WIDTH - 35) + cloud * 10, 33);
			}
			cloud = cloud + 1;
			for (int i = 0; i < cloud; i++) {
				drawCloud((SCREEN_WIDTH / 2) + 50 + 10 * i, 42);
				drawCharacter((SCREEN_WIDTH - 35) + cloud * 10, 33);
			}
		}

		// 깜빡이게 하는 부분
		for (int i = 0; i < MAX_NUM_WORD; i++) {
			if (words[i].spawn == 1 && words[i].alive == 0) {
				if (words[i].blink > 0) {
					if (words[i].blink % 2 != 0) {
						deleteWord(i);
					}
					else {
						drawWord(i);
					}
					words[i].blink--;
				}
			}
		}
		Sleep(100);
		frame_count++;
		
		int n = isSccess();
		if (n == 1) {
			saveRec(&min, &sec, &score, &level);
			cls(BLACK, WHITE);
			gameOver1(&n, &min, &sec, &score, &level);
		}
		if (cloud == 0) {
			saveRec(&min, &sec, &score, &level);
			cls(BLACK, WHITE);
			gameOver2(&min, &sec, &score, &level);
			Sleep(60000); 
		}
	}
}

void gameInfo(int x, int y) {
	drawBox(22, 5, SCREEN_WIDTH + 5, SCREEN_HEIGHT + 13);
	textcolor(BLACK, WHITE);
	gotoxy(x, y); printf("                                         ** 게임목표 **");
	gotoxy(x, y + 1); printf("                지구온난화 현상으로부터 얼음이 녹아 펭귄 '핑구'가 위험에 처했어요");
	gotoxy(x, y + 2); printf("                             단어를 입력해 핑구를 구해주세요!");
	gotoxy(x, y + 3); printf("\n");
	gotoxy(x, y + 4); printf("                                         ** 게임방법 **");
	gotoxy(x, y + 5); printf("\n");
	gotoxy(x, y + 6); printf("                - 산성비처럼 단어가 화면 위쪽에서 아래쪽으로 떨어지는 형태로 진행됩니다.");
	gotoxy(x, y + 8); printf("                - 화면에 나타난 단어를 정확하게 타이핑하여 입력해야 합니다.");
	gotoxy(x, y + 10); printf("                - 주어진 시간 내에 단어를 입력하지 못했다면, 얼음이 하나씩  녹아내립니다.");
	gotoxy(x, y + 12); printf("                - 올바른 단어를 입력하면 점수가 상승합니다. ");
	gotoxy(x, y + 14); printf("                (LV.1 : 10점, LV2 : 20점, LV3 : 30점, LV4 : 40점, LV5 : 50점)");
	gotoxy(x, y + 16); printf("                - 레벨 당 주어진 개수의 단어를 입력하면 레벨이 상승합니다.");
	gotoxy(x, y + 18); printf("                 (LV.1 ~ LV.3은 단어 20개, LV.4 ~ LV.5는 단어 30개)");
	gotoxy(x, y + 20); printf("                - 레벨이 올라갈수록 단어가 더욱 빠르게 떨어집니다.");
	gotoxy(x, y + 22); printf("                - 랜덤으로 생성되는 파란색 단어와 초록색 단어를 입력하면 좋은 효과가 발생합니다");
	gotoxy(x, y + 24); printf("                 파란색 단어 -> 대략 4초간 단어가 느리게 떨어집니다.");
	gotoxy(x, y + 26); printf("                 초록색 단어 -> 얼음이 하나씩 다시 생성됩니다.");
	gotoxy(x, y + 28); printf("                               게임을 즐기시고 핑구를 구해주세요!");
	gotoxy(x, y + 31); printf("                          선택화면으로 돌아가려면 SPACE바를 눌러주세요");
	drawCharacter(100, y + 18);
	drawCloud(110, y + 27);

	char ch = getch();
	if (ch == EMPTY) {
		cls(BLACK, WHITE);
		titleDraw(20, 10);
		int selectedOption = menuDraw();
		selectedOp(selectedOption);
	}
}

void selectedOp(int selectedOption)
{
	switch (selectedOption) {
	case 1:
		cls(BLACK, WHITE);
		drawBox(12, 3, SCREEN_WIDTH, SCREEN_HEIGHT);
		gameStart();
		break;
	case 2:
		cls(BLACK, WHITE);
		rankingShow();
		break;
	case 3:
		cls(BLACK, WHITE);
		gameInfo(24,8);
		break;
	case 4:
		cls(BLACK, WHITE);
		exit(0);
		break;
	}
}

int main()
{
	cls(BLACK, WHITE);
	removeCursor(); // 커서를 안보이게 한다
	init();
	PlaySound(TEXT("C:\\Users\\User\\Music\\playsound.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	titleDraw(20,10);
	int selectedOption = menuDraw();
	selectedOp(selectedOption);

	return 0;
}