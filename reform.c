#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

typedef struct Car
{
    int id;
    int type;
    int currentSpeed;
    int currentLane;
    double position;
    double nextCheckPoint;
} Car;

typedef struct CarNode
{
    Car car;
    struct CarNode *next;
} CarNode;

typedef struct Road
{
    int lanes;     //车道数量
    double length; //车段长度
    double checkpointInterval;
    CarNode *carListHead; //车辆列表
    int **carCountsPerSegment;
    int checkpointCount;
} Road;

void initialRoad(Road *road, double length, int lanes, double checkpointInterval)
{
    road->length = length;
    road->lanes = lanes;
    road->checkpointInterval = checkpointInterval;
    road->carListHead = NULL;
    road->checkpointCount = (int)(length / checkpointInterval);
    road->carCountsPerSegment = (int **)malloc(road->lanes * sizeof(int *));
    for (int i = 0; i < road->lanes; i++)
    {
        road->carCountsPerSegment[i] = (int *)malloc(road->checkpointCount * sizeof(int));
        memset(road->carCountsPerSegment[i], 0, road->checkpointCount * sizeof(int));
    }
}

void initializeCar(Car *car, int id, int type, int currentSpeed, int currentLane, double position)
{
    car->id = id;
    car->type = type;
    car->currentSpeed = currentSpeed;
    car->currentLane = currentLane;
    car->position = position;
    car->nextCheckPoint = 0;
}

void addCar(Road *road, Car newCar)
{
    CarNode *newNode = (CarNode *)malloc(sizeof(CarNode));
    if (newNode == NULL)
    {
        printf("Memory allocation failed");
        exit(1);
    }
    newNode->car = newCar;
    newNode->next = road->carListHead;
    road->carListHead = newNode;
}

void resetCarCountsPerSegment(Road *road)
{
    for (int i = 0; i < road->lanes; i++)
    {
        for (int j = 0; j < road->checkpointCount; j++)
        {
            road->carCountsPerSegment[i][j] = 0;
        }
    }
}

void updateCar(Road *road, double dt)
{
    CarNode *current = road->carListHead;

    while (current != NULL)
    {
        double newPosition = current->car.position + current->car.currentSpeed * dt;
        int currentCheckpointIndex = (int)(current->car.position / road->checkpointInterval);
        int newCheckpointIndex = (int)(newPosition / road->checkpointInterval);
        if (newCheckpointIndex > currentCheckpointIndex)
        {
            printf("Car %d passed checkpoint %d\n", current->car.id, newCheckpointIndex);
        }
        if (newCheckpointIndex >= road->checkpointCount)
        {
            printf("Car %d reached the end of the road\n", current->car.id);
            current->car.position = road->length;
            current = current->next;
            continue;
        }
        road->carCountsPerSegment[current->car.currentLane - 1][newCheckpointIndex]++;
        current->car.position = newPosition;
        current = current->next;
    }
}

void initializeRoad(Road *road, double length, double checkpointInterval, int lanes)
{
    road->length = length;
    road->checkpointInterval = checkpointInterval;
    road->lanes = lanes;
}

void printCarCountsPerSegment(const Road *road)
{
    printf("车辆统计数据：\n");
    for (int i = 0; i < road->checkpointCount - 1; i++)
    { // 遍历每个车段
        printf("车段 %d (从 %.2f m 到 %.2f m):\n", i, i * road->checkpointInterval, (i + 1) * road->checkpointInterval);
        for (int j = 0; j < road->lanes; j++)
        { // 遍历每个车道
            printf("\t车道 %d: %d 辆车\n", j + 1, road->carCountsPerSegment[j][i]);
        }
    }
}

void freeCarList(CarNode *head)
{
    CarNode *current = head;
    while (current != NULL)
    {
        CarNode *next = current->next;
        free(current); // 释放当前节点
        current = next;
    }
}

void freeRoad(Road *road)
{
    // 首先，释放车辆链表
    freeCarList(road->carListHead);

    // 接着，释放每个车段每个车道的车辆数量统计数组
    if (road->carCountsPerSegment != NULL)
    {
        for (int i = 0; i < road->lanes; i++)
        {
            if (road->carCountsPerSegment[i] != NULL)
            {
                free(road->carCountsPerSegment[i]);
            }
        }
        free(road->carCountsPerSegment); // 释放包含行指针的数组
    }

    // 如果Road中还有其他需要动态分配的成员，这里也应该释放它们

    // 最后，可以选择性地重置Road结构体的指针，以避免野指针
    road->carListHead = NULL;
    road->carCountsPerSegment = NULL;
}

int main()
{
    srand(time(NULL));
    Road road;
    initialRoad(&road, 1000, 3, 100);

    int carCount = 5;
    for (int i = 0; i < carCount; i++)
    {
        Car newCar = {
            i,
            rand() % 3,
            rand() % 30,
            rand() % 3 + 1,
            0.0,
            0.0};
        addCar(&road, newCar);
    }

    double dt = 1.0;
    double simulationTime = 20;
    for (double currentTime = 0; currentTime < simulationTime; currentTime += dt)
    {
        resetCarCountsPerSegment(&road);
        updateCar(&road, dt);
        printCarCountsPerSegment(&road);
    }
    freeRoad(&road);
    return 0;
}
