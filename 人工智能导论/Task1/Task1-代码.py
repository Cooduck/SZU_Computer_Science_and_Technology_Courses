# coding=utf-8
# 简校滨-高性能班-2022150176
import random
from queue import Queue, PriorityQueue
import time


# dir为0、1、2、3时，分别表示上、右、下、左的移动方向
def move_zero(board, dir):
    zero_pos = board.index('0')  # 找到空白格子（'0'）的位置
    swap_pos = zero_pos  # 初始化交换位置为当前空白格子的位置

    # 判断移动方向并更新交换位置
    if dir == 0 and zero_pos > 2:  # 上移，确保不越界
        swap_pos = zero_pos - 3
    elif dir == 1 and zero_pos % 3 != 2:  # 右移，确保不越界
        swap_pos = zero_pos + 1
    elif dir == 2 and zero_pos < 6:  # 下移，确保不越界
        swap_pos = zero_pos + 3
    elif dir == 3 and zero_pos % 3 != 0:  # 左移，确保不越界
        swap_pos = zero_pos - 1

    # 将棋盘字符串转换为列表以便交换
    str_list = list(board)
    str_list[zero_pos], str_list[swap_pos] = str_list[swap_pos], str_list[zero_pos]  # 交换空白格子与目标位置的元素
    return ''.join(str_list)  # 将列表转换回字符串并返回


# 获取元素0移动的方向
def get_dir(dir):
    if dir == 0:
        return "up"
    elif dir == 1:
        return "right"
    elif dir == 2:
        return "down"
    elif dir == 3:
        return "left"


# 深度优先搜索算法实现
def dfs(init_board, target_board):
    if init_board == target_board:
        return None  # 如果初始棋盘已经等于目标棋盘，则不需要搜索，直接返回None

    open_table = []     # 初始化一个栈open_table用于存放待探索的棋盘状态和路径
    closed_table = {}   # 初始化一个字典closed_table用于记录已经探索过的棋盘状态，避免重复探索
    open_table.append((init_board, list()))     # 将初始棋盘状态和初始路径（空路径）加入open_table栈

    while len(open_table):
        now_board, now_path = open_table[-1][0], open_table[-1][1]
        open_table.pop()    # 弹出栈顶元素
        closed_table[now_board] = 1     # 将当前棋盘状态标记为已探索

        for i in range(4):
            tmp_path = now_path.copy()
            now_dir = get_dir(i)
            next_board = move_zero(now_board, i)
            # 如果新棋盘状态等于目标棋盘状态，找到了解决方案，返回路径
            if next_board == target_board:
                tmp_path.append(now_dir)
                return tmp_path
            # 如果新棋盘状态未被探索过，则将其加入open_table栈继续探索
            if next_board not in closed_table:
                tmp_path.append(now_dir)
                open_table.append((next_board, tmp_path))

# 宽度优先搜索算法实现
def bfs(init_board, target_board):
    if init_board == target_board:
        return None  # 如果初始棋盘已经等于目标棋盘，则不需要搜索，直接返回None

    open_table = Queue()    # 初始化一个队列open_table用于存放待探索的棋盘状态
    closed_table = {}   # 初始化一个字典closed_table用于记录已经探索过的棋盘状态，避免重复探索
    open_table.put((init_board, list()), block=False)   # 将初始棋盘状态和初始路径（空路径）加入open_table队列

    while open_table.qsize():
        (now_board, now_path) = open_table.get(block=False)  # 从open_table队列中取出当前棋盘状态和路径
        closed_table[now_board] = 1 # 将当前棋盘状态标记为已探索，并存储在closed_table中

        # 遍历所有可能的移动方向
        for i in range(4):
            tmp_path = now_path.copy()
            now_dir = get_dir(i)
            next_board = move_zero(now_board, i)    # 根据当前方向移动空白格，得到新的棋盘状态
            # 如果新棋盘状态等于目标棋盘状态，找到了解决方案，返回路径
            if next_board == target_board:
                tmp_path.append(now_dir)
                return tmp_path
            # 如果新棋盘状态未被探索过，则将其加入open_table队列继续探索
            if next_board not in closed_table:
                tmp_path.append(now_dir)
                open_table.put((next_board, tmp_path), block=False)


