// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *category; char *text; } Note;
Note notes[10000]; int nnotes = 0;

int main() {
    char cmd[20], category[100], text[1000], word[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s", category);
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            notes[nnotes].category = strdup(category);
            notes[nnotes++].text = strdup(text);
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", category);
            for (int i = 0; i < nnotes; ) {
                if (!strcmp(notes[i].category, category)) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < nnotes - 1; j++) notes[j] = notes[j + 1];
                    nnotes--;
                } else {
                    i++;
                }
            }
        } else if (!strcmp(cmd, "KEEP")) {
            scanf("%s", category);
            for (int i = 0; i < nnotes; ) {
                if (strcmp(notes[i].category, category) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < nnotes - 1; j++) notes[j] = notes[j + 1];
                    nnotes--;
                } else {
                    i++;
                }
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", word);
            for (int i = 0; i < nnotes; i++) {
                if (strstr(notes[i].text, word)) {
                    printf("%s %s\n", notes[i].category, notes[i].text);
                }
            }
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i = 0; i < nnotes; i++) {
                printf("%s %s\n", notes[i].category, notes[i].text);
            }
        }
    }
    for (int i = 0; i < nnotes; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    return 0;
}