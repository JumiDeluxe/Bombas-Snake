#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>

#define COLOR_END   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define DEFAULT_BOARD_WIDTH 20
#define DEFAULT_BOARD_HEIGHT 16



typedef struct board_tile
{
    unsigned int occupied : 1;
    unsigned int point : 1;
    unsigned int snake : 1;
    unsigned int bomb : 1;
} board_tile;


board_tile board[DEFAULT_BOARD_HEIGHT][DEFAULT_BOARD_WIDTH];
int points = 0;

void prepare_board(int width, int height) {

    for(int i = 0; i < height; i++)
    {
        board[0][i].occupied = 1;
        board[0][i].point = 0;
        board[0][i].snake = 0;
        board[0][i].bomb = 0;
        board[width-1][i].occupied = 1;
        board[width-1][i].point = 0;
        board[width-1][i].snake = 0;
        board[width-1][i].bomb = 0;
    }

    for(int i = 0; i < width; i++)
    {
        board[i][0].occupied = 1;
        board[i][0].point = 0;
        board[i][0].snake = 0;
        board[i][0].bomb = 0;
        board[i][height-1].occupied = 1;
        board[i][height-1].point = 0;
        board[i][height-1].snake = 0;
        board[i][height-1].bomb = 0;
    }


    for(int i = 1; i < width-2; i++) {
        for(int j = 1; j < height-2; j++) {
            board[i][j].occupied = 0;
            board[i][j].point = 0;
            board[i][j].snake = 0;
            board[i][j].bomb = 0;
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
}

struct elem
{
    int x;
    int y;
    struct elem *next;
};

struct elem* create_elem(int x, int y)
{
    board[x][y].snake = 1;
    board[x][y].occupied = 1;

    struct elem* temp;
    temp=(struct elem*) malloc(sizeof(struct elem));
    temp->x = x;
    temp->y = y;
    temp->next = NULL;
    return temp;
}

struct elem* add_to_beginning(struct elem* list, int x, int y)
{
    board[x][y].snake = 1;
    board[x][y].occupied = 1;

    struct elem* new_elem;
    new_elem = create_elem(x, y);
    new_elem->next = list;
    return new_elem;
}

void remove_end(struct elem* list)
{
    struct elem *temp = list;

    if(temp->next == NULL)
    {
        board[temp->x][temp->y].snake = 0;
        board[temp->x][temp->y].occupied = 0;
        free(temp);
        return;
    }

    while(temp->next->next != NULL)
    {
        temp = temp->next;
    }
    board[temp->next->x][temp->next->y].snake = 0;
    board[temp->next->x][temp->next->y].occupied = 0;

    free(temp->next);
    temp->next = NULL;
}

/*
void print_snake_coordinates(struct elem* list)
{
    if(list->next == NULL)
    {
        printf("\n x %d y %d", list->x, list->y);
        if(board[list->x][list->y].occupied) printf("\nzacheckowane\n");
        else printf("niecheck");
        return;
    }
    printf("\n x %d y %d", list->x, list->y);
    print_snake_coordinates(list->next);
}*/

int generate_random_number(int lower, int upper)
{
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

void generate_point(int width, int height)
{
    width--;
    height--;
    int x, y;

    do
    {
        x = generate_random_number(1, width);
        y = generate_random_number(1, height);
    }
    while (board[x][y].occupied);

    board[x][y].point = 1;
}

void collect_point(int x, int y)
{
    board[x][y].point = 0;
    generate_point(DEFAULT_BOARD_WIDTH, DEFAULT_BOARD_HEIGHT);
}

int check_next_tile(int x, int y)
{
    //0 - occupied, 1 - point, 2 - empty
    if(board[x][y].occupied) return 0;
    else if(board[x][y].point) return 1;
    else return 2;
}

void free_all_elems(struct elem* list) {
    while(list->next != NULL) {
        remove_end(list);
    }
}

void play()
{
    int active_x = 3;
    int active_y = DEFAULT_BOARD_HEIGHT/2;
    struct elem* snake = create_elem(1, active_y);
    snake = add_to_beginning(snake, 2, active_y);
    snake = add_to_beginning(snake, 3, active_y);

    printf("\nKliknij 'x' by zakończyć działanie programu\n");

    generate_point(DEFAULT_BOARD_WIDTH, DEFAULT_BOARD_HEIGHT);

    char direction = 'd';

    display_board(DEFAULT_BOARD_WIDTH, DEFAULT_BOARD_HEIGHT);

    for(;;)
    {
        scanf("\n%c", &direction);

        //printf("%d", (int)capture);

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
            printf("%sGAME OVER\n", RED);
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
    }
}

int main()
{
    prepare_board(DEFAULT_BOARD_WIDTH, DEFAULT_BOARD_HEIGHT);

    play();
    printf("eee");
    return 0;
}