# 一致代价搜索算法实现
def ucs(init_board, target_board):
    if init_board == target_board:
        return None  # 如果初始棋盘已经等于目标棋盘，则不需要搜索，直接返回None

    open_table = PriorityQueue()    # 初始化一个优先队列open_table用于存放待探索的棋盘状态和路径
    closed_table = {}    # 初始化一个字典closed_table用于记录已经探索过的棋盘状态，避免重复探索
    open_table.put((0, (init_board, list())), block=False)

    while open_table.qsize():
        (_, (now_board, now_path)) = open_table.get(block=False)    # 优先级最低的（即最先加入的）先出队
        closed_table[now_board] = 1 # 将当前棋盘状态标记为已探索，并存储在closed_table中

        for i in range(4):
            tmp_path = now_path.copy()
            now_dir = get_dir(i)
            next_board = move_zero(now_board, i)    # 根据当前方向移动空白格，得到新的棋盘状态
            # 如果新棋盘状态等于目标棋盘状态，找到了解决方案，返回路径
            if next_board == target_board:
                tmp_path.append(now_dir)
                return tmp_path
            # 如果新棋盘状态未被探索过，则将其加入open_table队列继续探索
            # 优先级设置为当前路径长度，保证路径长度越短的先被探索
            if next_board not in closed_table:
                tmp_path.append(now_dir)
                open_table.put((len(tmp_path), (next_board, tmp_path)), block=False)


