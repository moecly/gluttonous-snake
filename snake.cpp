//
// Created by 13492 on 2022/11/2.
//

#include <iostream>
#include "snake.h"
#include <ctime>
#include <random>
#include <unistd.h>
#include <conio.h>

using namespace std;

#define PLACE_PIC "  "
#define SNAKE_PIC "□"
#define SNAKE_HEAD_PIC "■"
#define FOOD_PIC "●"
#define WALL_PIC "■"

#define ADD_TIME  50
#define MAX_TIME  777
#define ADD_SCORE 1000

#define DISPLAY_X 3
#define DISPLAY_Y 3
default_random_engine e;

typedef enum game_status {
    con,
    succ,
    end,
} game_status;

typedef enum directions {
    up,
    down,
    left,
    right,
} directions;

typedef enum info {
    place,
    snake,
    wall,
    food,
} info;

//typedef enum

/*
 * 构建
 */
snake::snake(void) {
    e.seed(time(nullptr));

    //创建新的控制台缓冲区
    this->handle = new my_handle();
    this->handle->put = GetStdHandle(STD_OUTPUT_HANDLE);

    //隐藏缓冲区的光标
    this->handle->coord = {0, 0};
    this->handle->bytes = 0;
    this->handle->cci.bVisible = 0;
    this->handle->cci.dwSize = 1;

    SetConsoleCursorInfo(this->handle->put, &this->handle->cci);
    this->handle->p = &this->handle->put;
    SetConsoleActiveScreenBuffer(this->handle->p);
}

int snake::snake_init(unsigned int width, unsigned int height) {
    this->graph = new my_graph();
    this->food = new my_food();
    this->ske = new my_snake();
    this->p_my_info = new my_info();
    this->p_my_info->time = 0;
    this->p_my_info->score = 0;

    this->graph->width = width + 2;
    this->graph->height = height + 2;
    this->ske->direction = directions::right;
    snake_location ske;
    ske.x = this->graph->width / 2;
    ske.y = this->graph->height / 2;
    this->ske->snake.push_back(ske);

    /* 刷新食物 */
    if (get_food(&this->food->x, &this->food->y) == game_status::succ)
        return game_status::succ;
    this->food->pop_x = this->food->x;
    this->food->pop_y = this->food->y;

    /* 创建数组 */
    create_graph();

    display_graph();
    return game_status::con;
}

bool is_number(string s) {
    unsigned int n = s.length();
    if (n == 0) {
        return false;
    }

    for (char ch: s) {
        if (ch < '0' || ch > '9') {
            return false;
        }
    }

    return true;
}

/*
 * 打印开始界面
 */
bool snake::print_start(void) {
//    SetConsoleActiveScreenBuffer(this->handle->put);
    system("cls");
    cout << "------------------------------------------------------" << endl;
    cout << "------------------Gluttonous snake--------------------" << endl;
    cout << "----------    1. select graph size    ----------------" << endl;
    cout << "----------    2. exit                 ----------------" << endl;
    cout << "------------------------------------------------------" << endl;
    cout << "------------------------------------------------------" << endl;
    cout << "------------------------------------------------------" << endl;
    unsigned char ch = cin.get();
    if (ch == '1') {
        string width, height;
        system("cls");
        cout << "width: ";
        cin >> width;
        if (!is_number(width)) {
            return print_start();
        }

        cout << "height: ";
        cin >> height;
        if (!is_number(height)) {
            return print_start();
        }

        if (snake_init(stoi(width), stoi(height)) == game_status::succ) {
            return game_status::succ;
        }
//        system("cls");
        return game_status::con;
    }

    if (ch == '2') {
        return game_status::end;
    }
    return print_start();
}

/*
 * 打印结束界面
 */
bool snake::print_end(void) {
//    SetConsoleActiveScreenBuffer(this->handle->put);
    system("cls");
    cout << "------------------------------------------------------" << endl;
    cout << "------------------Game over---------------------------" << endl;
    cout << "----------    1. restart game         ----------------" << endl;
    cout << "----------    2. exit                 ----------------" << endl;
    cout << "------------------------------------------------------" << endl;
    cout << "        " << "score: " << this->p_my_info->score << endl;
    cout << "------------------------------------------------------" << endl;
    unsigned char ch = getchar();
    if (ch == '1')
        return true;
    if (ch == '2')
        return false;

    return print_end();
}

bool snake::print_succ(void) {
//    SetConsoleActiveScreenBuffer(this->handle->put);
    system("cls");
    cout << "------------------------------------------------------" << endl;
    cout << "------------------Success!--------------------------" << endl;
    cout << "----------    1. restart game         ----------------" << endl;
    cout << "----------    2. exit                 ----------------" << endl;
    cout << "------------------------------------------------------" << endl;
    cout << "        " << "score: " << this->p_my_info->score << endl;
    cout << "------------------------------------------------------" << endl;
    unsigned char ch = getchar();
    if (ch == '1')
        return true;
    if (ch == '2')
        return false;
    return print_succ();
}


