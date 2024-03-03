#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct operation
{
    int *speed;  // 变速
    int *change; // 变道
} Operation;     // 操作

typedef struct road // 车道
{
    int **carCounts; // 车道上的车辆数量count[第几个车道][第几段车道测速]
    int lanes;       // 车道数量
} * Road;            // 车道

typedef struct car
{
    int type;            // 车辆类型
    int *speed;          // 车辆速度
    int *lane;           // 车辆车道
    int predictSection;        // 车辆所在的车段
    double predictDistance;       // 车辆所在的距离
    Operation operation; // 车辆在接下来n个节点想要进行的操作
    Operation command;   // 系统给车辆的命令,存储要变化的车速，变道
} * Car;                 // 车辆的初始数据

//初始化车辆
void initializeCar(Car car, int car_Number, int laneNum, int dotNum)
{
    for (int i = 0; i < car_Number; i++)
    {
        car[i].speed = (int *)malloc(sizeof(int) * (dotNum + 1));
        car[i].lane = (int *)malloc(sizeof(int) * (dotNum + 1));
        car[i].operation.speed = (int *)malloc(sizeof(int) * (dotNum + 1));
        car[i].operation.change = (int *)malloc(sizeof(int) * (dotNum + 1));
        car[i].command.speed = (int *)malloc(sizeof(int) * (dotNum + 1));
        car[i].command.change = (int *)malloc(sizeof(int) * (dotNum + 1));

        int random_speed = rand() % 61 + 30; // 30 to 90
        int random_type = rand() % 4;        // 0 to 3
        car[i].speed[0] = random_speed;
        car[i].lane[0] = rand() % laneNum + 1; // 1 to 5
        car[i].predictSection = 0;
        car[i].predictDistance = 0;
        car[i].type = random_type;
    }
}

//初始化道路
void initializeRoad(Road road, int laneNum, int dotNum)
{
    road->lanes=laneNum;
    road->carCounts = (int **)malloc(sizeof(int*) * (laneNum + 1));
    if (road->carCounts == NULL)
    {
        // 处理内存分配失败的情况
        free(road); // 先释放已分配的 road
        return;
    }
    for (int i = 0; i <= laneNum; i++)
    {
        road->carCounts[i] = (int *)malloc(sizeof(int) * (dotNum + 1));
        if (road->carCounts[i] == NULL)
        {
            // 处理内存分配失败的情况
            for (int j = 0; j <= i; j++)
            {
                free(road->carCounts[j]);
            }
            free(road->carCounts); // 先释放已分配的 road
            return ;
        }
        for (int j = 0; j <= dotNum; j++)
        {
            road->carCounts[i][j] = 0;
        }
    }
}

void generateOperation(Car car, int operationNumber, int dotNumber)
{
    for (int i = 1; i <= dotNumber; i++)
    {
        car->operation.speed[i] = rand() % 61 + 30; // 速度范围：30-90
        car->operation.change[i] = rand() % 3 - 1;  // 变道：-1、0、1
        printf("该车辆第%d次操作速度变为%d，车道增加%d\n", i,
               car->operation.speed[i], car->operation.change[i]);
    }
}

void firstAllocate(Car cars,int carNum)
{
    for(int i=0;i<carNum;i++)
    {
        switch (cars[i].type)
        {
        case 0: // 轿车
            cars[i].lane[0] = (cars[i].speed[0] > 30 && cars[i].speed[0] < 60) ? 4 : 5;
            break;
        case 1: // 摩托车
            cars[i].lane[0] = 1;
            break;
        case 2: // 货车
            cars[i].lane[0] = 2;
            break;
        case 3: // 客车
            cars[i].lane[0] = 3;
            break;
        default:
            printf("未知车辆类型\n");
            break;
        }
        printf("经过测速点0后车辆速度为%d，分配的车道为%d\n", cars[i].speed[0], cars[i].lane[0]);
    }
}

