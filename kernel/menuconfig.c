#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define CONFIG_FILE ".config"
#define CONFIG_H_FILE "config.h"
#define MAX_OPTIONS 10

struct config_option {
    const char* name;
    const char* description;
    int enabled;
};

struct config_option options[] = {
    {"CONFIG_DEBUG", "Enable debug output", 0},
    {"CONFIG_SPLASH", "Enable splash screen", 0},
    {"CONFIG_INSTALLER", "Enable installer", 0},
    {"CONFIG_GRAPHICS", "Enable graphics support", 1},
    {"CONFIG_FILESYSTEM", "Enable filesystem support", 1},
    {NULL, NULL, 0}
};

void init_screen(void) {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Define color pairs
    init_pair(1, COLOR_BLUE, COLOR_BLACK);    // Title
    init_pair(2, COLOR_WHITE, COLOR_BLUE);    // Help text
    init_pair(3, COLOR_WHITE, COLOR_BLACK);   // Normal text
    init_pair(4, COLOR_BLACK, COLOR_WHITE);   // Selected item
}

void draw_box(int y, int x, int height, int width) {
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + width - 1, ACS_URCORNER);
    mvaddch(y + height - 1, x, ACS_LLCORNER);
    mvaddch(y + height - 1, x + width - 1, ACS_LRCORNER);

    for (int i = 1; i < width - 1; i++) {
        mvaddch(y, x + i, ACS_HLINE);
        mvaddch(y + height - 1, x + i, ACS_HLINE);
    }

    for (int i = 1; i < height - 1; i++) {
        mvaddch(y + i, x, ACS_VLINE);
        mvaddch(y + i, x + width - 1, ACS_VLINE);
    }
}

void draw_title(void) {
    attron(COLOR_PAIR(1));
    mvprintw(1, 2, "ArcOS v0.8k Configuration");
    attroff(COLOR_PAIR(1));
}

void draw_help(void) {
    attron(COLOR_PAIR(2));
    mvprintw(3, 2, "Use arrow keys to navigate, Space to toggle, Enter to select");
    attroff(COLOR_PAIR(2));
}

void draw_options(int selected) {
    int i;
    for (i = 0; options[i].name != NULL; i++) {
        if (i == selected) {
            attron(COLOR_PAIR(4));
        } else {
            attron(COLOR_PAIR(3));
        }
        mvprintw(5 + i, 2, "[%c] %s", options[i].enabled ? '*' : ' ', options[i].name);
        mvprintw(5 + i, 30, "%s", options[i].description);
        attroff(COLOR_PAIR(3) | COLOR_PAIR(4));
    }

    // Draw save/exit options
    attron(COLOR_PAIR(3));
    mvprintw(5 + i + 1, 2, "[S] Save and Exit");
    mvprintw(5 + i + 2, 2, "[Q] Exit without saving");
    attroff(COLOR_PAIR(3));
}

void save_config(void) {
    FILE* fp = fopen(CONFIG_FILE, "w");
    if (!fp) {
        mvprintw(LINES - 2, 2, "Error: Could not open .config file");
        return;
    }

    for (int i = 0; options[i].name != NULL; i++) {
        fprintf(fp, "%s=%c\n", options[i].name, options[i].enabled ? 'y' : 'n');
    }
    fclose(fp);

    // Generate config.h
    fp = fopen(CONFIG_H_FILE, "w");
    if (!fp) {
        mvprintw(LINES - 2, 2, "Error: Could not open config.h file");
        return;
    }

    fprintf(fp, "#ifndef CONFIG_H\n");
    fprintf(fp, "#define CONFIG_H\n\n");

    for (int i = 0; options[i].name != NULL; i++) {
        if (options[i].enabled) {
            fprintf(fp, "#define %s 1\n", options[i].name);
        }
    }

    fprintf(fp, "\n#endif // CONFIG_H\n");
    fclose(fp);

    mvprintw(LINES - 2, 2, "Configuration saved successfully");
}

void load_config(void) {
    FILE* fp = fopen(CONFIG_FILE, "r");
    if (!fp) return;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char* name = strtok(line, "=");
        char* value = strtok(NULL, "\n");
        if (name && value) {
            for (int i = 0; options[i].name != NULL; i++) {
                if (strcmp(options[i].name, name) == 0) {
                    options[i].enabled = (value[0] == 'y');
                    break;
                }
            }
        }
    }
    fclose(fp);
}

int main(void) {
    init_screen();
    load_config();

    int selected = 0;
    int running = 1;

    while (running) {
        clear();
        draw_box(0, 0, LINES - 2, COLS - 1);
        draw_title();
        draw_help();
        draw_options(selected);
        refresh();

        int ch = getch();
        switch (ch) {
            case KEY_UP:
                if (selected > 0) selected--;
                break;
            case KEY_DOWN:
                if (options[selected + 1].name != NULL) selected++;
                break;
            case ' ':
                if (options[selected].name != NULL) {
                    options[selected].enabled = !options[selected].enabled;
                }
                break;
            case 's':
            case 'S':
                save_config();
                running = 0;
                break;
            case 'q':
            case 'Q':
                running = 0;
                break;
        }
    }

    endwin();
    return 0;
} 