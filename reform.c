#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

/**
 * @brief Structure representing a car.
 *
 * This structure holds information about a car, including its ID, type, current speed, current lane,
 * position, next checkpoint, target speed, and target lane.
 */
typedef struct Car
{
    int id;                // 车辆ID
    int type;              // 车辆类型
    int currentSpeed;      // 车辆当前速度
    int currentLane;       // 车辆当前车道
    double position;       // 车辆位置
    double nextCheckPoint; // 车辆下一个检查点
    int targetSpeed;       // 车辆目标速度
    int targetLane;        // 车辆目标车道
} Car;

//改用链表去存储，方便增删和遍历
typedef struct CarNode
{
    Car car;              //车辆
    struct CarNode *next; //下一个车辆
} CarNode;

typedef struct Road
{
    int lanes;     //车道数量
    double length; //车段长度
    double checkpointInterval;
    CarNode *carListHead; //车辆列表
    int *carCountPerLane;
    int checkpointCount;
    int **carCountsPerSegment;
    double **avgSpeedPerLaneSegment; // 每个车段每个车道的平均速度
} Road;

void initializeRoad(Road *road, double length, int lanes, double checkpointInterval)
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
    road->avgSpeedPerLaneSegment = (double **)malloc(road->lanes * sizeof(double *));
    for (int j = 0; j < road->lanes; j++)
    {
        road->avgSpeedPerLaneSegment[j] = (double *)calloc(road->checkpointCount, sizeof(double));
        memset(road->avgSpeedPerLaneSegment[j], 0, road->checkpointCount * sizeof(double));
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

void updateSegmentLaneInfo(Road *road)
{
    // 重置统计信息
    for (int i = 0; i < road->lanes; i++)
    {
        memset(road->carCountsPerSegment[i], 0, road->checkpointCount * sizeof(int));
        memset(road->avgSpeedPerLaneSegment[i], 0, road->checkpointCount * sizeof(double));
    }

    // 遍历所有车辆更新统计信息
    CarNode *current = road->carListHead;
    while (current)
    {
        int segmentIndex = (int)(current->car.position / road->checkpointInterval);
        if(segmentIndex >= road->checkpointCount){
            segmentIndex = road->checkpointCount - 1;
        }
        int laneIndex = current->car.currentLane;

        // 累加速度和车辆数
        road->carCountsPerSegment[laneIndex][segmentIndex]++;
        road->avgSpeedPerLaneSegment[laneIndex][segmentIndex] += current->car.currentSpeed;

        current = current->next;
    }

    // 计算平均速度
    for (int i = 0; i < road->lanes; i++)
    {
        for (int j = 0; j < road->checkpointCount; j++)
        {
            if (road->carCountsPerSegment[i][j] > 0)
            {
                road->avgSpeedPerLaneSegment[i][j] /= road->carCountsPerSegment[i][j];
            }
        }
    }
}

void adjustCarSpeed(Car *car, const Road *road)
{
    // 随机因子用于模拟驾驶者的随机加速或减速决策
    int randValue = rand() % 100;

    // 假设有一定几率根据当前情况加速或减速
    if (randValue < 40)
    {
        // 基于某些条件加速，比如当前速度低于速度限制
        if (car->currentSpeed < 100)
        {                                                // 假设90是某个理想速度
            car->targetSpeed = car->currentSpeed * 1.05; // 加速5%
        }
    }
    else if (randValue < 80)
    {
        // 基于某些条件减速，比如接近前车
        car->targetSpeed = car->currentSpeed * 0.95; // 减速5%
    }
    else
    {
        car->targetSpeed = car->currentSpeed; // 保持当前速度
    }
    // 确保速度不超过最大速度限制，也不低于最小速度
    if (car->currentSpeed > 120)
    {
        car->targetSpeed = 120; // 假设120是最大速度限制
    }
    else if (car->currentSpeed < 60)
    {
        car->targetSpeed = 60; // 假设20是最小速度
    }
}

void attemptLaneChange(Car *car, const Road *road)
{
    // 随机决定是否尝试变道
    int randValue = rand() % 100;
    // 假定有30%的机会考虑变道
    if (randValue < 30)
    {
        // 假定驾驶者倾向于向左变道（如果可能），以超车
        if (car->currentLane > 1)
        { // 确保不是在最左侧车道
            printf("Car %d decides to change from lane %d to lane %d\n", car->id, car->currentLane + 1, car->currentLane - 1 + 1);
            car->targetLane = car->currentLane - 1; // 向左变道
        }
        else if (car->currentLane < road->lanes)
        { // 如果在最左侧车道，且有右侧车道，则考虑向右变道
            printf("Car %d decides to change from lane %d to lane %d\n", car->id, car->currentLane + 1, car->currentLane + 1 + 1);
            car->targetLane = car->currentLane + 1; // 向右变道
        }
    }
    else
    {
        car->targetLane = car->currentLane;
    }
}

void assignOperations(Road *road, Car *car)
{
    adjustCarSpeed(car, road);    // 驾驶员想调整车速
    attemptLaneChange(car, road); // 驾驶员尝试变道
    int segmentIndex = (int)(car->position / road->checkpointInterval);
    updateSegmentLaneInfo(road); // 更新车道信息
    //考虑变道情况
    if (car->targetLane != car->currentLane)
    {
        if (road->carCountsPerSegment[car->targetLane][segmentIndex] < road->carCountsPerSegment[car->currentLane][segmentIndex]) //目标车道车辆数小于当前车道
        {
            car->currentLane = car->targetLane;
        }
    }
    car->currentSpeed = car->targetSpeed;
    printf("Car %d: current speed: %d, current lane: %d, target speed: %d, target lane: %d\n", car->id, car->currentSpeed, car->currentLane + 1, car->targetSpeed, car->targetLane + 1);
}

void printCarCountsPerSegment(const Road *road)
{
    printf("车辆统计数据：\n");
    for (int i = 0; i < road->lanes; i++)
    {
        printf("车道%d：", i + 1);
        for (int j = 0; j < road->checkpointCount; j++)
        {
            printf("%d ", road->carCountsPerSegment[i][j]);
        }
        printf("\n");
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
            assignOperations(road, &current->car);
        }
        if (newCheckpointIndex >= road->checkpointCount)
        {
            printf("Car %d reached the end of the road\n", current->car.id);
            current->car.position = road->length;
            current = current->next;
            continue;
        }
        current->car.position = newPosition;
        current = current->next;
    }
}

