#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define car_number 1 //车辆的数量
#define road_length 10000 // 道路长度
#define laneNum 5 // 车道数量
#define assumeCarSpeed 20
const int segementLength = 200;
const int dot_number = road_length / segementLength; // 测速点的数量
#define dotNumber dot_number

typedef struct operation
{
    int speed[dotNumber];  // 变速
    int change[dotNumber]; // 变道
} Operation;               // 操作

typedef struct road // 车道
{
    int speed[car_number];                     // 车道上的车辆速度
    int carCounts[laneNum + 1][dotNumber + 1]; // 车道上的车辆数量count[第几个车道][第几段车道测速]
} * Road;                                      // 车道

typedef struct car
{
    int type;                    // 车辆类型
    int speed[dotNumber + 1];    // 车辆速度
    int lane[dotNumber + 1];     // 车辆车道
    int section[dotNumber + 1];  // 车辆所在的车段
    int distance[dotNumber + 1]; // 车辆所在的距离
    Operation operation;         // 车辆在接下来四个节点想要进行的操作
    Operation command;           // 系统给车辆的命令,存储要变化的车速，变道
} * Car;                         // 车辆的初始数据

//初始化车辆
void initializeCar(Car car, int car_Number)
{
    for (int i = 0; i < car_Number; i++)
    {
        int random_speed = rand() % 61 + 30; // 30 to 90
        int random_type = rand() % 4;        // 0 to 3
        car[i].speed[0] = random_speed;
        car[i].lane[0] = rand() % 5 + 1; // 1 to 5
        car[i].section[0] = 0;
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
    for (int i = 1; i <= dotNumber; i++)
    {
        car->operation.speed[i] = rand() % 61 + 30; // 速度范围：30-90
        car->operation.change[i] = rand() % 3 - 1;  // 变道：-1、0、1
        printf("该车辆第%d次操作速度变为%d，车道增加%d\n", i,
               car->operation.speed[i], car->operation.change[i]);
    }
}

void firstAllocate(Car car)
{
    switch (car->type)
    {
    case 0: // 轿车
        car->lane[0] = (car->speed[0] > 30 && car->speed[0] < 60) ? 4 : 5;
        break;
    case 1: // 摩托车
        car->lane[0] = 1;
        break;
    case 2: // 货车
        car->lane[0] = 2;
        break;
    case 3: // 客车
        car->lane[0] = 3;
        break;
    default:
        printf("未知车辆类型\n");
        break;
    }
    printf("经过测速点0后车辆速度为%d，分配的车道为%d\n", car->speed[0], car->lane[0]);
}

void allocateCommand(Car cars, Road road, int carNumber, int number)
{
    for (int i = 0; i < carNumber; i++)
    {
        if (cars[i].operation.change[i] == 0)
        {
            cars[i].command.speed[number] = cars[i].operation.speed[number];
            if (cars[i].operation.speed[number] >= 30 && cars[i].operation.speed[number] < 60)
            {
                if (cars[i].lane[number] >= 3)
                {
                    cars[i].command.change[number] = 2 - cars[i].lane[number - 1];
                    cars[i].lane[number] = 2;
                }
            }
            else if (cars[i].operation.speed[number] >= 60 && cars[i].operation.speed[number] <= 90)
            {
                if (cars[i].lane[number] == 1 || cars[i].lane[number] == 2)
                {
                    cars[0].command.change[number] = 3 - cars[i].lane[number - 1];
                    cars[0].lane[number] = 3;
                }
            }
        }
        else if (cars[i].operation.change[number] == 1) //驾驶员想要向左变道
        {
            if (cars[i].lane[number - 1] == 5) //五车道无法变道
            {
                cars[i].command.change[number] = 0; //不变道
                cars[i].command.speed[number] = cars[i].operation.speed[number];
            }
            else if (cars[i].lane[number - 1] > 2) //车在二车道即以上
            {
                cars[i].command.speed[number] = cars[i].operation.speed[number];
                cars[i].command.change[number] = cars[i].operation.change[number];
            }
            else if (cars[i].lane[number - 1] == 2)
            {
                cars[i].command.speed[number] = cars[i].operation.speed[number];
                cars[i].command.change[number] = cars[i].operation.change[number];
            }
            else
            {
                cars[i].command.speed[number] = cars[i].operation.speed[number];
                cars[i].command.change[number] = cars[i].operation.change[number];
            }
        }
        else if (cars[i].operation.change[number] == -1) //驾驶员想要向右变道
        {
            if (cars[i].lane[number - 1] == 1)
            {
                cars[i].command.change[number] = 0;
                cars[i].command.speed[number] = cars[i].operation.speed[number];
            }
            else if (cars[i].lane[number - 1] > 3)
            {
                cars[i].command.speed[number] = cars[i].operation.speed[number];
                cars[i].command.change[number] = cars[i].operation.change[number];
            }
            else if (cars[i].lane[number - 1] == 3)
            {
                cars[i].command.speed[number] = cars[i].operation.speed[number];
                cars[i].command.change[number] = cars[i].operation.change[number];
            }
            else
            {
                cars[i].command.speed[number] = cars[i].operation.speed[number];
                cars[i].command.change[number] = cars[i].operation.change[number];
            }
        }
        cars[i].lane[number] = cars[i].command.change[number] + cars[i].lane[number - 1];
        if (cars[i].lane[number] > 2 && cars[i].command.speed[number] < 60)
        {
            cars[i].command.speed[number] = 60;
        }
        else if (cars[i].lane[number] <= 2 && cars[i].command.speed[number] > 60)
        {
            cars[i].command.speed[number] = 60;
        }
        cars[i].speed[number] = cars[i].command.speed[number];
        printf("第%d辆车的原车速为%d，变化后车速为%d，操作为%d\n", i + 1, cars[i].speed[number - 1], cars[i].speed[number], cars[i].command.speed[number]);
        printf("第%d辆车在第%d个时间段的速度：%d  分配的车道为%d\n", i + 1, number, cars[i].speed[number], cars[i].lane[number]);
    }
}

void predictCarPosition(Car car, int carNumber, int dot_number, int segementLength, int number, int currentTime,int elapsedTime)
{
    for (int i = 0; i < carNumber; i++)
    {
        car[i].distance[number] = car[i].speed[number - 1] * elapsedTime + car[i].distance[number - 1];
        int section = car[i].distance[number] / segementLength + 1; //车辆所在的车段
        if (section > dot_number)
        {
            car[i].section[number] = dot_number;
        }
        else
        {
            car[i].section[number] = section;
        }
        printf("第%d辆车在第%d个时间段的位置：%d车道%d车段\n", i + 1, number, car[i].lane[number - 1], car[i].section[number]);
    }
}

void countCars(Car car, int carNumber, Road road,int dot_number)
{
    int currentTime = 0;
    initializeRoad(road);
    for(int i=1;i<=dotNumber;i++)
    {
        currentTime += 10;
        predictCarPosition(car, carNumber, dot_number, segementLength,i, currentTime,10);
        allocateCommand(car, road, carNumber, i);
        for(int j=0;j<carNumber;j++)
        {
            road->carCounts[car[j].lane[i]][car[j].section[i]]++;
        }
        for(int j=1;j<=laneNum;j++)
        {
            for(int k=1;k<=dot_number;k++)
            {
                printf("第%d车道第%d车段车辆数量为%d\n",j,k,road->carCounts[j][k]);
            }
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
    int timeStep = 0;
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
        printf("Car %d: type: %s, speed: %d, road: %d\n", i + 1, vehicle_types[car[i].type], car[i].speed[0], car[i].lane[0]);
    }
    //生成车辆操作
    for (int i = 0; i < car_number; i++)
    {
        printf("Car %d operations:\n", i + 1);
        generateOperation(&car[i], dotNumber);
    }
    //第一次分配车道，根据车辆类型和速度
    for (int i = 0; i < car_number; i++)
    {
        firstAllocate(&car[i]);
    }
    countCars(car, car_number, road, dot_number);
    free(road);
    free(car);
}