#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <ctype.h>

#define MAX_OPTIONS 100
#define MAX_LINE_LENGTH 256

typedef struct {
    char name[64];
    char description[256];
    int value;
    int type;  // 0=bool, 1=int, 2=string
    char string_value[256];
} config_option;

config_option options[MAX_OPTIONS];
int option_count = 0;
int current_selection = 0;
int max_y, max_x;

void init_screen(void) {
    initscr();
    start_color();
    use_default_colors();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);    // Selected item
    init_pair(2, COLOR_WHITE, COLOR_BLACK);   // Normal item
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);  // Header
    curs_set(0);
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, max_y, max_x);
}

void cleanup_screen(void) {
    endwin();
}

void load_config(void) {
    FILE *fp = fopen("include/config.h", "r");
    if (!fp) {
        printf("Error: Could not open config.h\n");
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "#define CONFIG_")) {
            char name[64], value[256];
            int type = 0;
            
            // Parse the line
            if (sscanf(line, "#define %s %s", name, value) == 2) {
                strcpy(options[option_count].name, name);
                
                // Determine type and value
                if (strcmp(value, "1") == 0 || strcmp(value, "0") == 0) {
                    type = 0;  // Boolean
                    options[option_count].value = atoi(value);
                } else if (isdigit(value[0]) || value[0] == '-') {
                    type = 1;  // Integer
                    options[option_count].value = atoi(value);
                } else {
                    type = 2;  // String
                    strcpy(options[option_count].string_value, value);
                }
                
                options[option_count].type = type;
                option_count++;
            }
        }
    }
    fclose(fp);
}

void save_config(void) {
    FILE *fp = fopen("include/config.h", "w");
    if (!fp) {
        printf("Error: Could not open config.h for writing\n");
        exit(1);
    }

    fprintf(fp, "#ifndef CONFIG_H\n#define CONFIG_H\n\n");
    
    for (int i = 0; i < option_count; i++) {
        if (options[i].type == 0) {
            fprintf(fp, "#define %s %d\n", options[i].name, options[i].value);
        } else if (options[i].type == 1) {
            fprintf(fp, "#define %s %d\n", options[i].name, options[i].value);
        } else {
            fprintf(fp, "#define %s %s\n", options[i].name, options[i].string_value);
        }
    }
    
    fprintf(fp, "\n#endif // CONFIG_H\n");
    fclose(fp);
}

void draw_menu(void) {
    clear();
    
    // Draw header
    attron(COLOR_PAIR(3));
    mvprintw(0, 0, "ArcOS Kernel Configuration");
    mvprintw(1, 0, "Use arrow keys to navigate, Enter to toggle, S to save, Q to quit");
    attroff(COLOR_PAIR(3));
    
    // Draw options
    int start_y = 3;
    for (int i = 0; i < option_count; i++) {
        if (i == current_selection) {
            attron(COLOR_PAIR(1));
        } else {
            attron(COLOR_PAIR(2));
        }
        
        char value_str[32];
        if (options[i].type == 0) {
            sprintf(value_str, "[%s]", options[i].value ? "X" : " ");
        } else if (options[i].type == 1) {
            sprintf(value_str, "%d", options[i].value);
        } else {
            strcpy(value_str, options[i].string_value);
        }
        
        mvprintw(start_y + i, 2, "%s: %s", options[i].name, value_str);
        attroff(COLOR_PAIR(1) | COLOR_PAIR(2));
    }
    
    refresh();
}

void handle_input(void) {
    int ch;
    while ((ch = getch()) != 'q') {
        switch (ch) {
            case KEY_UP:
                if (current_selection > 0) current_selection--;
                break;
            case KEY_DOWN:
                if (current_selection < option_count - 1) current_selection++;
                break;
            case '\n':  // Enter key
                if (options[current_selection].type == 0) {
                    options[current_selection].value = !options[current_selection].value;
                } else if (options[current_selection].type == 2) {
                    echo();
                    curs_set(1);
                    char input[256];
                    mvprintw(max_y - 2, 0, "Enter new value for %s: ", options[current_selection].name);
                    getnstr(input, sizeof(input) - 1);
                    strncpy(options[current_selection].string_value, input, sizeof(options[current_selection].string_value) - 1);
                    options[current_selection].string_value[sizeof(options[current_selection].string_value) - 1] = '\0';
                    noecho();
                    curs_set(0);
                }
                break;
            case 's':
            case 'S':
                save_config();
                mvprintw(max_y - 1, 0, "Configuration saved!");
                refresh();
                napms(1000);
                break;
        }
        draw_menu();
    }
}

int main(void) {
    init_screen();
    load_config();
    draw_menu();
    handle_input();
    cleanup_screen();
    return 0;
}