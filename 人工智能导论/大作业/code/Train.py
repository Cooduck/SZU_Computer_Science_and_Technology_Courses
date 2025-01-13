"""
Train.py
这段代码实现了训练一个在俄罗斯方块游戏环境的DQN智能体。
通过经验回放池存储和采样历史经验，智能体使用Q网络预测并优化每个状态下的动作价值，采用规则和epsilon-greedy策略平衡探索与利用。
训练过程中，智能体不断与环境交互并更新Q网络，周期性地同步目标网络，最终优化智能体的决策能力。
代码还包括模型保存和训练过程的日志记录功能，便于跟踪训练进度和结果。
"""
import collections
from tqdm import tqdm
import torch
import torch.nn.functional as F
from TetrisAI import *
from Tetris_config import *
import logging
import os
from datetime import datetime


class ReplayBuffer:
    """ 经验回放池 """
    def __init__(self, capacity):
        self.buffer = collections.deque(maxlen=capacity)  # 队列,先进先出

    def add(self, state, action, reward, next_state, done):  # 将数据加入buffer
        self.buffer.append((state, action, reward, next_state, done))

    def sample(self, batch_size):  # 从buffer中采样数据,数量为batch_size
        transitions = random.sample(self.buffer, batch_size)
        state, action, reward, next_state, done = zip(*transitions)
        return np.array(state), np.array(action), reward, np.array(next_state), done

    def size(self):  # 目前buffer中数据的数量n
        return len(self.buffer)


class Qnet(torch.nn.Module):
    """ Q网络 """
    def __init__(self):
        super(Qnet, self).__init__()
        self.fc1 = torch.nn.Linear(220, 1024)
        self.fc2 = torch.nn.Linear(1024, 2048)
        self.fc3 = torch.nn.Linear(2048, 1024)
        self.fc4 = torch.nn.Linear(1024, 1)

    def forward(self, x):
        x = F.relu(self.fc1(x))  # 隐藏层使用ReLU激活函数
        x = F.relu(self.fc2(x))
        x = F.relu(self.fc3(x))
        return self.fc4(x)


