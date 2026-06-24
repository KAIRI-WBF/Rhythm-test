#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <ctype.h>
#include <conio.h>

#define MAX_NOTES 20
#define NOTE_SYMBOLS 4

typedef struct {
    char symbol;
    int waitTime;
    int judged;
} Note;

int bpm = 120;
int beatInterval;

// 彻底阻断所有输入缓存写入
void disableEcho() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
    SetConsoleMode(hStdin, mode);
}
void enableEcho() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    mode |= (ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
    SetConsoleMode(hStdin, mode);
}

// 函数声明
void setColor(int color);
void resetColor();
void printTitle();
void printInstructions();
void selectBPM();
int getBarSpeed(int bpm);
void generateNotes(Note notes[], int *count);
void playGame(Note notes[], int count);
void printResult(int score, int total, int perfect, int perfectPlus, int good, int ok, int miss);
char waitAnyKey();
char getChoiceNoEnter();

int main() {
    Note notes[MAX_NOTES];
    int noteCount = 0;
    char choice;

    srand((unsigned int)time(NULL));

    system("cls");
    printTitle();
    printInstructions();
    selectBPM();

    printf("\nPress ANY KEY to start game...");
    waitAnyKey();

    do {
        system("cls");
        generateNotes(notes, &noteCount);
        playGame(notes, noteCount);

        while (_kbhit()) _getch();

        printf("\n\nPlay again? Press [y] yes / [n] no: ");
        choice = getChoiceNoEnter();
        choice = tolower(choice);

    } while (choice == 'y');

    printf("\nThanks for playing! Goodbye!\n");
    waitAnyKey();
    return 0;
}

char waitAnyKey() {
    while (!_kbhit());
    return _getch();
}

char getChoiceNoEnter() {
    while (1) {
        if (_kbhit()) {
            char ch = tolower(_getch());
            if (ch == 'y' || ch == 'n') {
                return ch;
            }
        }
    }
}

void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void resetColor() {
    setColor(7);
}

void printTitle() {
    setColor(11);
    printf("+========================================================+\n");
    printf("|         RHYTHM TAP - COLOR FEEDBACK + EASY MODE        |\n");
    printf("+========================================================+\n");
    resetColor();
}

void printInstructions() {
    printf("\n[RULES - EASY MODE]\n");
    printf("   SEE THE NOTE -> PRESS THE KEY -> GET SCORE!\n\n");
    printf("   Keys: ");
    setColor(12); printf("[A] "); resetColor();
    setColor(10); printf("[S] "); resetColor();
    setColor(14); printf("[D] "); resetColor();
    setColor(13); printf("[F]\n\n");
    printf("   COLOR GUIDE:\n");
    setColor(13); printf("   PURPLE "); resetColor();
    printf("= PERFECT+ (time <= 20%% of bar)\n");
    setColor(10); printf("   GREEN  "); resetColor();
    printf("= PERFECT (time <= 40%% of bar)\n");
    setColor(14); printf("   YELLOW "); resetColor();
    printf("= GOOD (time <= 70%% of bar)\n");
    setColor(7); printf("   WHITE  "); resetColor();
    printf("= OK (time <= 85%% of bar)\n");
    setColor(12); printf("   RED    "); resetColor();
    printf("= MISS (time > 85%% of bar)\n\n");
    printf("   Just match the color when you tap!\n");
}

int getBarSpeed(int bpm) {
    if (bpm <= 80) return 2500;
    if (bpm <= 100) return 2200;
    if (bpm <= 120) return 1900;
    if (bpm <= 140) return 1600;
    if (bpm <= 160) return 1300;
    if (bpm <= 180) return 1100;
    if (bpm <= 200) return 900;
    if (bpm <= 220) return 750;
    if (bpm <= 240) return 600;
    return 500;
}

