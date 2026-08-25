/* F025.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} ClassStudent;

typedef struct {
    char *name;
    ClassStudent *students;
    int std_count;
    int std_cap;
} ClassRoom;

int main(void) {
    ClassRoom *rooms = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CLASS ", 6) == 0) {
            char *name = line + 6;
            char *n_name = strdup(name);
            if (n_name) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    ClassRoom *tmp = realloc(rooms, new_cap * sizeof(ClassRoom));
                    if (tmp) {
                        rooms = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_name);
                        continue;
                    }
                }
                rooms[count].name = n_name;
                rooms[count].students = NULL;
                rooms[count].std_count = 0;
                rooms[count].std_cap = 0;
                count++;
            }
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char *args = line + 8;
            char *space1 = strchr(args, ' ');
            if (space1) {
                *space1 = '\0';
                char *cname = args;
                char *rest = space1 + 1;
                char *space2 = strchr(rest, ' ');
                char *sname = NULL;
                char *snote = NULL;
                if (space2) {
                    *space2 = '\0';
                    sname = rest;
                    snote = space2 + 1;
                } else {
                    sname = rest;
                    snote = "";
                }
                for (int i = 0; i < count; i++) {
                    if (strcmp(rooms[i].name, cname) == 0) {
                        ClassRoom *cr = &rooms[i];
                        char *n_sname = strdup(sname);
                        char *n_snote = strdup(snote);
                        if (n_sname && n_snote) {
                            if (cr->std_count >= cr->std_cap) {
                                int new_scap = cr->std_cap == 0 ? 4 : cr->std_cap * 2;
                                ClassStudent *tmp = realloc(cr->students, new_scap * sizeof(ClassStudent));
                                if (tmp) {
                                    cr->students = tmp;
                                    cr->std_cap = new_scap;
                                } else {
                                    free(n_sname); free(n_snote);
                                    break;
                                }
                            }
                            cr->students[cr->std_count].name = n_sname;
                            cr->students[cr->std_count].note = n_snote;
                            cr->std_count++;
                        } else {
                            free(n_sname); free(n_snote);
                        }
                        break;
                    }
                }
                *space1 = ' ';
                if (space2) *space2 = ' ';
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char *args = line + 9;
            char *space1 = strchr(args, ' ');
            if (space1) {
                *space1 = '\0';
                char *fromC = args;
                char *rest = space1 + 1;
                char *space2 = strchr(rest, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *toC = rest;
                    char *sname = space2 + 1;
                    int f_idx = -1, t_idx = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(rooms[i].name, fromC) == 0) f_idx = i;
                        if (strcmp(rooms[i].name, toC) == 0) t_idx = i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        ClassRoom *fcr = &rooms[f_idx];
                        ClassRoom *tcr = &rooms[t_idx];
                        for (int j = 0; j < fcr->std_count; j++) {
                            if (strcmp(fcr->students[j].name, sname) == 0) {
                                if (tcr->std_count >= tcr->std_cap) {
                                    int new_scap = tcr->std_cap == 0 ? 4 : tcr->std_cap * 2;
                                    ClassStudent *tmp = realloc(tcr->students, new_scap * sizeof(ClassStudent));
                                    if (tmp) {
                                        tcr->students = tmp;
                                        tcr->std_cap = new_scap;
                                    } else {
                                        break;
                                    }
                                }
                                tcr->students[tcr->std_count] = fcr->students[j];
                                tcr->std_count++;
                                for (int k = j; k < fcr->std_count - 1; k++) {
                                    fcr->students[k] = fcr->students[k + 1];
                                }
                                fcr->std_count--;
                                break;
                            }
                        }
                    }
                    *space2 = ' ';
                }
                *space1 = ' ';
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *cname = args;
                char *sname = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(rooms[i].name, cname) == 0) {
                        ClassRoom *cr = &rooms[i];
                        for (int j = 0; j < cr->std_count; j++) {
                            if (strcmp(cr->students[j].name, sname) == 0) {
                                free(cr->students[j].name);
                                free(cr->students[j].note);
                                for (int k = j; k < cr->std_count - 1; k++) {
                                    cr->students[k] = cr->students[k + 1];
                                }
                                cr->std_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "DELETECLASS ", 12) == 0) {
            char *cname = line + 12;
            for (int i = 0; i < count; i++) {
                if (strcmp(rooms[i].name, cname) == 0) {
                    free(rooms[i].name);
                    for (int j = 0; j < rooms[i].std_count; j++) {
                        free(rooms[i].students[j].name);
                        free(rooms[i].students[j].note);
                    }
                    free(rooms[i].students);
                    for (int j = i; j < count - 1; j++) {
                        rooms[j] = rooms[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("CLASS %s\n", rooms[i].name);
                for (int j = 0; j < rooms[i].std_count; j++) {
                    printf("%s %s\n", rooms[i].students[j].name, rooms[i].students[j].note);
                }
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(rooms[i].name);
        for (int j = 0; j < rooms[i].std_count; j++) {
            free(rooms[i].students[j].name);
            free(rooms[i].students[j].note);
        }
        free(rooms[i].students);
    }
    free(rooms);
    return 0;
}
