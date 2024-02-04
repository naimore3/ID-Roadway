import os
import sys
import optparse
import random
import traci
from sumolib import checkBinary


class Operation: # 操作
    def __init__(self):
        self.speed = [0, 0, 0, 0] # 变速
        self.change = [0, 0, 0, 0] # 变道

class Car: # 车辆的初始数据
    def __init__(self):
        self.type = "" # 车辆类型
        self.speed = 0 # 车辆初始速度
        self.road = 0 # 车辆初始的车道
        self.operation = Operation() # 车辆在接下来四个节点想要进行的操作
        self.command = Operation() # 系统给车辆的命令

# 检测是否已经添加环境变量
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")




# sumo自带的，检查gui版本的sumo是否存在
def get_options():
    optParser = optparse.OptionParser()
    optParser.add_option("--nogui", action="store_true",
                         default=False, help="run the commandline version of sumo")
    options, args = optParser.parse_args()
    return options



# 定义*.rou 文件
def generate_routefile(type,road):
    
    # 打开文件，“w” 代表将进行写入操作
    with open("C:\\Users\\30423\\Desktop\\Co-Simulation\\Co-Simulation\\V1.rou.xml", "w") as routes:
        print("""<routes>
    <vType id="sedan" accel="3.0" decel="6.0"  length="5"   color="1,0,1"/>
    <vType id="motorcycle" accel="5.0" decel="6.0"  length="2"   color="1,1,1"/>
    <vType id="van" accel="2.0" decel="4.0"  length="9"   color="1,1,0"/>
    <vType id="coach" accel="3.0" decel="4.0"  length="8"   color="1,0,0"/>

    <route id="route0"  edges="E0 E1 E2 E3 E4"/> 
    
    <vehicle id="0" type="%s"  depart="0" route="route0" departLane="%d"/>
    

</routes>"""%(type,road-1), file=routes)
        


