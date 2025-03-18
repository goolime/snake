#include "snake_map.h"
#include <pthread.h>
#include <iostream>
#include <vector>
#include <utility>
#include <stdlib.h>
#include <time.h>
#include "macros.h"
#include <list>


using namespace std;

SnakeMap::SnakeMap(Snake *snake)
{
    this->snake = snake;
    clear_map(this->map_array);
    srand(time(NULL));
    update_snake_food(true);
}

void SnakeMap::redraw(void)
{
    clear_map(this->map_array);
    for (int i = 0; i < MAP_END; i++)
    {
        cout << endl;
    }
    update_score();
    vector<pair<int, int>> snake_parts = snake->snake_parts;
    for (int i = 0; i < snake_parts.size(); i++)
    {
        pair<int, int> tmp = snake_parts[i];
        map_array[tmp.first][tmp.second] = SNAKE_CHAR;
    }
    update_snake_head(map_array, snake);
    update_snake_food(false);
    map_array[snake_food.first][snake_food.second] = SNAKE_FOOD_CHAR;
    if (snake->helpmode){
        calcdist();
        paveway(snake->snake_head,snake->get_direction());
    }
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            switch (map_array[i][j]){
                case SNAKE_CHAR:
                case SNAKE_HEAD_WEST:
                case SNAKE_HEAD_NORTH:
                case SNAKE_HEAD_EAST:
                case SNAKE_HEAD_SOUTH:
                    cout << "\033[31m" << map_array[i][j] << "\033[0m ";
                    break;
                case SNAKE_FOOD_CHAR:
                    cout << "\033[34m" << map_array[i][j] << "\033[0m ";
                    break;
                case MAP_ROAD_CHAR_VER:
                case MAP_ROAD_CHAR_HOR:
                    cout << "\033[32m" << map_array[i][j] << "\033[0m ";
                    break;
                default:
                   cout << map_array[i][j] << " ";
            }
            
        }
        cout << endl;
    }
}

void SnakeMap::update_snake_food(bool force_update)
{
    if (snake->food_eaten || force_update)
    {
        while (true)
        {
            int random_i = rand() % MAP_WIDTH;
            int random_j = rand() % MAP_HEIGHT;
            if (map_array[random_i][random_j] == MAP_CHAR)
            {
                snake_food = make_pair(random_i, random_j);
                snake->set_snake_food(snake_food);
                snake->food_eaten = false;
                break;
            }
        }
    }
}

void SnakeMap::paveway(pair<int,int> location, Direction dir){
    if(location==this->snake_food) return;
    Direction nextdir;
    pair<int,int> nextlocation;
    int min = waymap[location.first][location.second];
    if (map_array[location.first][location.second]==MAP_CHAR)
        switch (dir)
        {
        case West:
        case East:
            map_array[location.first][location.second] = MAP_ROAD_CHAR_VER;
            break;
        case North:
        case South:
            map_array[location.first][location.second] = MAP_ROAD_CHAR_HOR;
        default:
            break;
        }
        
    for (int i=0; i<4; i++){
        if (i!=dir && (i+dir)%2==0) continue;
        switch ((Direction)i){
            case West:
                if (location.second+1<MAP_WIDTH && waymap[location.first][location.second+1]<min){
                    nextdir=West;
                    nextlocation=make_pair(location.first,location.second+1);
                    min=waymap[location.first][location.second+1];
                }
                break;
            case East:
                if (location.second-1>=0 && waymap[location.first][location.second-1]<min){
                    nextdir=East;
                    nextlocation=make_pair(location.first,location.second-1);
                    min=waymap[location.first][location.second-1];
                }
                break;
            case North:
                if (location.first+1<MAP_HEIGHT && waymap[location.first+1][location.second]<min){
                    nextdir=North;
                    nextlocation=make_pair(location.first+1,location.second);
                    min=waymap[location.first+1][location.second];
                }
                break;
            case South:
                if (location.first-1<MAP_HEIGHT && waymap[location.first-1][location.second]<min){
                    nextdir=South;
                    nextlocation=make_pair(location.first-1,location.second);
                    min=waymap[location.first-1][location.second];
                }
                break;
        }
    }
    paveway(nextlocation,nextdir);
}

void clear_map(char map_array[MAP_HEIGHT][MAP_WIDTH])
{
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            map_array[i][j] = MAP_CHAR;
        }
    }
}

void update_snake_head(char map_array[MAP_HEIGHT][MAP_WIDTH], Snake *snake)
{
    char snake_head_char = SNAKE_CHAR;
    enum Direction direction = snake->get_direction();
    switch (direction)
    {
    case West:
        snake_head_char = SNAKE_HEAD_WEST;
        break;
    case North:
        snake_head_char = SNAKE_HEAD_NORTH;
        break;
    case East:
        snake_head_char = SNAKE_HEAD_EAST;
        break;
    case South:
        snake_head_char = SNAKE_HEAD_SOUTH;
        break;
    }
    pair<int, int> snake_head = snake->snake_head;
    map_array[snake_head.first][snake_head.second] = snake_head_char;
}

void SnakeMap::update_score(void)
{
    cout << "Score:" << snake->length << endl;
}

void SnakeMap::calcdist(){
    list<pair <int,int>> line;
    for (int i=0; i<MAP_HEIGHT; i++){
        for (int j=0; j<MAP_WIDTH; j++){
            waymap[i][j]=MAP_HEIGHT*MAP_WIDTH;
        }
    }
    waymap[snake_food.first][snake_food.second]=0;
    line.push_back(snake_food);
    while(!line.empty()){
        pair<int,int> curr=line.front();
        line.pop_front();
        int val = waymap[curr.first][curr.second]+1;

        //NORTH
        if (curr.first+1<MAP_HEIGHT && map_array[curr.first+1][curr.second]==MAP_CHAR && waymap[curr.first+1][curr.second]>val){
            waymap[curr.first+1][curr.second]=val;
            line.push_back(make_pair(curr.first+1,curr.second));
        }

        //SOUTH
        if (curr.first-1>=0 && map_array[curr.first-1][curr.second]==MAP_CHAR && waymap[curr.first-1][curr.second]>val){
            waymap[curr.first-1][curr.second]=val;
            line.push_back(make_pair(curr.first-1,curr.second));
        }

        //WEST
        if (curr.second+1<MAP_WIDTH && map_array[curr.first][curr.second+1]==MAP_CHAR && waymap[curr.first][curr.second+1]>val){
            waymap[curr.first][curr.second+1]=val;
            line.push_back(make_pair(curr.first,curr.second+1));
        }

        //EAST
        if (curr.second-1>=0 && map_array[curr.first][curr.second-1]==MAP_CHAR && waymap[curr.first][curr.second-1]>val){
            waymap[curr.first][curr.second-1]=val;
            line.push_back(make_pair(curr.first,curr.second-1));
        }
    }
}