void freeRoad(Road *road)
{
    CarNode *current = road->carListHead;
    while (current != NULL)
    {
        CarNode *temp = current;
        current = current->next;
        free(temp);
    }
    road->carListHead = NULL;

    if (road->carCountsPerSegment != NULL)
    {
        for (int i = 0; i < road->lanes; i++)
        {
            if (road->carCountsPerSegment[i] != NULL)
            {
                free(road->carCountsPerSegment[i]);
            }
        }
        free(road->carCountsPerSegment);
        road->carCountsPerSegment = NULL; // 避免悬挂指针
    }

    // 释放每个车段每个车道的平均速度统计数组
    if (road->avgSpeedPerLaneSegment != NULL)
    {
        for (int j = 0; j < road->lanes; j++)
        {
            if (road->avgSpeedPerLaneSegment[j] != NULL)
            {
                free(road->avgSpeedPerLaneSegment[j]);
            }
        }
        free(road->avgSpeedPerLaneSegment);
        road->avgSpeedPerLaneSegment = NULL;
    }
}

int main()
{
    srand(time(NULL));
    Road road;
    initializeRoad(&road, 1500, 5, 100);

    int carCount = 10;
    for (int i = 0; i < carCount; i++)
    {
        Car newCar = {
            i,
            rand() % 3,
            rand() % 120 + 60,
            rand() % 5,
            0.0,
            0.0};
        addCar(&road, newCar);
    }
    double dt = 1.0;
    double simulationTime = 20;
    for (double currentTime = 0; currentTime < simulationTime; currentTime += dt)
    {
        updateCar(&road, dt);
    }
    printCarCountsPerSegment(&road);
    freeRoad(&road);
    return 0;
}