# 主函数
def main():
    vehicle_types = ["sedan", "motorcycle", "van", "coach"]  #车辆类型

    # 创建包含一个 Car 对象的列表
    car = [Car()]
    
    # 随机生成车辆速度（速度范围：30-90）
    random_speed = random.randint(30, 90)
    car[0].speed = random_speed
    random_road = random.randint(1, 5)
    car[0].road = random_road

    # 随机生成车辆类型（共四种："sedan","motorcycle", "van","coach"，轿车、摩托车、货车、客车)
    random_type = random.randint(0, 3)
    car[0].type = vehicle_types[random_type]
    print(f"该车车型：{car[0].type}")

    

    # 随机生成车辆想要进行的操作（车道为1-5）
    for i in range(4):
        speed = random.randint(30, 90)
        change = random.randint(-1, 1)
        car[0].operation.speed[i] = speed
        car[0].operation.change[i] = change
        print(f"该车辆第{i+1}次操作速度变为{car[0].operation.speed[i]}，车道增加{car[0].operation.change[i]}")
    
    # 每到达一次测量点系统分配道路与速度（但目前只是单一车辆，故直接将车辆想要的操作作为系统分配）
    print(f"该车辆的初始速度为{car[0].speed}，初始车道为{car[0].road}")
    
    # 车辆经分配的速度、车道列表初始化
    speed=[0,0,0,0,0]
    lane=[0,0,0,0,0]

    # 先按照车型来分配，轿车行驶速度快，分配到第四、五车道；客车在第三车道；货车在属于重型车，在第二车道；摩托车在第一车道
    if random_type == 0:
        if car[0].speed >= 30 and car[0].speed < 60:
            car[0].road = 4
            print(f"经过测速点0后车辆速度为{car[0].speed}，分配的车道为4")
        else:
            car[0].road = 5
            print(f"经过测速点0后车辆速度为{car[0].speed}，分配的车道为5")
    elif random_type == 1:
        car[0].road = 1
        print(f"经过测速点0后车辆速度为{car[0].speed}，分配的车道为1")
    elif random_type == 2:
        car[0].road = 2
        print(f"经过测速点0后车辆速度为{car[0].speed}，分配的车道为2")
    else:
        car[0].road = 3
        print(f"经过测速点0后车辆速度为{car[0].speed}，分配的车道为3")
    speed[0]=car[0].speed/3.6 #转化为米每秒
    lane[0]=car[0].road
    

    # 然后车辆数据开始变化，开始分配
    for i in range(4):
        if car[0].operation.change[i] == 0:
            car[0].speed = car[0].operation.speed[i]
            if car[0].speed >= 30 and car[0].speed < 60:
                if car[0].road == 3 or car[0].road == 4 or car[0].road == 5:
                    car[0].command.speed[i] = car[0].operation.speed[i]
                    car[0].command.change[i] = -1
                    car[0].road = 2
        else:
            if car[0].operation.change[i] == 1:
                if car[0].road == 5:
                    car[0].operation.change[i] = 0
                else:
                    if car[0].road > 2:
                        car[0].command.speed[i] = -1
                        car[0].command.change[i] = car[0].operation.change[i]
                        car[0].road += car[0].operation.change[i]
                    elif car[0].road == 2:
                        car[0].command.speed[i] = 90
                        car[0].command.change[i] = car[0].operation.change[i]
                        car[0].speed = 90
                        car[0].road += car[0].operation.change[i]
                    else:
                        car[0].command.speed[i] = -1
                        car[0].command.change[i] = car[0].operation.change[i]
                        car[0].road += 1
            else:
                if car[0].road == 1:
                    car[0].operation.change[i] = 0
                else:
                    if car[0].road > 3:
                        car[0].command.speed[i] = -1
                        car[0].command.change[i] = car[0].operation.change[i]
                        car[0].road -= 1
                    elif car[0].road == 3:
                        car[0].command.speed[i] = 60
                        car[0].command.change[i] = car[0].operation.change[i]
                        car[0].speed = 60
                        car[0].road -= 1
                    else:
                        car[0].command.speed[i] = -1
                        car[0].command.change[i] = car[0].operation.change[i]
                        car[0].road -= 1
        speed[i+1]=car[0].speed/3.6
        lane[i+1]=car[0].road
        print(f"经过测速点{i+1}后车辆速度为{car[0].speed}，分配的车道为{car[0].road}")
    
    # 写入.rou文件
    generate_routefile(car[0].type,lane[0])

    i=0
    while traci.simulation.getMinExpectedNumber() > 0: # 当车道中还有车时   

        traci.simulationStep()  # 推进仿真进行一步
        
        traci.vehicle.changeLane("0",lane[i]-1,0) # 变道
        traci.vehicle.setMaxSpeed("0",speed[i]) # 变速

        # 获取车辆位置
        intersection_id = "J" + str(i+2)
        intersection_position = traci.junction.getPosition(intersection_id)
        
        # 获取路口（测速点）位置
        vehicle_id = "0"
        vehicle_position = traci.vehicle.getPosition(vehicle_id)

        threshold_distance = 15  # 设置一个合适的阈值，单位是米

        # 计算车辆与路口距离是否小于阈值
        is_vehicle_at_intersection = (intersection_position[0] - vehicle_position[0]) < threshold_distance 
        
        # 获取车辆信息
        sp=traci.vehicle.getSpeed("0")
        la=traci.vehicle.getLaneID("0")
        print("车此时所在车道为%s，速度为%.2f"%(la,sp))
        
        if is_vehicle_at_intersection==True: # 若车辆与路口距离小于阈值
            i+=1
        
    traci.close()  # 仿真结束 关闭TraCI
    sys.stdout.flush()  # 清除缓冲区'''    
    


if __name__ == "__main__":
    # 根据下载的sumo决定打开gui版本的sumo还是cmd版本的sumo
    options = get_options()
    if options.nogui:
        sumoBinary = checkBinary('sumo')
    else:
        sumoBinary = checkBinary('sumo-gui')

    # 根据指定路径打开sumocfg文件
    sumocfgfile = "C:\\Users\\30423\\Desktop\\Co-Simulation\\Co-Simulation\\V1.sumocfg"  # sumocfg文件的位置
    traci.start([sumoBinary, "-c", sumocfgfile])  # 打开sumocfg文件

    # 运行主函数
    main()