# 计算曼哈顿距离
def manhattan_distance(board, target_board):
    distance = 0
    for i in range(len(board)):
        if board[i] != '0':
            # 将一维转换为二维，计算出曼哈顿距离
            target_pos = target_board.index(board[i])
            distance += abs(i // 3 - target_pos // 3) + abs(i % 3 - target_pos % 3)
    return distance


# A*搜索算法实现
def astar(init_board, target_board):
    if init_board == target_board:
        return None  # 如果初始棋盘已经等于目标棋盘，则不需要搜索，直接返回None

    open_table = PriorityQueue()     # 初始化一个优先队列open_table用于存放待探索的棋盘状态和路径
    closed_table = {}   # 初始化一个字典closed_table用于记录已经探索过的棋盘状态，避免重复探索
    open_table.put((0, (init_board, list())))   # 将初始棋盘状态和初始路径（空路径）加入open_table队列，初始代价为0

    while open_table.qsize():
        (cost, (now_board, now_path)) = open_table.get()    # 代价最低的（即最先加入的）先出队
        closed_table[now_board] = 1     # 将当前棋盘状态标记为已探索，并存储在closed_table中
        for i in range(4):
            tmp_path = now_path.copy()
            now_dir = get_dir(i)
            next_board = move_zero(now_board, i)    # 根据当前方向移动空白格，得到新的棋盘状态
            # 如果新棋盘状态等于目标棋盘状态，找到了解决方案，返回路径
            if next_board == target_board:
                tmp_path.append(now_dir)
                return tmp_path
            # 如果新棋盘状态未被探索过，则将其加入open_table队列继续探索
            if next_board not in closed_table:
                # 计算 f(n) = g(n) + h(n)，其中g(n)是实际成本，h(n)是启发式估计成本（曼哈顿距离）
                g = len(tmp_path) + 1  # 实际成本，即当前路径的长度加1
                h = manhattan_distance(next_board, target_board)  # 曼哈顿距离作为启发式函数
                tmp_path.append(now_dir)
                open_table.put((g + h, (next_board, tmp_path))) # 将新棋盘状态和路径加入open_table队列，代价为g(n)+h(n)


# 判断八数码问题是否可解
def is_solvable(init_board, target_board):
    # 计算给定列表中的逆序对数量
    def count_inversions(flat_list):
        inversions = 0
        # 遍历列表中的每个元素
        for i in range(len(flat_list)):
            # 比较该元素与后面所有元素
            for j in range(i + 1, len(flat_list)):
                # 只计算非零元素的逆序对
                if flat_list[i] != '0' and flat_list[j] != '0' and flat_list[i] > flat_list[j]:
                    inversions += 1  # 增加逆序对计数
        return inversions  # 返回逆序对数量

    # 将初始棋盘和目标棋盘转换为列表
    initBoard = list(init_board)
    inversions1 = count_inversions(initBoard)  # 计算初始棋盘的逆序对数量

    targetBoard = list(target_board)
    inversions2 = count_inversions(targetBoard)  # 计算目标棋盘的逆序对数量

    # 判断逆序对数量的奇偶性
    if (inversions1 % 2) == (inversions2 % 2):
        return True  # 如果奇偶性相同，则该问题可解
    else:
        return False  # 如果奇偶性不同，则该问题不可解


# 验证算法正确性
def verify(f, name):
    init_board = ["312405678", "120345678", "042135678"]
    target_board = "012345678"
    expected_path = [["left", "up"], ["left", "left"], ["down", "right", "up", "left"]]

    for i in range(len(init_board)):
        path = f(init_board[i], target_board)
        if path != expected_path[i]:
            print(f"{name}算法未通过正确性验证")
            return
    print(f"{name}算法通过了正确性验证")


# 测试每个算法平均运行时间
def test_algorithm_runtime():
    # 定义需要测试的算法
    algorithms = {
        "BFS": bfs,
        "UCS": ucs,
        "A*": astar
    }
    iterations = 50  # 测试的轮数
    # 用于存储每个算法的平均运行时间
    average_times = {
        "BFS": 0,
        "UCS": 0,
        "A*": 0
    }
    # 进行指定轮数的测试
    for _ in range(iterations):
        my_list = list(range(9))  # 创建初始列表
        while True:
            random.shuffle(my_list)  # 随机打乱列表
            init_board = ''.join(map(str, my_list))  # 生成初始棋盘字符串
            random.shuffle(my_list)  # 再次打乱列表以生成目标棋盘
            target_board = ''.join(map(str, my_list))  # 生成目标棋盘字符串

            # 检查当前初始棋盘和目标棋盘是否可解
            if is_solvable(init_board, target_board):
                break  # 如果可解，则退出循环

        # 对每个算法进行性能测试
        for name, algorithm in algorithms.items():
            start_time = time.time()  # 记录开始时间
            algorithm(init_board, target_board)  # 调用算法
            end_time = time.time()  # 记录结束时间

            # 累加该算法的运行时间
            average_times[name] += (end_time - start_time) / iterations

    # 输出每个算法的平均运行时间
    print("算法平均运行时间（秒）：")
    for algorithm, avg_time in average_times.items():
        print(f"{algorithm}算法: {avg_time:.6f}")


# 求解八数码问题的函数
def solve(algorithm, init_board, target_board):
    algorithms = {
        "DFS": dfs,
        "BFS": bfs,
        "UCS": ucs,
        "A*": astar
    }
    print(f"使用{algorithm}算法求解八数码问题")
    print("初始棋盘: " + init_board)
    print("目标棋盘: " + target_board)

    # 检查初始棋盘和目标棋盘是否可解
    if is_solvable(init_board, target_board):
        path = algorithms[algorithm](init_board, target_board)
        first = 1   # 初始化一个标志变量，用于控制打印格式
        print("初始棋盘转变成目标棋盘的路径: ")
        if path:
            idx = 0  # 初始化一个索引变量，用于控制换行
            for dir in path:
                if first == 0:
                    # 如果不是第一个方向，则打印箭头和方向，并且索引加1
                    print(" ->", dir, end='')
                    idx += 1
                    if idx % 10 == 0:
                        print("\n", end='')  # 每10个方向换一行
                else:
                    print(dir, end='')  # 如果是第一个方向，则直接打印方向
                    first = 0
    else:
        # 如果问题无解，则打印无解信息
        print("问题无解，即初始棋盘无法转变成目标棋盘")
    print("\n")



if __name__ == '__main__':

    # verify(dfs, "DFS")  # 验证DFS算法
    # verify(bfs, "BFS")  # 验证BFS算法
    # verify(ucs, "UCS")  # 验证UCS算法
    # verify(astar, "A*")  # 验证A*算法

    # for i in range(3):
    #     # 用字符串来表示棋盘
    #     my_list = list(range(9))
    #     random.shuffle(my_list)  # 打乱棋盘
    #     init_board = ''.join(map(str, my_list))  # 初始布局
    #     random.shuffle(my_list)  # 打乱棋盘
    #     target_board = ''.join(map(str, my_list))  # 目标布局
    #     solve("BFS", init_board, target_board)  # 用BFS算法求解问题
    #     solve("UCS", init_board, target_board)  # 用UCS算法求解问题
    #     solve("A*", init_board, target_board)  # 用A*算法求解问题

    test_algorithm_runtime()