class DQN:
    """ DQN算法（深度Q网络） """
    def __init__(self, learning_rate, gamma, epsilon_start,
                 epsilon_end, num_decay_episodes, target_update, device):
        # 初始化DQN算法中的各个参数
        self.q_net = Qnet().to(device)  # 创建 Q 网络
        self.q_net.load_state_dict(torch.load("./Best Model/best_model.pth", weights_only=True))  # 可选：加载预训练模型
        self.target_q_net = Qnet().to(device)  # 创建目标网络
        self.target_q_net.load_state_dict(torch.load("./Best Model/best_model.pth", weights_only=True))  # 可选：加载预训练模型
        self.optimizer = torch.optim.Adam(self.q_net.parameters(), lr=learning_rate)    # 使用 Adam 优化器来更新 Q 网络的参数
        self.gamma = gamma  # 折扣因子，用于计算未来奖励的影响
        self.epsilon = epsilon_start  # epsilon-greedy 策略中的 epsilon 值，控制探索与利用的平衡
        self.epsilon_start = epsilon_start  # 初始的 epsilon 值
        self.epsilon_end = epsilon_end  # 最终的 epsilon 值
        self.num_decay_episodes = num_decay_episodes  # 用于 epsilon 衰减的总回合数
        self.target_update = target_update  # 目标网络更新的频率
        self.count = 0  # 用于跟踪更新次数
        self.device = device  # 设备（CPU 或 GPU）

    def prioritize_line_clear(self, next_states):
        """ 优先选择清除行最多的动作 """
        max_clear_lines = 0
        best_action_index = -1  # 初始化选择的动作索引
        # 遍历所有的候选状态，选择清除行数最多的动作
        for index, state in enumerate(next_states):
            clear_lines = state[-4]  # 当前清除的行数
            if clear_lines > max_clear_lines:
                max_clear_lines = clear_lines
                best_action_index = index  # 更新最大清除行数的动作索引
        # 如果清除行数为零，返回 -1 表示没有优先选择的动作
        if max_clear_lines == 0:
            return -1
        else:
            return best_action_index  # 返回选择的最优动作的下标

    def take_action(self, next_states):
        """ 根据规则和epsilon-greedy策略选择动作 """
        # 首先尝试根据清除行数的优先规则选择动作
        next_idx = self.prioritize_line_clear(next_states)
        if next_idx != -1:
            return next_idx  # 如果找到优先选择的动作，直接返回该动作的索引

        # 如果 epsilon 小于随机值，执行随机动作（探索）
        if np.random.random() < self.epsilon:
            return np.random.randint(0, len(next_states))  # 随机选择一个动作

        else:
            # 否则，执行贪婪策略，选择具有最大 Q 值的动作（利用）
            next_states_tensor = torch.tensor(next_states, dtype=torch.float).to(self.device)
            # 计算所有 next_states 对应的 Q 值
            with torch.no_grad():
                q_values = self.q_net(next_states_tensor)  # 获取 Q 值
            # 找出最大 Q 值对应的动作索引
            _, max_q_index = torch.max(q_values, dim=0)
            return max_q_index  # 返回最大 Q 值对应的动作索引

    def update(self, transition_dict, episodes):
        """ 更新 Q 网络的参数 """
        # 将经验数据转换为张量
        states = torch.tensor(transition_dict['states'], dtype=torch.float).to(self.device)
        rewards = torch.tensor(transition_dict['rewards'], dtype=torch.float).view(-1, 1).to(self.device)
        next_states = torch.tensor(transition_dict['next_states'], dtype=torch.float).to(self.device)
        dones = torch.tensor(transition_dict['dones'], dtype=torch.float).view(-1, 1).to(self.device)

        # 获取当前状态下的 Q 值
        q_values = self.q_net(states)
        # 获取目标网络在下个状态下的 Q 值
        max_next_q_values = self.target_q_net(next_states)
        # 计算 TD 误差目标（即目标 Q 值）
        q_targets = rewards + self.gamma * max_next_q_values * (1 - dones)
        # 计算损失（均方误差）
        dqn_loss = torch.mean(F.mse_loss(q_values, q_targets))

        # 清零优化器的梯度
        self.optimizer.zero_grad()
        # 反向传播并更新网络参数
        dqn_loss.backward()
        self.optimizer.step()

        # 每一定次数的更新后，更新目标网络的参数
        if self.count % self.target_update == 0:
            self.target_q_net.load_state_dict(self.q_net.state_dict())  # 将 Q 网络的参数复制给目标网络
        self.count += 1

        # 衰减 epsilon 值（使得探索逐渐变少，利用变多）
        self.epsilon = self.epsilon_end + (max(self.num_decay_episodes - episodes, 0) *
                                           (self.epsilon_start - self.epsilon_end) / self.num_decay_episodes)


def load_experience_from_file(file_path, replay_buffer):
    """ 导入人工产生的经验数据 """
    with open(file_path, 'r') as f:
        experience_data = json.load(f)

    # 将加载的经验数据加入经验池
    for experience in experience_data:
        replay_buffer.add(experience['state'], experience['action'], experience['reward'],
                          experience['next_state'], experience['done'])

    print(f"成功加载 {len(experience_data)} 条经验")

# 定义超参数
lr = 2e-3
num_episodes = 10000
num_decay_episodes = 4000
gamma = 0.98
epsilon_start = 1
epsilon_end = 0.01
target_update = 10
buffer_size = 50000
minimal_size = 500
batch_size = 128
device = torch.device("cuda") if torch.cuda.is_available() else torch.device("cpu")


def random_dic(dicts):
    """ 打乱字典顺序 """
    dict_key_ls = list(dicts.keys())
    random.shuffle(dict_key_ls)
    new_dic = {}
    for key in dict_key_ls:
        new_dic[key] = dicts.get(key)
    return new_dic


