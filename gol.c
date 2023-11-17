#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define HEIGHT  30
#define WIDTH   100

int canvas[HEIGHT][WIDTH];
int canvas_diff[HEIGHT][WIDTH];
//char level[] = " .cod8#";

//need terminal encoding to be changed from
// UTF-8 to Obsolete Encoding Cyrilic IBM855
// to print the block character with ascii
//code 219
int level[] = {' ', 219};

#define LEVEL_COUNT ((sizeof(level) / sizeof(level[0])) - 1)

void random_fill(void);
void fill_diff(void);
void apply_diff(void);
void disp_diff(void);
void horizontal_fill(void);
void rectangle_fill(void);
int count_neighbors_in_subgrid(int r, int c);
float randf(void);
int emod(int a, int b);

void disp(void);

int main() {
    srand(time(NULL));
    random_fill(); 
    // horizontal_fill();
    // rectangle_fill();
    disp();
    while(1) {
        // disp();
        fill_diff();
        // disp_diff();
        apply_diff();
        disp();
        // fill_diff();
        // apply_diff();
        // disp();
        usleep(5 * 1000 * 15);
    }
    
    return 0;
}

void disp(void) {
    printf("\n------------------------------\n");
    char c;
    for(int ii = 0; ii < HEIGHT; ii++) {
        for(int jj = 0; jj < WIDTH; jj++) {
            //c = level[(int)(canvas[ii][jj] * LEVEL_COUNT)];
            c = level[canvas[ii][jj]];
            // c = level[2];
            printf("%c", c);
        }
        printf("\n");
    }
    printf("\n------------------------------\n");
}

void disp_diff(void) {
    printf("\n******************************\n");
    char c;
    for(int ii = 0; ii < HEIGHT; ii++) {
        for(int jj = 0; jj < WIDTH; jj++) {
            //c = level[(int)(canvas[ii][jj] * LEVEL_COUNT)];
            c = level[canvas_diff[ii][jj]];
            // c = level[2];
            printf("%d\t", canvas_diff[ii][jj]);
        }
        printf("\n");
    }
    printf("\n******************************\n");
}

void random_fill(void) {
    for(int ii = 0; ii < HEIGHT; ii++) {
        for(int jj = 0; jj < WIDTH; jj++) {
            canvas[ii][jj] = rand() % 2;
        }
    }
}

void horizontal_fill(void) {
    for(int jj = 0; jj < WIDTH; jj++) {
        canvas[0][jj] = 1; 
    }
}

void rectangle_fill(void) {
    for(int ii = HEIGHT/4; ii <= 3*HEIGHT/4; ii++)
        for(int jj = WIDTH/4; jj < 3*WIDTH/4; jj++) {
            canvas[ii][jj] = 1; 
        }
}

void apply_diff(void) {
    for(int ii = 0; ii < HEIGHT; ii++) {
        for(int jj = 0; jj < WIDTH; jj++) {
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
    // printf("&&&&&&&&&&&&&Neighbors in subgrid start:&&&&&&&&&&&&&\n");
    for(int ii = 0; ii < HEIGHT; ii++) {
        for(int jj = 0; jj < WIDTH; jj++) {
            int current_cell = canvas[ii][jj];
            count_live_cells = count_neighbors_in_subgrid(ii, jj);
            // printf("%d\t", count_live_cells);
            // canvas_diff[ii][jj] = count_live_cells;
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
        // printf("\n");
    }
    // printf("\n&&&&&&&&&&&&&Neighbors in subgrid end:&&&&&&&&&&&&&");
}

int count_neighbors_in_subgrid(int curr_row, int curr_col) {
    int count = 0;
    for(int ii = (curr_row - 1); ii <= (curr_row + 1); ii++) {
        for(int jj = (curr_col - 1); jj <= (curr_col + 1); jj++) {
            int r = emod(ii, HEIGHT);
            int c = emod(jj, WIDTH);
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