void selectBPM() {
    int choice;
    const char* bpmNames[] = {"VERY SLOW", "SLOW", "NORMAL", "FAST", "VERY FAST",
                              "INSANE", "IMPOSSIBLE", "LUDICROUS", "MACHINE", "GODLIKE"};
    int bpmOptions[] = {80, 100, 120, 140, 160, 180, 200, 220, 240, 260};
    int numOptions = 10;

    printf("\n+--------------------------------------------------------+\n");
    printf("|                 SELECT BPM                             |\n");
    printf("+--------------------------------------------------------+\n");

    for (int i = 0; i < numOptions; i++) {
        if (i == 2) {
            setColor(14);
            printf("  [%d] %d BPM - %s  <-- DEFAULT\n", i+1, bpmOptions[i], bpmNames[i]);
            resetColor();
        } else {
            printf("  [%d] %d BPM - %s\n", i+1, bpmOptions[i], bpmNames[i]);
        }
    }

    printf("\n  [0] CUSTOM BPM\n");
    printf("\nChoose BPM (1-10, 0 for custom): ");
    scanf("%d", &choice);
    while (_kbhit()) _getch();

    if (choice >= 1 && choice <= numOptions) {
        bpm = bpmOptions[choice - 1];
    } else if (choice == 0) {
        printf("Enter custom BPM (60-300): ");
        scanf("%d", &bpm);
        while (_kbhit()) _getch();
        if (bpm < 60) bpm = 60;
        if (bpm > 300) bpm = 300;
    } else {
        bpm = 120;
    }

    beatInterval = 60000 / bpm;

    setColor(10);
    printf("\n  BPM set to: %d  |  Beat interval: %dms\n", bpm, beatInterval);
    resetColor();

    int barSpeed = getBarSpeed(bpm);
    printf("  Response window: %dms (EASY MODE)\n\n", barSpeed);
}

void generateNotes(Note notes[], int *count) {
    char symbols[] = {'A', 'S', 'D', 'F'};
    *count = 10 + rand() % 6;

    int totalTime = 0;
    for (int i = 0; i < *count; i++) {
        notes[i].symbol = symbols[rand() % NOTE_SYMBOLS];
        int beats = 2 + rand() % 3;
        totalTime += beats * beatInterval;
        notes[i].waitTime = totalTime;
        notes[i].judged = 0;
    }
}

