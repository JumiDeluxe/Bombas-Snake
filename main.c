#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>
#include "scores.h"
#include "utilities.h"

#define COLOR_END   "\033[0m"
#define RED     "\033[31m"      // Red
#define YELLOW  "\033[33m"      // Yellow
#define WHITE   "\033[37m"      // White
#define BOLDGREEN   "\033[1m\033[32m"      // Bold Green
/*
#define BLACK   "\033[30m"      // Black
#define GREEN   "\033[32m"      // Green
#define BLUE    "\033[34m"      // Blue
#define MAGENTA "\033[35m"      // Magenta
#define CYAN    "\033[36m"      // Cyan
#define BOLDBLACK   "\033[1m\033[30m"      // Bold Black
#define BOLDRED     "\033[1m\033[31m"      // Bold Red
#define BOLDYELLOW  "\033[1m\033[33m"      // Bold Yellow
#define BOLDBLUE    "\033[1m\033[34m"      // Bold Blue
#define BOLDMAGENTA "\033[1m\033[35m"      // Bold Magenta
#define BOLDCYAN    "\033[1m\033[36m"      // Bold Cyan
#define BOLDWHITE   "\033[1m\033[37m"      // Bold White
*/

#define DEFAULT_BOARD_WIDTH 20
#define DEFAULT_BOARD_HEIGHT 16



typedef struct board_tile
{
    unsigned int occupied : 1;
    unsigned int point : 1;
    unsigned int snake : 1;
} board_tile;

struct elem
{
    int x;
    int y;
    struct elem *next;
};


board_tile board[DEFAULT_BOARD_HEIGHT][DEFAULT_BOARD_WIDTH];
struct elem* bombs = NULL;
int points = 0;
char name[32] = {0};
int free_tiles = (DEFAULT_BOARD_HEIGHT-1)*(DEFAULT_BOARD_WIDTH-1) - 4; //used to calculate how many bombs can be placed (at start 3 snake tiles + 1 point tile)

board_tile snake_tile() {
    board_tile tile;
    tile.occupied = 1;
    tile.point = 0;
    tile.snake = 1;

    return tile;
}

board_tile occupied_tile() {
    board_tile tile;
    tile.occupied = 1;
    tile.point = 0;
    tile.snake = 0;

    return tile;
}

board_tile point_tile() {
    board_tile tile;
    tile.occupied = 0;
    tile.point = 1;
    tile.snake = 0;

    return tile;
}

board_tile empty_tile() {
    board_tile tile;
    tile.occupied = 0;
    tile.point = 0;
    tile.snake = 0;

    return tile;
}

void prepare_board(int width, int height) {

    for(int i = 0; i < height; i++)
    {
        board[0][i] = occupied_tile();
        board[width-1][i] = occupied_tile();
    }

    for(int i = 0; i < width; i++)
    {
        board[i][0] = occupied_tile();
        board[i][height-1] = occupied_tile();
    }


    for(int i = 1; i < width-2; i++) {
        for(int j = 1; j < height-2; j++) {
            board[i][j] = empty_tile();
        }
    }
}

void display_board(int width, int height) {
for(int j = 0; j < height; j++) {
    for(int i = 0; i < width; i++) {
            if(board[i][j].point) printf("%s#%s", WHITE, COLOR_END);
            else if(!board[i][j].occupied) printf("%s#%s", RED, COLOR_END);
            else if(board[i][j].snake) printf("%s#%s", BOLDGREEN, COLOR_END);
            else printf("%s#%s", YELLOW, COLOR_END);
        }
        putchar('\n');
    }
    putchar('\n');
    clear_screen();
}

struct elem* alloc_elem(int x, int y, struct elem* next) {
    struct elem* new_elem;
    new_elem = (struct elem*) malloc(sizeof(struct elem));
    new_elem->x = x;
    new_elem->y = y;
    new_elem->next = next;
    return new_elem;
}

struct elem* create_elem(int x, int y, struct elem* next)
{
    free_tiles--;
    board[x][y] = snake_tile();

    return alloc_elem(x, y, next);
}

struct elem* add_to_beginning(struct elem* list, int x, int y)
{
    free_tiles--;
    board[x][y] = snake_tile();

    return create_elem(x, y, list);
}

