//
// Created by 13492 on 2022/11/2.
//

#ifndef UNTITLED_SNAKE_H
#define UNTITLED_SNAKE_H

#include <deque>
#include <vector>
#include <windows.h>

using namespace std;

struct snake_location {
    unsigned int x;
    unsigned int y;
};

typedef struct my_snake {
    deque<snake_location> snake;
    unsigned int direction;
    snake_location pop_loca;
    snake_location last_push;
} my_snake, *p_snake;

typedef struct food {
    unsigned int x;
    unsigned int y;
    unsigned int pop_x;
    unsigned int pop_y;
} my_food, *p_food;

typedef struct graph {
    vector<vector<int>> graph;
    unsigned int width;
    unsigned int height;
} my_graph, *p_graph;

typedef struct handle {
//    HANDLE put, buf;
    HANDLE put;
    HANDLE *p;
    bool use_buf;
    CONSOLE_CURSOR_INFO cci;
    COORD coord;
    DWORD bytes;
} my_handle, *p_handle;

typedef struct my_info {
    unsigned int time;
    unsigned int score;
} my_info, *p_info;

class snake {
private:
    p_snake ske;
    p_food food;
    p_graph graph;
    p_handle handle;
    p_info p_my_info;

public:
    snake(void);

    int get_food(unsigned int *x, unsigned int *y);

    int display_graph(void);

    int create_graph(void);

    int start(void);

    int move(void);

    int get_new_location(int *x, int *y);

    bool is_vaild(unsigned int x, unsigned int y);

    bool key_input(void);

    int snake_init(unsigned int width, unsigned int height);

    bool print_end(void);

    bool print_start(void);

    bool print_succ(void);

    int update_graph(void);

    int display_score(void);
};


#endif //UNTITLED_SNAKE_H
