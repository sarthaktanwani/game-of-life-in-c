#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <string.h>

#define HEIGHT  1000
#define WIDTH   1000   

int canvas[HEIGHT][WIDTH];
int canvas_diff[HEIGHT][WIDTH];
//char level[] = " .cod8#";

//need terminal encoding to be changed from
// UTF-8 to Obsolete Encoding Cyrilic IBM855
// to print the block character with ascii
//code 219
int level[] = {' ', 219};
int current_state[] = {' ', '.', '-', '+', 'c', 'o', 'a', 'A', '@', '#'};
int columns = WIDTH;
int rows = HEIGHT;

#define LEVEL_COUNT (int)((sizeof(level) / sizeof(level[0])) - 1)
#define STATE_COUNT (int)((sizeof(current_state) / sizeof(current_state[0])) - 1)

void parse_input_arguments(int argc, const char* argv[]);
void random_fill(void);
void fill_diff(void);
void apply_diff(void);
void transition(char state);
void (*startWorld)(void);
void horizontal_fill(void);
void rectangle_fill(void);
int count_neighbors_in_subgrid(int r, int c);
float randf(void);
int emod(int a, int b);

void disp(void);

int main(int argc, const char*argv[]) {
    
    parse_input_arguments(argc, argv);

    srand(time(NULL));
    (*startWorld)();
    disp();
    while(1) {
        fill_diff();
        apply_diff();
        disp();
        usleep(5 * 1000 * 15);
    }
    
    return 0;
}

void parse_input_arguments(int argc, const char*argv[]) {
    extern char* optarg;
    int option;
    while(1) {
        static struct option long_options[] =
        {
          {"columns", required_argument, 0, 'r'},
          {"rows",    required_argument, 0, 'c'},
          {"dead",  required_argument, 0, 'd'},
          {"alive",  required_argument, 0, 'a'},
          {"start-world",    required_argument, 0, 's'},
          {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;
        
        option = getopt_long (argc, (char * const *)argv, "r:c:d:a:s:", long_options, &option_index);

        if(option == -1) {
            break;
        }
    
        switch(option) {
            case 'r':
                    /**
                     * @brief we need to remove 2 from the total number of lines
                     * to properly print on the entire command line window.
                     * 
                     * 1. Printing the entire world matrix print 1 extra line +(-1)
                     * 3. There is 1 line for the cursor on the last line     +(-1)
                     * TOTAL:                                                 =(-2)
                     */
                    rows = atoi(optarg) - 2;
                    printf("You entered number of rows\n");
                    break;
            case 'c':
                    columns = atoi(optarg);
                    printf("You entered number of columns\n");
                    break;
            case 'd':
                    level[0] = current_state[0] = (int )*optarg;
                    for(int i = 0; i <= LEVEL_COUNT; i++) {
                        printf("level[%d]=%c\t", i, level[i]);
                    }
                    printf("\n");
                    break;    
            case 'a':
                    level[LEVEL_COUNT] = (int )*optarg;
                    current_state[STATE_COUNT] = (int )*optarg;
                    for(int i = 0; i <= LEVEL_COUNT; i++) {
                        printf("level[%d]=%c\t", i, level[i]);
                    }
                    printf("\n");
                    break;
            case 's':
                    if(strcmp(optarg, "rectangle") == 0) {
                        startWorld = &rectangle_fill;
                    }
                    else if(strcmp(optarg, "horizontal") == 0) {
                        startWorld = &horizontal_fill;
                    } 
                    else {
                        startWorld = &random_fill;
                    } 
                    break;
            default:
                    printf("Error");
        }
    }
}

void disp(void) {
    fputc('\n', stdout);
    char c;
    for(int ii = 0; ii < rows; ii++) {
        for(int jj = 0; jj < columns; jj++) {
            //c = level[(int)(canvas[ii][jj] * LEVEL_COUNT)];
            // c = level[canvas[ii][jj]];
            c = ((canvas[ii][jj] == 0) ? level[0] : level[1]);
            fputc(c, stdout);
        }
        fputc('\n', stdout);
    }
}

void transition(char state) {
    fputc('\n', stdout);
    char c;
    for(int ii = 0; ii < rows; ii++) {
        for(int jj = 0; jj < columns; jj++) {
            c = ((canvas[ii][jj] == 0) ? current_state[0] : state);
            fputc(c, stdout);
        }
        fputc('\n', stdout);
    }
}

void random_fill(void) {
     for(int ii = rows/4; ii <= 3*rows/4; ii++) {
        for(int jj = columns/4; jj < 3*columns/4; jj++) {
            canvas[ii][jj] = rand() % 2;
        }
    }
}

void horizontal_fill(void) {
    for(int jj = columns/4; jj < 3*columns/4; jj++) {
        canvas[rows/2][jj] = 1; 
    }
}

void rectangle_fill(void) {
    for(int ii = rows/4; ii <= 3*rows/4; ii++)
        for(int jj = columns/4; jj < 3*columns/4; jj++) {
            canvas[ii][jj] = 1; 
        }
}

void apply_diff(void) {
    for(int ii = 0; ii < rows; ii++) {
        for(int jj = 0; jj < columns; jj++) {
            canvas[ii][jj] += canvas_diff[ii][jj];
        }
    }
}

/**
 * @brief Any live cell with fewer than two live neighbours dies, as if by underpopulation.
 *  Any live cell with two or three live neighbours lives on to the next generation.
 *  Any live cell with more than three live neighbours dies, as if by overpopulation.
 *  Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 * 
 */


void fill_diff(void) {
    int count_live_cells = 0;
    for(int ii = 0; ii < rows; ii++) {
        for(int jj = 0; jj < columns; jj++) {
            int current_cell = canvas[ii][jj];
            count_live_cells = count_neighbors_in_subgrid(ii, jj);
            if(current_cell == 0) {
                if(count_live_cells == 3) {
                    canvas_diff[ii][jj] = 1;
                }
                else {
                    canvas_diff[ii][jj] = 0;
                }
            }
            else if(current_cell == 1) {
                if(count_live_cells > 3 || count_live_cells < 2) {
                    canvas_diff[ii][jj] = -1;
                }
                else {
                    canvas_diff[ii][jj] = 0;
                }
            }
            else {
                canvas_diff[ii][jj] = 0;
            }
        }
    }
}

int count_neighbors_in_subgrid(int curr_row, int curr_col) {
    int count = 0;
    for(int ii = (curr_row - 1); ii <= (curr_row + 1); ii++) {
        for(int jj = (curr_col - 1); jj <= (curr_col + 1); jj++) {
            int r = emod(ii, rows);
            int c = emod(jj, columns);
            if(ii == curr_row && jj == curr_col) { 
                continue;
            }
            else if(canvas[r][c] == 1) {
                count++;
            }
        }
    }
    return count;
}

float randf(void) {
    return (float) rand() / (float) RAND_MAX;
}

int emod(int a, int b)
{
    return (a%b + b)%b;
}
