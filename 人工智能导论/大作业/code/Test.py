"""
Test.py
该代码实现了基于训练好的Q网络模型的俄罗斯方块AI测试。
完成多个测试回合后，计算并输出平均得分、清除的行数和生成的方块数量。
并且还可以可视化测试结果，展示每个回合的得分变化曲线。
"""
import numpy as np
import torch
from TetrisAI import TetrisAI
from Tetris_config import *
from Train import Qnet
import matplotlib.pyplot as plt
import random


def prioritize_line_clear(next_states):
    """ 根据清除行数优先选择动作 """
    max_clear_lines = 0
    best_action_index = -1  # 初始化下标
    # 遍历所有的候选动作和状态
    for index, state in enumerate(next_states):
        clear_lines = state[-4]  # 当前清除的行数
        if clear_lines > max_clear_lines:
            max_clear_lines = clear_lines
            best_action_index = index  # 更新最大清除行数的下标
    if max_clear_lines == 0:
        return -1
    else:
        return best_action_index


def take_action(q_net, device, next_states):
    """ 根据规则和贪婪策略选择动作 """
    next_idx = prioritize_line_clear(next_states)
    if next_idx != -1:
        return next_idx  # 选择最优的动作

    if np.random.random() < 0.01:
        # 随机选择动作
        return np.random.randint(0, len(next_states))
    else:
        # 贪婪策略，选择 Q 值最大的状态
        next_states_tensor = torch.tensor(next_states, dtype=torch.float).to(device)

        # 计算所有 next_states 中的 Q 值
        with torch.no_grad():
            q_values = q_net(next_states_tensor)  # 获取 Q 值

        # 找出最大 Q 值对应的下标
        _, max_q_index = torch.max(q_values, dim=0)
        return max_q_index


def random_dic(dicts):
    """ 随机打乱字典的顺序 """
    dict_key_ls = list(dicts.keys())
    random.shuffle(dict_key_ls)
    new_dic = {}
    for key in dict_key_ls:
        new_dic[key] = dicts.get(key)
    return new_dic


def ai_operation(model_path):
    """ 运行AI进行操作 """
    device = torch.device("cuda") if torch.cuda.is_available() else torch.device("cpu")
    q_net = Qnet().to(device)  # 创建Q网络
    q_net.load_state_dict(torch.load(model_path, weights_only=True))  # 加载训练好的模型权重
    env = TetrisAI(SIZE, AREA_WIDTH, AREA_HEIGHT, BORDER_WIDTH, BORDER_COLOR, SCREEN_WIDTH, SCREEN_HEIGHT,
                   BG_COLOR)  # 初始化游戏环境
    _ = env.reset(True)  # 重置游戏并显示
    done = False
    while not done:
        next_steps = env.get_next_states()  # 获取所有可能的下一个状态
        next_steps = random_dic(next_steps)  # 打乱状态顺序，增加随机性
        next_actions, next_states = zip(*next_steps.items())  # 获取动作和状态的配对
        next_idx = take_action(q_net, device, next_states)  # 选择动作
        action = next_actions[next_idx]  # 获取对应的动作
        reward, done = env.step(action)  # 执行动作并获取奖励
    return env.score, env.tot_clear_lines, env.tot_blocks  # 返回得分、清除行数和方块数


# 主函数，进行多个测试回合并计算平均成绩
if __name__ == '__main__':
    num_test_episodes = 1000  # 测试回合数
    score_list = []  # 存储每个回合的得分
    clear_lines_list = []  # 存储每个回合清除的行数
    blocks_list = []  # 存储每个回合生成的方块数
    model_path = "./Best Model/best_model.pth"  # 训练好的模型路径
    for episode in range(num_test_episodes):
        score, clear_lines, blocks = ai_operation(model_path)  # 进行一次测试
        score_list.append(score)  # 记录得分
        clear_lines_list.append(clear_lines)  # 记录清除的行数
        blocks_list.append(blocks)  # 记录生成的方块数
    # 输出测试结果
    print(f'Test completed. Over {num_test_episodes} episodes: average score {np.mean(score_list):.2f}, '
          f'average clear_lines {np.mean(clear_lines_list):.2f}, average blocks {np.mean(blocks_list):.2f}')

    # 可视化测试结果
    plt.plot(score_list)
    plt.xlabel('Episode')  # x轴标签
    plt.ylabel('Total Reward')  # y轴标签
    plt.title('Test Results')  # 图标题
    plt.show()  # 显示图形
