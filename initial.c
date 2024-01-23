#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define number 1

typedef struct operation
{
    int speed[4];  // 变速
    int change[4]; // 变道
} Operation;       // 操作

typedef struct car
{
    char type[12];       // 车辆类型
    int speed;           // 车辆初始速度
    int road;            // 车辆初始的车道
    Operation operation; // 车辆在接下来四个节点想要进行的操作
    Operation command;   // 系统给车辆的命令
} *Car;                  // 车辆的初始数据
int main(void)           // 单个车辆分配
{
    const char *vehicle_types[] = {"sedan", "motorcycle", "van",
                                   "coach"}; // 车辆类型
    /*int number;//车辆的数量
    printf("请输入车辆数量：");
    scanf("%d", &number);*/

    Car car;
    car = (Car)malloc(sizeof(Car) * number);

    // 随机生成车辆速度（速度范围：30-90）
    srand(time(NULL));
    int random_speed = rand() % 61 + 30;
    car[0].speed = random_speed;
    int random_road = rand() % 5 + 1;
    car[0].road = random_road;

    // 随机生成车辆类型（共四种："sedan","motorcycle",
    // "van","coach"，轿车、摩托车、货车、客车)
    int random_type = rand() % 4;
    strncpy(car[0].type, vehicle_types[random_type], 10);
    car[0].type[11] = '\0';
    printf("该车车型：%s\n", car[0].type);

    srand(time(NULL));
    // 随机生成车辆想要进行的操作（车道为1-5）
    for (int i = 0; i < 4; i++)
    {
        int speed = rand() % 61 + 30;
        int change = rand() % 3 - 1; // 三种可能，-1、0、1，向上，不变，向左
        car[0].operation.speed[i] = speed;
        car[0].operation.change[i] = change;
        printf("该车辆第%d次操作速度变为%d，车道增加%d\n", i + 1,
               car[0].operation.speed[i], car[0].operation.change[i]);
    }

    // 每到达一次测量点系统分配道路与速度（但目前只是单一车辆，故直接将车辆想要的操作作为系统分配）
    printf("该车辆的初始速度为%d，初始车道为%d\n", car[0].speed, car[0].road);
    // 先按照车型来分配，轿车行驶速度快，分配到第四、五车道；客车在第三车道；货车在属于重型车，在第二车道；摩托车在第一车道
    if (random_type == 0) // 轿车
    {
        // 等区间划分速度
        if (car[0].speed >= 30 && car[0].speed < 60)
        {
            car[0].road = 4;
            printf("经过测速点0后车辆速度为%d，分配的车道为4\n", car[0].speed);
        }
        else
        {
            car[0].road = 5;
            printf("经过测速点0后车辆速度为%d，分配的车道为5\n", car[0].speed);
        }
    }
    else if (random_type == 1) // 摩托车
    {
        car[0].road = 1;
        printf("经过测速点0后车辆速度为%d，分配的车道为1\n", car[0].speed);
    }
    else if (random_type == 2) // 货车
    {
        car[0].road = 2;
        printf("经过测速点0后车辆速度为%d，分配的车道为2\n", car[0].speed);
    }
    else
    {
        car[0].road = 3;
        printf("经过测速点0后车辆速度为%d，分配的车道为3\n", car[0].speed);
    }
    // 然后车辆数据开始变化，开始分配
    for (int i = 0; i < 4; i++)
    {
        if (car[0].operation.change[i] == 0) // 驾驶员没有变道意愿
        {
            car[0].speed = car[0].operation.speed[i]; // 将车速设置为驾驶员想要的速度
            if (car[0].speed >= 30 && car[0].speed < 60)
            {
                if (car[0].road == 3 || car[0].road == 4 || car[0].road == 5)
                {
                    car[0].command.speed[i] = car[0].operation.speed[i]; // 不改变速度
                    car[0].command.change[i] = -1;                       // 改变车道
                    car[0].road = 2;                                     // 问题在于不知道哪个车道车辆更少
                }
            }
            else
            {
                if (car[0].road == 1 || car[0].road == 2)
                {
                    car[0].command.speed[i] = car[0].operation.speed[i]; // 不改变速度
                    car[0].command.change[i] = 1;                        // 改变车道
                    car[0].road = 3;                                     // 问题在于不知道哪个车道车辆更少
                }
            }
        }
        else // 驾驶员有变道意愿
        {
            if (car[0].operation.change[i] == 1) // 驾驶员想要向左变道
            {
                if (car[0].road == 5) // 五车道无法变道
                {
                    car[0].operation.change[i] = 0; // 无法变道
                }
                else
                {
                    // NOTE： 为什么要判断>2
                    if (car[0].road > 2) // 车在二车道即以上
                    {
                        car[0].command.speed[i] = -1;
                        car[0].command.change[i] = car[0].operation.change[i];
                        car[0].road += car[0].operation.change[i]; // 车道加一
                    }
                    else if (car[0].road == 2) // 车在二车道
                    {
                        car[0].command.speed[i] = 90;
                        car[0].command.change[i] = car[0].operation.change[i];
                        car[0].speed = 90;
                        car[0].road += car[0].operation.change[i];
                    }
                    else
                    {
                        car[0].command.speed[i] = -1;
                        car[0].command.change[i] = car[0].operation.change[i];
                        car[0].road++;
                    }
                }
            }
            else
            {
                if (car[0].road == 1) // 一车道无法变道
                {
                    car[0].command.change[i] = 0;
                }
                else
                {
                    if (car[0].road > 3) // 车在三车道即以上
                    {
                        car[0].command.speed[i] = -1;
                        car[0].command.change[i] = car[0].operation.change[i];
                        car[0].road--;
                    }
                    else if (car[0].road == 3)
                    {
                        car[0].command.speed[i] = 60;
                        car[0].command.change[i] = car[0].operation.change[i];
                        car[0].speed = 60;
                        car[0].road--;
                    }
                    else
                    {
                        car[0].command.speed[i] = -1;
                        car[0].command.change[i] = car[0].operation.change[i];
                        car[0].road--;
                    }
                }
            }
        }
        printf("经过测速点%d后车辆速度为%d，分配的车道为%d\n", i + 1, car[0].speed, car[0].road);

        /*car[0].command.speed[i] = car[0].operation.speed[i];
        car[0].command.change[i] = car[0].operation.change[i];
        printf("经过测速点%d后车辆速度为%d，车道为%d\n", i + 1,
        car[0].command.speed[i], car[0].command.change[i]);*/
    }
}