if __name__ == '__main__':
    # 初始话游戏环境、回放池、Q网络
    env = TetrisAI(SIZE, AREA_WIDTH, AREA_HEIGHT, BORDER_WIDTH, BORDER_COLOR, SCREEN_WIDTH, SCREEN_HEIGHT, BG_COLOR)
    replay_buffer = ReplayBuffer(buffer_size)
    load_experience_from_file("experience_data.json", replay_buffer)    # 读取经验载入到回放池中
    agent = DQN(lr, gamma, epsilon_start, epsilon_end, num_decay_episodes, target_update, device)

    # 配置日志
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    log_filename = f'./Log/training_log_{timestamp}.log'
    if os.path.exists(log_filename):
        os.remove(log_filename)
    logging.basicConfig(filename=log_filename, level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

    # 存储每轮游戏的奖励
    return_list = []
    for i in range(10):
        with (tqdm(total=int(num_episodes / 10), desc='Iteration %d' % i) as pbar):
            for i_episode in range(int(num_episodes / 10)):
                true_episode = int(i * num_episodes / 10) + i_episode + 1
                episode_return = 0  # 记录本轮游戏的总奖励
                state = env.reset(False)    # 重置游戏环境
                done = False    # 游戏结束标志
                holes, bumpiness, height = 0, 0, 0  # 当前状态的空洞数、凹凸度、最大高度
                while not done:
                    next_steps = env.get_next_states()  # 获取所有合法动作及其对应的状态
                    next_steps = random_dic(next_steps)     # 打乱返回的状态
                    next_actions, next_states = zip(*next_steps.items())    # 打包
                    next_idx = agent.take_action(next_states)   # 决策使用哪一个动作
                    next_state = next_states[next_idx]
                    next_holes, next_bumpiness, next_height = next_state[-3], next_state[-2], next_state[-1]    # 下一状态的空洞数、凹凸度、最大高度
                    action = next_actions[next_idx]     # 选择动作
                    reward, done = env.step(action)     # 执行动作
                    # reward = reward - (next_holes - holes) * 0.01 - (next_bumpiness - bumpiness) * 0.01     # 根据得分和惩罚计算奖励
                    replay_buffer.add(state, action, reward, next_state, done)  # 存储经验
                    state = next_state  # 更新状态
                    holes, bumpiness, height = next_holes, next_bumpiness, next_height  # 更新空洞数、凹凸度、最大高度
                    episode_return += reward    # 存储本回合奖励

                # 当buffer数据的数量超过一定值后,才进行Q网络训练
                if replay_buffer.size() > minimal_size:
                    b_s, b_a, b_r, b_ns, b_d = replay_buffer.sample(batch_size)     # 在回放池采样
                    transition_dict = {
                        'states': b_s,
                        'actions': b_a,
                        'next_states': b_ns,
                        'rewards': b_r,
                        'dones': b_d
                    }
                    agent.update(transition_dict, true_episode)     # 训练Q网络
                return_list.append(episode_return)

                # 输出训练过程的信息
                if (i_episode + 1) % 10 == 0:
                    pbar.set_postfix({
                        'episode':
                        '%d' % (num_episodes / 10 * i + i_episode + 1),
                        'return':
                        '%.3f' % np.mean(return_list[-10:])
                    })
                    logging.info(
                        f'Episode {true_episode}/{num_episodes}, Total Reward: {env.score}, '
                        f'Total Clear Lines:{env.tot_clear_lines}, Total Blocks:{env.tot_blocks}, '
                        f'Epsilon: {agent.epsilon:.2f}')
                pbar.update(1)

        # 每隔一定步数保存模型
        model_path = f'./Model/model_episode_{int((i + 1) * (num_episodes / 10))}.pth'
        torch.save(agent.target_q_net.state_dict(), model_path)
        logging.info(f'Model saved to {model_path}')

    # 输出每大轮的平均奖励
    for i in range(10):
        logging.info(f'Episode {i * int(num_episodes / 10) + 1} - {(i + 1) * int(num_episodes / 10)}, '
                     f'Average Reward:{np.mean(return_list[i * int(num_episodes / 10):(i + 1) * int(num_episodes / 10)])}')