/*
 * 开始游戏
 */
int snake::start(void) {
    /* 初始化界面 */
    int status;
    while (true) {
        status = print_start();
        if (status == game_status::succ) {
            if (!print_succ()) {
                return false;
            }
        }
        if (status == game_status::end) {
            return false;
        }
        while ((status = move()) == game_status::con)
//            display_graph();
            update_graph();
        if (status == game_status::succ) {
            if (!print_succ()) {
                return false;
            }
        } else if (!print_end())
            return false;
    }
    return true;
}


int snake::get_new_location(int *x, int *y) {
    snake_location snake = this->ske->snake.front();
    *x = snake.x;
    *y = snake.y;
    switch (this->ske->direction) {
        case directions::up:
            *y -= 1;
            break;
        case directions::down:
            *y += 1;
            break;
        case directions::left:
            *x -= 1;
            break;
        case directions::right:
            *x += 1;
            break;
        default:
            break;
    }
    return true;
}

/*
 * 判断数据是否有效
 */
bool snake::is_vaild(unsigned int x, unsigned int y) {
    if (x == 0 || y == 0 || x == this->graph->width - 1 || y == this->graph->height - 1) {
        return false;
    }
    for (auto [a, b]: this->ske->snake) {
        if (x == a && y == b) {
            return false;
        }
    }
    return true;
}

/*
 * 键盘输入
 */
bool snake::key_input(void) {
    char ch, tmp;
    while (_kbhit()) {
        tmp = _getch();
        if (tmp == 'a' || tmp == 'A' || tmp == 'W' || tmp == 'w' || tmp == 'S' || tmp == 's' || tmp == 'D' ||
            tmp == 'd')
            ch = tmp;
    }

    if ((ch == 'A' || ch == 'a') && this->ske->direction != directions::right) {
        this->ske->direction = directions::left;
    } else if ((ch == 'W' || ch == 'w') && this->ske->direction != directions::down) {
        this->ske->direction = directions::up;
    } else if ((ch == 'S' || ch == 's') && this->ske->direction != directions::up) {
        this->ske->direction = directions::down;
    } else if ((ch == 'D' || ch == 'd') && this->ske->direction != directions::left) {
        this->ske->direction = directions::right;
    }
    return true;
}

/*
 * 移动
 */
int snake::move(void) {
    int new_x, new_y;

    key_input();

    get_new_location(&new_x, &new_y);
    if (!is_vaild(new_x, new_y)) {
        return game_status::end;
    }

    snake_location snake;
    snake.x = new_x;
    snake.y = new_y;

    this->ske->last_push = this->ske->snake.front();
    this->ske->snake.push_front(snake);

//    snake = this->ske->snake.back();
    if (this->graph->graph[new_x][new_y] == info::food) {
        this->food->pop_x = this->food->x;
        this->food->pop_y = this->food->y;
        if (get_food(&this->food->x, &this->food->y) == game_status::succ) {
            return game_status::succ;
        }
        this->p_my_info->time += ADD_TIME;
        this->p_my_info->time = MAX_TIME > this->p_my_info->time ? this->p_my_info->time : MAX_TIME;
        this->p_my_info->score += ADD_SCORE;
        this->graph->graph[this->food->x][this->food->y] = info::food;
        this->graph->graph[new_x][new_y] = info::snake;
        return game_status::con;
    }

    this->ske->pop_loca = this->ske->snake.back();
    this->graph->graph[new_x][new_y] = info::snake;
    this->graph->graph[snake.x][snake.y] = info::place;
    this->ske->snake.pop_back();

    return game_status::con;
}

void delay_ms(unsigned int t) {
    unsigned int i = 100000000 - t * 100000;
    for (; i > 0; i--)
        for (; t > 0; t--) {
        }
}

/*
 * 更新图
 */
int snake::update_graph(void) {
    if (this->food->pop_x != this->food->x || this->food->pop_y != this->food->y) {
        this->handle->coord.X = DISPLAY_X + this->food->pop_x * 2;
        this->handle->coord.Y = DISPLAY_Y + this->food->pop_y;
        WriteConsoleOutputCharacterA(*this->handle->p, PLACE_PIC, 2, this->handle->coord, &this->handle->bytes);

        this->handle->coord.X = DISPLAY_X + this->food->x * 2;
        this->handle->coord.Y = DISPLAY_Y + this->food->y;
        WriteConsoleOutputCharacterA(*this->handle->p, FOOD_PIC, 2, this->handle->coord, &this->handle->bytes);

        this->food->pop_x = this->food->x;
        this->food->pop_y = this->food->y;
    }

    if (this->ske->last_push.x != this->ske->pop_loca.x || this->ske->last_push.y != this->ske->pop_loca.y) {
        this->handle->coord.X = DISPLAY_X + this->ske->last_push.x * 2;
        this->handle->coord.Y = DISPLAY_Y + this->ske->last_push.y;
        WriteConsoleOutputCharacterA(*this->handle->p, SNAKE_PIC, 2, this->handle->coord, &this->handle->bytes);
    }

    this->handle->coord.X = DISPLAY_X + this->ske->pop_loca.x * 2;
    this->handle->coord.Y = DISPLAY_Y + this->ske->pop_loca.y;
    WriteConsoleOutputCharacterA(*this->handle->p, PLACE_PIC, 2, this->handle->coord, &this->handle->bytes);

    snake_location snake = this->ske->snake.front();

    this->handle->coord.X = DISPLAY_X + snake.x * 2;
    this->handle->coord.Y = DISPLAY_Y + snake.y;
    WriteConsoleOutputCharacterA(*this->handle->p, SNAKE_HEAD_PIC, 2, this->handle->coord, &this->handle->bytes);
    display_score();
    delay_ms(this->p_my_info->time);
}

