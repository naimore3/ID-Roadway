#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define car_number 1
#define dot_number 4
#define road_length 1000
typedef struct operation
{
    int speed[4];  // 变速
    int change[4]; // 变道
} Operation;       // 操作
typedef struct road // 车道
{
    int speed[car_number]; // 车道上的车辆速度
    int count[6][dot_number+1][dot_number+1]; // 车道上的车辆数量count[车道][第几段车道测速][第几次测速]
} *Road;                     // 车道
typedef struct car
{
    char type[12];       // 车辆类型
    int speed;           // 车辆初始速度
    int road;            // 车辆初始的车道
    double time[dot_number];         // 车辆经历的时间
    Operation operation; // 车辆在接下来四个节点想要进行的操作
    Operation command;   // 系统给车辆的命令
} *Car;                  // 车辆的初始数据


int main(void)           // 单个车辆分配
{
    //NOTE 车道
    Road road;
    road = (Road)malloc(sizeof(struct road));
    memset(road, 0, sizeof(struct road));
    const char *vehicle_types[] = {"sedan", "motorcycle", "van",
                                   "coach"}; // 车辆类型
    /*int car_number;//车辆的数量
    printf("请输入车辆数量：");
    scanf("%d", &car_number);*/

    Car car;
    car = (Car)malloc(sizeof(struct car) * car_number); // BUG 内存分配导致的问题，已经修复
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
        //NOTE 判断车辆数量
        
        /*
        对每一辆车都预测其到达下一个测速点的时间，从而判断其在下一个监测点时候，在哪一个&&哪一段车道上
        */
        car[0].time[i]=(road_length*1.0/(dot_number+1))/car[0].speed;

        if (car[0].operation.change[i] == 0) // 驾驶员没有变道意愿
        {
            car[0].speed = car[0].operation.speed[i]; // 将车速设置为驾驶员想要的速度
            if (car[0].speed >= 30 && car[0].speed < 60)
            {
                if (car[0].road>=3)
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
                    car[0].road = 3;                                     // TODO 从3-5三个车道选车少的
                }
            }
        }
        else if (car[0].operation.change[i] == 1) // 驾驶员想要向左变道
        {
            // NOTE 1-2车道车速max=60，3-5车道车速max=90
            if (car[0].road == 5) // 五车道无法变道
            {
                car[0].command.change[i] = 0;
                if(car[0].operation.speed[i] < 60)
                {
                    car[0].command.speed[i] = 90;
                }
                else
                {
                    car[0].command.speed[i] = car[0].operation.speed[i];
                }
                car[0].speed = car[0].command.speed[i];
            }
            else if (car[0].road > 2) // 车在二车道即以上
            {
                car[0].command.speed[i] = -1;
                car[0].command.change[i] = car[0].operation.change[i];
                car[0].road += car[0].operation.change[i];
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
        else if (car[0].operation.change[i] == -1) // 驾驶员想要向右变道
        {
            if (car[0].road == 1) // 一车道无法变道
            {
                car[0].command.change[i] = 0;
                if(car[0].operation.speed[i] > 60)
                {
                    car[0].command.speed[i] = 60;
                }
                else
                {
                    car[0].command.speed[i] = car[0].operation.speed[i];
                }
                car[0].speed = car[0].command.speed[i];
            }
            else if(car[0].road > 3) // 车在三车道即以上
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
        printf("经过测速点%d后车辆速度为%d，分配的车道为%d\n", i + 1, car[0].speed, car[0].road);
    }
    car[0].time[dot_number]=road_length*1.0/(dot_number+1)/car[0].speed;
    double t=0;
    for(int i=0;i<=dot_number;i++)
    {
        t+=car[0].time[i];
        printf("到达第%d个测速点的时间为%f\n",i+1,t);
    }
    for(int j=10;j<=40;j+=10)
    {
        double spend_time=0;
        for(int i=0;i<=dot_number;i++)
        {
            spend_time+=car[0].time[i];
            if(spend_time>j&&spend_time<j+10)
            {
                printf("第%ds检测时在第%d车段\n",j,i);
                road->count[car[0].road][i][j/10]++;
                break;
            }
        }
    }
    for(int i=1;i<=5;i++)
    {
        for(int j=1;j<=dot_number;j++)
        {
            for(int k=1;k<=4;k++)
            {
                printf("第%d车道第%d车段第%d0s检测的车辆数量为%d\n",i,j,k,road->count[i][j][k]);
            }
        }
    }



    
}