void playGame(Note notes[], int count) {
    disableEcho();

    int score = 0;
    int combo = 0;
    int currentNote = 0;
    int totalNotes = count;
    int perfectPlus = 0, perfect = 0, good = 0, ok = 0, miss = 0;
    int gameStartTime = GetTickCount();
    int noteStartTime = 0;
    int keyPressed = 0;
    char key = 0;
    int responseTime = 0;

    int barFullTime = getBarSpeed(bpm);
    // ★★★ 新的判定区间（基于时间比例）★★★
    // PERFECT+:  0% ~ 20%
    // PERFECT:  20% ~ 40%
    // GOOD:     40% ~ 70%
    // OK:       70% ~ 85%
    // MISS:     85% ~ 100%

    printf("\nGET READY...\n");
    printf("BPM: %d\n", bpm);
    Sleep(1000);
    printf("3...");
    Sleep(800);
    printf("2...");
    Sleep(800);
    printf("1...");
    Sleep(800);
    printf("GO!\n\n");
    Sleep(500);

    while (currentNote < count) {
        int currentTime = GetTickCount() - gameStartTime;

        if (currentTime >= notes[currentNote].waitTime && !notes[currentNote].judged) {

            noteStartTime = GetTickCount();
            keyPressed = 0;
            key = 0;

            int waitTime = barFullTime;

            while (!keyPressed && (GetTickCount() - noteStartTime < waitTime)) {

                system("cls");

                printf("+========================================================+\n");
                printf("|                  TAP NOW!                            |\n");
                printf("+========================================================+\n\n");

                printf("BPM: %d   SCORE: %d   COMBO: %d\n", bpm, score, combo);
                printf("PERFECT+:%d  PERFECT:%d  GOOD:%d  OK:%d  MISS:%d\n", 
                       perfectPlus, perfect, good, ok, miss);
                printf("NOTE: %d/%d\n\n", currentNote + 1, totalNotes);

                int elapsed = GetTickCount() - noteStartTime;
                int remain = waitTime - elapsed;
                if (remain < 0) remain = 0;

                float ratio = (float)elapsed / waitTime;

                // ★★★ 5种判定区域 ★★★
                int boxColor;
                char* statusText;

                if (ratio <= 0.20) {
                    boxColor = 13;      // 紫色 - PERFECT+
                    statusText = "PERFECT+ ZONE";
                } else if (ratio <= 0.40) {
                    boxColor = 10;      // 绿色 - PERFECT
                    statusText = "PERFECT ZONE";
                } else if (ratio <= 0.70) {
                    boxColor = 14;      // 黄色 - GOOD
                    statusText = "GOOD ZONE";
                } else if (ratio <= 0.85) {
                    boxColor = 7;       // 白色 - OK
                    statusText = "OK ZONE";
                } else {
                    boxColor = 12;      // 红色 - MISS
                    statusText = "MISS ZONE";
                }

                setColor(boxColor);
                printf("  [%s]\n", statusText);
                resetColor();

                setColor(boxColor);
                printf("          +-----------+\n");
                printf("          |           |\n");
                printf("          |     %c     |\n", notes[currentNote].symbol);
                printf("          |           |\n");
                printf("          +-----------+\n");
                resetColor();

                printf("\n   Time: [");
                int barLen = (int)((1 - ratio) * 40);
                if (barLen > 40) barLen = 40;
                for (int i = 0; i < 40; i++) {
                    if (i < barLen) {
                        if (ratio <= 0.20) {
                            setColor(13);
                            printf("=");
                        } else if (ratio <= 0.40) {
                            setColor(10);
                            printf("=");
                        } else if (ratio <= 0.70) {
                            setColor(14);
                            printf("=");
                        } else if (ratio <= 0.85) {
                            setColor(7);
                            printf("=");
                        } else {
                            setColor(12);
                            printf("=");
                        }
                        resetColor();
                    } else {
                        printf(" ");
                    }
                }
                printf("] %d%%\n", (int)((1 - ratio) * 100));

                printf("\n   >>> PRESS [%c] NOW! <<<\n", notes[currentNote].symbol);
                printf("   (Match the color for best result!)\n");

                if (GetAsyncKeyState('A') & 0x8000) {
                    key = 'A';
                    keyPressed = 1;
                    responseTime = GetTickCount() - noteStartTime;
                    while (GetAsyncKeyState('A') & 0x8000) Sleep(10);
                } else if (GetAsyncKeyState('S') & 0x8000) {
                    key = 'S';
                    keyPressed = 1;
                    responseTime = GetTickCount() - noteStartTime;
                    while (GetAsyncKeyState('S') & 0x8000) Sleep(10);
                } else if (GetAsyncKeyState('D') & 0x8000) {
                    key = 'D';
                    keyPressed = 1;
                    responseTime = GetTickCount() - noteStartTime;
                    while (GetAsyncKeyState('D') & 0x8000) Sleep(10);
                } else if (GetAsyncKeyState('F') & 0x8000) {
                    key = 'F';
                    keyPressed = 1;
                    responseTime = GetTickCount() - noteStartTime;
                    while (GetAsyncKeyState('F') & 0x8000) Sleep(10);
                }

                Sleep(15);
            }

            // ★★★ 5种判定 ★★★
            if (keyPressed) {
                float ratio = (float)responseTime / waitTime;

                if (key == notes[currentNote].symbol) {
                    if (ratio <= 0.20) {
                        // PERFECT+ - 紫色
                        int points = 20 + (combo / 5) * 4;
                        score += points;
                        combo++;
                        perfectPlus++;

                        setColor(13);
                        printf("\n\n  [PERFECT+!] +%d pts  (COMBO x%d)  %dms",
                               points, combo, responseTime);
                        resetColor();
                        while (_kbhit()) _getch();
                    } else if (ratio <= 0.40) {
                        // PERFECT - 绿色
                        int points = 15 + (combo / 5) * 3;
                        score += points;
                        combo++;
                        perfect++;

                        setColor(10);
                        printf("\n\n  [PERFECT!] +%d pts  (COMBO x%d)  %dms",
                               points, combo, responseTime);
                        resetColor();
                        while (_kbhit()) _getch();
                    } else if (ratio <= 0.70) {
                        // GOOD - 黄色
                        score += 8;
                        combo++;
                        good++;

                        setColor(14);
                        printf("\n\n  [GOOD!] +8 pts  (COMBO x%d)  %dms",
                               combo, responseTime);
                        resetColor();
                        while (_kbhit()) _getch();
                    } else if (ratio <= 0.85) {
                        // OK - 白色
                        score += 3;
                        combo = 0;  // OK 会重置连击
                        ok++;

                        setColor(7);
                        printf("\n\n  [OK] +3 pts  %dms",
                               responseTime);
                        resetColor();
                        while (_kbhit()) _getch();
                    } else {
                        // MISS - 红色
                        score -= 3;
                        if (score < 0) score = 0;
                        combo = 0;
                        miss++;

                        setColor(12);
                        printf("\n\n  [MISS!] -3 pts (Too slow: %dms)", responseTime);
                        resetColor();
                        while (_kbhit()) _getch();
                    }
                } else {
                    setColor(12);
                    printf("\n\n  [WRONG!] Expected [%c], got [%c]",
                           notes[currentNote].symbol, key);
                    resetColor();
                    score -= 2;
                    if (score < 0) score = 0;
                    combo = 0;
                    miss++;
                    while (_kbhit()) _getch();
                }
            } else {
                setColor(12);
                printf("\n\n  [MISS!] Time out!");
                resetColor();
                score -= 3;
                if (score < 0) score = 0;
                combo = 0;
                miss++;
                while (_kbhit()) _getch();
            }

            notes[currentNote].judged = 1;
            currentNote++;

            int showTime = 700;
            if (bpm >= 180) showTime = 500;
            if (bpm >= 240) showTime = 400;
            if (currentNote < count) {
                int waitStart = GetTickCount();
                while(GetTickCount() - waitStart < showTime)
                {
                    while (_kbhit()) _getch();
                    Sleep(10);
                }
            }
        }

        while (_kbhit()) _getch();
        Sleep(10);
    }

    while (_kbhit()) _getch();
    printResult(score, totalNotes, perfectPlus, perfect, good, ok, miss);
}