int snake::display_score(void) {
    this->handle->coord.Y = this->graph->height / 2 + DISPLAY_Y;
    this->handle->coord.X = 7 + this->graph->width * 2 + DISPLAY_X;

    string s = to_string(this->p_my_info->score);
    int n = s.length();
    char score[n];

    strcpy(score, s.c_str());
    WriteConsoleOutputCharacterA(*this->handle->p, "score: ", 7, this->handle->coord, &this->handle->bytes);
    this->handle->coord.X += 7;
    WriteConsoleOutputCharacterA(*this->handle->p, score, n, this->handle->coord, &this->handle->bytes);

//    SetConsoleActiveScreenBuffer(*this->handle->p);
    delay_ms(this->p_my_info->time);
    return true;
}

/**
 * 显示贪吃蛇，地图，食物
 */
int snake::display_graph(void) {
//    this->handle->use_buf = !this->handle->use_buf;
    system("cls");
//    if (this->handle->use_buf) {
//        this->handle->p = &this->handle->buf;
//    } else {
//        this->handle->p = &this->handle->put;
//    }

    /* 显示蛇，地图，食物，墙 */
    this->handle->coord.X = DISPLAY_X;
    this->handle->coord.Y = DISPLAY_Y;
    for (unsigned int j = 0; j < this->graph->height; j++) {
        for (unsigned int i = 0; i < this->graph->width; i++) {
            if (this->graph->graph[i][j] == info::wall) {
                WriteConsoleOutputCharacterA(*this->handle->p, WALL_PIC, 2, this->handle->coord,
                                             &this->handle->bytes);
            } else if (this->graph->graph[i][j] == info::snake) {
                WriteConsoleOutputCharacterA(*this->handle->p, SNAKE_PIC, 2, this->handle->coord,
                                             &this->handle->bytes);
            } else if (this->graph->graph[i][j] == info::food) {
                WriteConsoleOutputCharacterA(*this->handle->p, FOOD_PIC, 2, this->handle->coord,
                                             &this->handle->bytes);
            } else {
                WriteConsoleOutputCharacterA(*this->handle->p, PLACE_PIC, 2, this->handle->coord, &this->handle->bytes);
            }
            this->handle->coord.X += 2;
        }
        this->handle->coord.Y += 1;
        this->handle->coord.X = 3;
    }

    snake_location snake = this->ske->snake.front();
    this->handle->coord.X = DISPLAY_X + snake.x * 2;
    this->handle->coord.Y = DISPLAY_Y + snake.y;
    WriteConsoleOutputCharacterA(*this->handle->p, SNAKE_HEAD_PIC, 2, this->handle->coord,
                                 &this->handle->bytes);
    /* 显示分数 */
    display_score();

    return true;
}

/*
 * 随机生成食物
 */
int snake::get_food(unsigned int *x, unsigned int *y) {
    if (this->ske->snake.size() >= (this->graph->width - 2) * (this->graph->height - 2))
        return game_status::succ;
    int val = e();
    *x = e() % (this->graph->width - 2) + 1;
    *y = e() % (this->graph->height - 2) + 1;
    for (auto [a, b]: this->ske->snake) {
        if (*x == a && *y == b) {
            return get_food(x, y);
        }
    }
    return game_status::con;
}


/*
 * 创建地图信息
 */
int snake::create_graph(void) {
    this->graph->graph = vector<vector<int>>(this->graph->width, vector<int>(this->graph->height));
    snake_location snake = this->ske->snake.front();
    for (int i = 0; i < this->graph->width; i++) {
        for (int j = 0; j < this->graph->height; j++) {
            if (i == 0 || j == 0 || i == this->graph->width - 1 || j == this->graph->height - 1) {
                this->graph->graph[i][j] = info::wall;
            } else if (snake.x == i && snake.y == j) {
                this->graph->graph[i][j] = info::snake;
            } else if (this->food->x == i && this->food->y == j) {
                this->graph->graph[i][j] = info::food;
            } else {
                this->graph->graph[i][j] = info::place;
            }
        }
    }
    return true;
}