void remove_end(struct elem* list)
{
    struct elem *temp = list;

    if(temp->next == NULL)
    {
        board[temp->x][temp->y] = empty_tile();
        free(temp);
        return;
    }

    while(temp->next->next != NULL)
    {
        temp = temp->next;
    }
    board[temp->next->x][temp->next->y] = empty_tile();

    free(temp->next);
    temp->next = NULL;
    free_tiles++;
}

void free_all_elems(struct elem* list) {
    struct elem* prev;
    while (list != NULL)
    {
       prev = list;
       list = list->next;
       free(prev);
    }
}

void generate_point(int width, int height, bool isPoint) //0 - bomb, 1 - point
{
    width--;
    height--;
    int x, y;

    do
    {
        x = generate_random_number(1, width);
        y = generate_random_number(1, height);
    }
    while (board[x][y].occupied || board[x][y].point);

    if(isPoint) board[x][y] = point_tile();
    else {
        board[x][y] = occupied_tile();
    }
}

void collect_point(int x, int y)
{
    board[x][y].point = 0;
    generate_point(DEFAULT_BOARD_WIDTH, DEFAULT_BOARD_HEIGHT, true);
    //after collecting a point generate new bomb
    generate_point(DEFAULT_BOARD_WIDTH, DEFAULT_BOARD_HEIGHT, false);
    free_tiles--;
}

int check_next_tile(int x, int y)
{
    //0 - occupied, 1 - point, 2 - empty
    if(board[x][y].occupied) return 0;
    else if(board[x][y].point) return 1;
    else return 2;
}

void play()
{
    srand ( time(NULL) );
    prepare_board(DEFAULT_BOARD_WIDTH, DEFAULT_BOARD_HEIGHT);
    int active_x = 3;
    int active_y = DEFAULT_BOARD_HEIGHT/2;
    struct elem* snake = create_elem(1, active_y, NULL);
    snake = add_to_beginning(snake, 2, active_y);
    snake = add_to_beginning(snake, 3, active_y);

    printf("\nKliknij 'x' by zakończyć działanie programu\n");
    generate_point(DEFAULT_BOARD_WIDTH, DEFAULT_BOARD_HEIGHT, true);

    display_board(DEFAULT_BOARD_WIDTH, DEFAULT_BOARD_HEIGHT);

    struct termios t;
    tcgetattr(0, &t);
    t.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &t);
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
    char direction = 'd';

    for(;;)
    {
        read (0, &direction, 1);

        printf("Score: %d\n", points);
        switch(direction)
        {
        case 'w':
            active_y--;
            break;
        case 'a':
            active_x--;
            break;
        case 's':
            active_y++;
            break;
        case 'd':
            active_x++;
            break;
        case 'x':
            free_all_elems(snake);
            return;
        }

        switch(check_next_tile(active_x, active_y))
        {
        case 0:
            free_all_elems(snake);
            clear_screen();
            printf("%sGAME OVER\n", RED);
            add_new_score(points, name);
            return;
        case 1:
            collect_point(active_x, active_y);
            points++;
            break;
        default:
            remove_end(snake);
        }

        snake = add_to_beginning(snake, active_x, active_y);
        display_board(DEFAULT_BOARD_WIDTH, DEFAULT_BOARD_HEIGHT);

        usleep(200000);
    }
}

int menu_choice() {
    int opcja;
    system("figlet SNAKE");
    printf("Wybierz opcję:\n 1 - graj\n 2 - wyświetl ranking\n");
    echo_allowed(false);
    scanf("%d", &opcja);
    if(opcja < 1 || opcja > 2) return 1;
    echo_allowed(true);
    clear_screen();
    return opcja;
}

int main()
{
    //print_scores();
    switch (menu_choice())
    {
        case 1:
            printf("Wprowadź swoje imie (minimum 3 znaki)\n");
            int i = 0;
            name[0] = getchar();
            while(i < 31 && (name[i] = getchar()) != '\n') {
                i++;
            }
            name[i] = '\n';
            remove_spaces(name);
            if(strlen(name) < 3) strcpy(name, "anon");

            echo_allowed(false);
            play();
            echo_allowed(true);
            break;
        case 2:
            read_scores();
            break;
    }

    return 0;
}
