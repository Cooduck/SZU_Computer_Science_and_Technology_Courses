import numpy as np


class SolverBase:
    """ 多臂老虎机解决方案的基本框架 """
    def __init__(self, bandit):
        self.bandit = bandit    # 老虎机
        self.count = 0  # 总行动次数
        self.kCount = np.zeros(self.bandit.K)  # 每根拉杆的尝试次数
        self.reward = 0  # 当前行动的累计奖励
        self.cumulative_rewards = []  # 用于记录每一步的累计奖励
        self.best_action_ratio = []  # 用于记录每一步最佳行动的比例
        self.regret = 0.  # 当前步的累积懊悔
        self.cumulative_regrets = []  # 用于记录每一步的累积懊悔

    def take_one_step(self):
        """ 返回当前动作选择哪一根拉杆和获得的奖励 """
        raise NotImplementedError

    def run(self, num_action):
        """ 运行一定次数,num_steps为总运行次数 """
        for _ in range(num_action):
            # 行动一次
            idx, reward = self.take_one_step()
            self.count += 1
            self.kCount[idx] += 1
            self.reward += reward
            self.regret += self.bandit.best_prob - self.bandit.probs[idx]
            self.cumulative_rewards.append(self.reward) # 记录累计奖励
            self.best_action_ratio.append(self.kCount[self.bandit.best_idx] * 1.0 / self.count) # 记录最佳行动占比
            self.cumulative_regrets.append(self.regret) # 记录累计懊悔


class RandomSelection(SolverBase):
    """ 随机选择算法,继承Solver类 """
    def __init__(self, bandit):
        super(RandomSelection, self).__init__(bandit)

    def take_one_step(self):
        randomIdx = np.random.randint(0, self.bandit.K) # 随机选择一根杆
        reward = self.bandit.step(randomIdx)    # 获得的奖励
        return randomIdx, reward


class EpsilonGreedy(SolverBase):
    """ epsilon贪婪算法,继承Solver类 """
    def __init__(self, bandit, epsilon=0.01, init_prob=0.5):
        super(EpsilonGreedy, self).__init__(bandit)
        self.epsilon = epsilon
        #初始化拉动所有拉杆的期望奖励估值
        self.estimates = np.array([init_prob] * self.bandit.K)

    def take_one_step(self):
        if np.random.random() < self.epsilon:
            k = np.random.randint(0, self.bandit.K)  # 随机选择一根拉杆
        else:
            k = np.argmax(self.estimates)  # 选择期望奖励估值最大的拉杆
        r = self.bandit.step(k)  # 得到本次动作的奖励
        self.estimates[k] += (r - self.estimates[k]) * 1.0 / (self.kCount[k] + 1)    # 更新期望奖励估值
        return k, r


class UCB(SolverBase):
    """ UCB算法,继承Solver类 """
    def __init__(self, bandit, coef = 0.5, init_prob=0.5):
        super(UCB, self).__init__(bandit)
        self.estimates = np.array([init_prob] * self.bandit.K)
        self.coef = coef

    def take_one_step(self):
        ucb = self.estimates + self.coef * np.sqrt(
            np.log(self.count + 1) / (2 * (self.kCount + 1)))  # 计算上置信界
        k = np.argmax(ucb)  # 选出上置信界最大的拉杆
        r = self.bandit.step(k)
        self.estimates[k] += (r - self.estimates[k]) * 1.0 / (self.kCount[k] + 1)
        return k, r


class ThompsonSampling(SolverBase):
    """ 汤普森采样算法（使用Beta分布）,继承Solver类 """
    def __init__(self, bandit):
        super(ThompsonSampling, self).__init__(bandit)
        # 假设拉动每根拉杆的奖励服从Beta分布，初始为均匀分布
        self.alpha = np.ones(self.bandit.K)  # 每根拉杆奖励为1的次数
        self.beta  = np.ones(self.bandit.K)  # 每根拉杆奖励为0的次数

    def take_one_step(self):
        samples = np.random.beta(self.alpha, self.beta)  # 按照Beta分布采样一组奖励样本
        k = np.argmax(samples)  # 选出采样奖励最大的拉杆
        r = self.bandit.step(k)

        self.alpha[k] += r  # 更新Beta分布的第一个参数
        self.beta[k] += (1 - r)  # 更新Beta分布的第二个参数
        return k, r


class GradientBandit(SolverBase):
    """ 梯度赌博机算法, 继承SolverBase类 """
    def __init__(self, bandit, alpha=0.1):
        super(GradientBandit, self).__init__(bandit)
        self.alpha = alpha  # 学习率
        self.h = np.zeros(self.bandit.K)  # 偏好值初始化为零
        self.avg_reward = 0  # 平均奖励初始化为0

    def take_one_step(self):
        # 使用Softmax选择一个拉杆
        exp_h = np.exp(self.h - max(self.h))  # 为了避免溢出，减去最大值
        prob = exp_h / np.sum(exp_h)  # 计算每根拉杆的选择概率
        k = np.random.choice(self.bandit.K, p=prob)  # 根据概率选择拉杆
        r = self.bandit.step(k)  # 得到该拉杆的奖励

        # 更新平均奖励
        self.avg_reward += (r - self.avg_reward) / (self.count + 1)

        # 更新偏好值（梯度上升）
        for i in range(self.bandit.K):
            if i == k and r == 1:
                self.h[i] += self.alpha * (r - self.avg_reward) * (1 - prob[i])  # 对选择的拉杆，增加其偏好值
            else:
                self.h[i] -= self.alpha * (r - self.avg_reward) * prob[i]  # 对其他未选择的拉杆，减少其偏好值
        return k, r
