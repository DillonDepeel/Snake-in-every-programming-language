
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SNAKE_STEP_SIZE 5

#define SNAKE_PAIR 1
#define APPLE_PAIR 2
#define ALERT_PAIR 3

struct snake_t {
    int length;
    int max_length;
    int *body;
};

void move_snake(struct snake_t *snake, int y, int x);
int body_collision(struct snake_t *snake, int y, int x);
void grow_snake(struct snake_t *snake, int y, int x);
void clear_snake(WINDOW *game_window, struct snake_t *snake);
void print_snake(WINDOW *game_window, struct snake_t *snake);
void kill_snake(struct snake_t *snake);
void spawn_apple(WINDOW *game_window, int game_window_height, int game_window_width, int *apple_y, int *apple_x);
void game_over(WINDOW *game_window, int game_window_height, int game_window_width);
void print_debug(struct snake_t *snake);
struct snake_t *init_snake(int y, int x);

int main()
{
    time_t t;
    srand(time(&t));

    WINDOW *game_window;
    int game_window_height, game_window_width, game_window_y, game_window_x;
    int game_window_margin = 20;

    struct snake_t *snake;
    int apple_y, apple_x;
    int ch;
    int game_state = 1;
    int debug = 0;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(SNAKE_PAIR, COLOR_GREEN, COLOR_GREEN);
    init_pair(APPLE_PAIR, COLOR_RED, COLOR_RED);
    init_pair(ALERT_PAIR, COLOR_RED, COLOR_BLACK);
    refresh();

    game_window_height = LINES - game_window_margin / 2;
    game_window_width = COLS - (2 * game_window_margin);
    game_window_y = game_window_margin / 4;
    game_window_x = game_window_margin;

    game_window = newwin(game_window_height, game_window_width, game_window_y, game_window_x);
    box(game_window, 0, 0);
    
    mvwprintw(game_window, 0, 5, "| simple snake game |");

    snake = init_snake((rand() % (game_window_height - 2)) + 1, (rand() % (game_window_width - 2)) + 1);
    spawn_apple(game_window, game_window_height, game_window_width, &apple_y, &apple_x);

    wrefresh(game_window);

    do {

        if (ch == KEY_F(1)) {
            debug = 1;
        }

        if (debug) {
            print_debug(snake);
        }

        switch (ch) {
            case KEY_UP:
                if (body_collision(snake, snake->body[0] - 1, snake->body[1]))
                    game_state = 0;
                clear_snake(game_window, snake);
                move_snake(snake, snake->body[0] - 1, snake->body[1]);
                break;
            case KEY_DOWN:
                if (body_collision(snake, snake->body[0] + 1, snake->body[1]))
                    game_state = 0;
                clear_snake(game_window, snake);
                move_snake(snake, snake->body[0] + 1, snake->body[1]);
                break;
            case KEY_LEFT:
                if (body_collision(snake, snake->body[0], snake->body[1] - 1))
                    game_state = 0;
                clear_snake(game_window, snake);
                move_snake(snake, snake->body[0], snake->body[1] - 1);
                break;
            case KEY_RIGHT:
                if (body_collision(snake, snake->body[0], snake->body[1] + 1))
                    game_state = 0;
                clear_snake(game_window, snake);
                move_snake(snake, snake->body[0], snake->body[1] + 1);
                break;
        }

        if (snake->body[0] == apple_y && snake->body[1] == apple_x) {
            grow_snake(snake, apple_y, apple_x);
            spawn_apple(game_window, game_window_height, game_window_width, &apple_y, &apple_x);
        }

        print_snake(game_window, snake);

        if ((snake->body[0] <= 0 || snake->body[0] >= game_window_height - 1)
                || (snake->body[1] <= 0 || snake->body[1] >= game_window_width - 1)) {
            game_state = 0;
            break;
        }

    } while (game_state == 1 && (ch = getch()) != 'q');

    if (game_state == 0) {
        game_over(game_window, game_window_height, game_window_width);
        getch();
    }

    kill_snake(snake);
    endwin();

    return 0;
}

void game_over(WINDOW *game_window, int game_window_height, int game_window_width)
{
    wattron(game_window, COLOR_PAIR(ALERT_PAIR) | A_BLINK);
    mvwprintw(game_window, (game_window_height - 1) / 2, ((game_window_width - 1) - strlen("GAME OVER!")) / 2, "GAME OVER!");
    wattroff(game_window, COLOR_PAIR(ALERT_PAIR) | A_BLINK);
    wrefresh(game_window);
}

int body_collision(struct snake_t *snake, int y, int x)
{
    int i;
    for (i = 2; i < snake->length - 1; i += 2) {
        if (y == snake->body[i] && x == snake->body[i + 1])
            return 1;
    }
    return 0;
}

void move_snake(struct snake_t *snake, int y, int x)
{
    int i;
    for (i = snake->length - 3; i > 0; i -= 2) {
        snake->body[i + 2] = snake->body[i];
        snake->body[i + 1] = snake->body[i - 1];
    }
    snake->body[0] = y;
    snake->body[1] = x;
}

void grow_snake(struct snake_t *snake, int y, int x)
{
    if (snake->length >= snake->max_length) {
        snake->body = realloc(snake->body, (snake->length + (2 * SNAKE_STEP_SIZE)) * sizeof(int));
        snake->max_length = snake->length + (2 * SNAKE_STEP_SIZE);
    }
    snake->body[snake->length] = y;
    snake->body[snake->length + 1] = x;
    snake->length += 2;
}

void clear_snake(WINDOW *game_window, struct snake_t *snake)
{
    int i;
    for (i = 0; i < snake->length; i += 2) {
        mvwprintw(game_window, snake->body[i], snake->body[i + 1], " ");
    }
    wrefresh(game_window);
}

void print_snake(WINDOW *game_window, struct snake_t *snake)
{
    int i;
    for (i = 0; i < snake->length; i += 2) {
        wattron(game_window, COLOR_PAIR(SNAKE_PAIR));
        mvwprintw(game_window, snake->body[i], snake->body[i + 1], " ");
        wattroff(game_window, COLOR_PAIR(SNAKE_PAIR));
    }
    wrefresh(game_window);
}

struct snake_t *init_snake(int y, int x)
{
    struct snake_t *new_snake;

    new_snake = (struct snake_t*)malloc(sizeof(struct snake_t*));
    new_snake->length = 0;
    new_snake->max_length = (2 * SNAKE_STEP_SIZE);
    new_snake->body = (int*)malloc((2 * SNAKE_STEP_SIZE) * sizeof(int));

    grow_snake(new_snake, y, x);

    return new_snake;
}

void kill_snake(struct snake_t *snake)
{
    free(snake->body);
    free(snake);
}

void spawn_apple(WINDOW *game_window, int game_window_height, int game_window_width, int *apple_y, int *apple_x)
{
    *apple_y = (rand() % (game_window_height - 2)) + 1;
    *apple_x = (rand() % (game_window_width - 2)) + 1;
    wattron(game_window, COLOR_PAIR(APPLE_PAIR));
    mvwprintw(game_window, *apple_y, *apple_x, " ");
    wattroff(game_window, COLOR_PAIR(APPLE_PAIR));
    wrefresh(game_window);
}

void print_debug(struct snake_t *snake)
{
    int i;

    mvprintw(0, 5, "DEBUG");
    mvprintw(1, 5, "snake length: %d (max %d)", snake->length, snake->max_length);
    mvprintw(2, 5, "snake array: ");
    for (i = 0; i < snake->length - 1; i += 2) {
        printw("[ %3d %3d ] ", snake->body[i], snake->body[i + 1]);
    }
}
