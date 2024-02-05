#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define car_number 5
#define dot_number 4
#define road_length 1000
#define laneNum 5
typedef struct operation
{
    int speed[4];  // 变速
    int change[4]; // 变道
} Operation;       // 操作

typedef struct road // 车道
{
    int speed[car_number];                      // 车道上的车辆速度
    int carCounts[laneNum + 1][dot_number + 1]; // 车道上的车辆数量count[第几个车道][第几段车道测速]
} * Road;                                       // 车道

typedef struct car
{
    int type;                  // 车辆类型
    int speed[dot_number + 1]; // 车辆速度
    int road[dot_number + 1];  // 车辆车道
    double time[dot_number];   // 车辆经历的时间
    Operation operation;       // 车辆在接下来四个节点想要进行的操作
    Operation command;         // 系统给车辆的命令,存储要变化的车速，变道
} * Car;                       // 车辆的初始数据

//初始化车辆
void initializeCar(Car car, int car_Number)
{
    for (int i = 0; i < car_Number; i++)
    {
        int random_speed = rand() % 61 + 30; // 30 to 90
        int random_type = rand() % 4;        // 0 to 3
        car[i].speed[0] = random_speed;
        car[i].road[0] = rand() % 5 + 1; // 1 to 5
        car[i].type = random_type;
    }
}

//初始化道路
void initializeRoad(Road road)
{
    memset(road, 0, sizeof(struct road));
}

void generateOperation(Car car, int operation_number)
{
    for (int i = 0; i < dot_number; i++)
    {
        car->operation.speed[i] = rand() % 61 + 30; // 速度范围：30-90
        car->operation.change[i] = rand() % 3 - 1;  // 变道：-1、0、1
        printf("该车辆第%d次操作速度变为%d，车道增加%d\n", i + 1,
               car->operation.speed[i], car->operation.change[i]);
    }
}

void firstAllocate(Car car)
{
    switch (car->type)
    {
    case 0: // 轿车
        car->road[0] = (car->speed[0] > 30 && car->speed[0] < 60) ? 4 : 5;
        break;
    case 1: // 摩托车
        car->road[0] = 1;
        break;
    case 2: // 货车
        car->road[0] = 2;
        break;
    case 3: // 客车
        car->road[0] = 3;
        break;
    default:
        printf("未知车辆类型\n");
        break;
    }
    printf("经过测速点0后车辆速度为%d，分配的车道为%d\n", car->speed[0], car->road[0]);
}

void updateCarCounts(Road road, Car cars, int carNumber)
{
    // 清零统计数据
    memset(road->carCounts, 0, sizeof(road->carCounts));

    // 遍历所有车辆，更新统计数据
    for (int i = 0; i < car_number; i++)
    {
        for (int j = 0; j <= dot_number; j++)
        {
            int lane = cars[i].road[j];
            road->carCounts[lane][j]++;
        }
    }
}

int main()
{
    srand(time(NULL));
    Road road = malloc(sizeof(struct road));
    initializeRoad(road); //初始化道路
    const char *vehicle_types[] = {"sedan", "motorcycle", "van",
                                   "coach"}; // 车辆类型

    Car car = malloc(sizeof(struct car) * car_number);
    if (!car || !road) //内存分配失败
    {
        printf("Memory allocation failed\n");
        free(road);
        free(car);
        return 1;
    }
    initializeCar(car, car_number); //初始化车辆

    //打印车辆信息
    for (int i = 0; i < car_number; i++)
    {
        printf("Car %d: type: %s, speed: %d, road: %d\n", i, vehicle_types[car[i].type], car[i].speed[0], car[i].road[0]);
    }
    //生成车辆操作
    for (int i = 0; i < car_number; i++)
    {
        printf("Car %d operations:\n", i + 1);
        generateOperation(&car[i], dot_number);
    }
    //第一次分配车道，根据车辆类型和速度
    for (int i = 0; i < car_number; i++)
    {
        firstAllocate(&car[i]);
    }
    updateCarCounts(road, car, car_number);
    
    free(road);
    free(car);
}