void printResult(int score, int total, int perfectPlus, int perfect, int good, int ok, int miss) {
    system("cls");
    printf("+========================================================+\n");
    printf("|                   GAME OVER!                           |\n");
    printf("+========================================================+\n\n");

    printf("FINAL SCORE: ");
    setColor(14);
    printf("%d", score);
    resetColor();
    printf("\n\n");

    printf("SUMMARY:\n");
    setColor(13); printf("  PERFECT+: %d  ", perfectPlus); resetColor();
    setColor(10); printf("PERFECT: %d  ", perfect); resetColor();
    setColor(14); printf("GOOD: %d  ", good); resetColor();
    setColor(7); printf("OK: %d  ", ok); resetColor();
    setColor(12); printf("MISS: %d\n", miss); resetColor();
    printf("  Total Notes: %d\n\n", total);

    // ★★★ 计算准确率（PERFECT+ 和 PERFECT 算命中，GOOD 和 OK 算部分命中）★★★
    float accuracy = 0;
    if (total > 0) {
        accuracy = (float)(perfectPlus * 1.0 + perfect * 1.0 + good * 0.7 + ok * 0.4) / total * 100;
    }
    printf("  Accuracy: %.1f%%\n\n", accuracy);

    // ★★★ 新的评级系统 ★★★
    if (accuracy >= 85) {
        setColor(13);  // 紫色
        printf("  RANK: PERFECT+ - AMAZING!\n");
        resetColor();
    } else if (accuracy >= 70) {
        setColor(10);  // 绿色
        printf("  RANK: PERFECT - EXCELLENT!\n");
        resetColor();
    } else if (accuracy >= 60) {
        setColor(14);  // 黄色
        printf("  RANK: GOOD - NICE!\n");
        resetColor();
    } else if (accuracy >= 30) {
        setColor(7);   // 白色
        printf("  RANK: OK - KEEP PRACTICING!\n");
        resetColor();
    } else {
        setColor(12);  // 红色
        printf("  RANK: MISS - NEED MORE RHYTHM!\n");
        resetColor();
    }

    while (_kbhit()) _getch();
    enableEcho();
    printf("\nPress ANY KEY to continue...");
    waitAnyKey();
}