void allocateCommand(Car cars, Road road, int carNumber, int number)
{
    for (int i = 0; i < carNumber; i++)
    {
        cars[i].command.change[number] = 0;
        cars[i].command.speed[number] = cars[i].operation.speed[number];
        cars[i].lane[number]=cars[i].lane[number-1];
        if (cars[i].operation.change[number] != 0)
        {
            int newLane = cars[i].lane[number - 1] + cars[i].operation.change[number];
            if (newLane >= 1 && newLane <= road->lanes)
            {
                cars[i].command.change[number] = cars[i].operation.change[number];
                cars[i].lane[number] = newLane;
            }
            else
            {
                cars[i].lane[number] = cars[i].lane[number - 1];
            }
        }
        if (cars[i].lane[number] > 2 && cars[i].command.speed[number] < 60)
        {
            if(cars[i].speed[number-1]>60){
                cars[i].command.speed[number] = cars[i].speed[number-1];
            }
            else{
                cars[i].command.speed[number] = 60;
            }
        }
        else if (cars[i].lane[number] <= 2 && cars[i].command.speed[number] > 60)
        {
            if(cars[i].speed[number-1]<60){
                cars[i].command.speed[number] = cars[i].speed[number-1];
            }
            else{
                cars[i].command.speed[number] = 60;
            }
        }
        cars[i].speed[number] = cars[i].command.speed[number];
        printf("第%d辆车的原车速为%d，变化后车速为%d，操作为%d\n", i + 1, cars[i].speed[number - 1], cars[i].speed[number], cars[i].command.change[number]);
        printf("第%d辆车在第%d个测速点的速度：%d  分配的车道为%d\n", i + 1, number, cars[i].speed[number], cars[i].lane[number]);
    }
}


void updateRoadCondition(Road road,Car cars,int carNum,int dotNum,int num)
{
    for(int lane=1;lane<=road->lanes;lane++)
    {
        for(int section=1;section<=dotNum;section++)
        {
            road->carCounts[lane][section]=0;
        }
    }

    //根据车辆的预测位置更新道路状况
    for(int i=0;i<carNum;i++)
    {
        int lane=cars[i].lane[num];
        int section=cars[i].predictSection+1;
        if(lane>0&&lane<=road->lanes&&section>=0&&section<dotNum)
        {
            road->carCounts[lane][section]++;
        }
    }
}
void freeMemory(Car cars, Road road, int carNumber)
{
    if (cars != NULL) {
        for (int i = 0; i < carNumber; i++) {
            // 释放 Car 结构体中的动态分配内存
            free(cars[i].speed); cars[i].speed = NULL;
            free(cars[i].lane); cars[i].lane = NULL;

            // 检查并释放 Operation 结构体中的动态分配内存
            if (cars[i].operation.speed != NULL) {
                free(cars[i].operation.speed);
                cars[i].operation.speed = NULL;
            }
            if (cars[i].operation.change != NULL) {
                free(cars[i].operation.change);
                cars[i].operation.change = NULL;
            }
            if (cars[i].command.speed != NULL) {
                free(cars[i].command.speed);
                cars[i].command.speed = NULL;
            }
            if (cars[i].command.change != NULL) {
                free(cars[i].command.change);
                cars[i].command.change = NULL;
            }
        }
        // 释放 Car 数组本身
        free(cars);
    }
    if (road != NULL) {
        // 释放 Road 结构体中的动态分配内存
        if (road->carCounts != NULL) {
            for (int i = 0; i < road->lanes; i++) {
                free(road->carCounts[i]);
            }
            free(road->carCounts);
            road->carCounts = NULL;
        }
        free(road);
        road = NULL;
    }
}

void printRoadCondition(Road road, int dotNum)
{
    for (int i = 1; i <= road->lanes; i++)
    {
        for (int j = 1; j <= dotNum; j++)
        {
            printf("%d ", road->carCounts[i][j]);
        }
        printf("\n");
    }
}


int main()
{
    srand(time(NULL));
    int car_number = 1;
    int lane_number = 5;
    int road_length = 1000;
    int test_length = 200;
    int dot_number = road_length / test_length;

    Road road = malloc(sizeof(struct road));
    initializeRoad(road, lane_number, dot_number); //初始化道路
    const char *vehicle_types[] = {"sedan", "motorcycle", "van",
                                   "coach"}; // 车辆类型
    Car car = malloc(sizeof(struct car) * car_number);
    int timeStep = 0;
    if (!car || !road) //内存分配失败
    {
        printf("Memory allocation failed\n");
        free(road);
        free(car);
        return 1;
    }
    initializeCar(car, car_number, lane_number, dot_number); //初始化车辆

    //打印车辆信息
    for (int i = 0; i < car_number; i++)
    {
        printf("Car %d: type: %s, speed: %d, road: %d\n", i + 1, vehicle_types[car[i].type], car[i].speed[0], car[i].lane[0]);
    }
    //生成车辆操作
    for (int i = 0; i < car_number; i++)
    {
        printf("Car %d operations:\n", i + 1);
        generateOperation(&car[i], dot_number, dot_number);
    }
    //第一次分配车道，根据车辆类型和速度
    firstAllocate(car, car_number);
    updateRoadCondition(road,car,car_number,dot_number,0);

    freeMemory(car, road, car_number);
}