import os
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from Bandit import BernoulliBandit
from solution import *


def plot_results(cumulative_rewards, cumulative_regret, best_action_ratio, algorithm_name):
    """ 绘制累计奖励和最佳行动占比的图 """

    plt.figure(figsize=(12, 6))

    # 设置中文字体支持
    plt.rcParams['axes.unicode_minus'] = False  # 解决负号显示为方块的问题

    # 设置大标题
    plt.suptitle(f'{algorithm_name}', fontsize=16)

    # 累计奖励
    plt.subplot(1, 3, 1)
    plt.plot(cumulative_rewards)
    plt.title('RewardS - Steps')
    plt.xlabel('Steps')
    plt.ylabel('Rewards')

    # 累计懊悔
    plt.subplot(1, 3, 2)
    plt.plot(cumulative_regret)
    plt.title('Regrets - Steps')
    plt.xlabel('Steps')
    plt.ylabel('Regrets')

    # 最佳行动占比
    plt.subplot(1, 3, 3)
    plt.plot(best_action_ratio)
    plt.title('Best Action Ratios - Steps')
    plt.xlabel('Steps')
    plt.ylabel('Best Action Ratios')

    plt.tight_layout()

    # 调整布局，确保大标题不与子图重叠
    plt.subplots_adjust(top=0.85)

    # 保存图表到指定目录
    save_path = os.path.join("./result", f'{algorithm_name}_result.jpg')
    plt.savefig(save_path)



def epsilonGreedy_different_epsilon_plot_results():
    """ 绘制不同epsilon下epsilon贪婪算法的最佳行动占比随行动次数的变化 """

    np.random.seed(1)  # 设定随机种子，确保实验可重复
    K = 10  # 拉杆数
    bandit = BernoulliBandit(K)

    # 定义多个 epsilon 值
    epsilon_values = [0.01, 0.05, 0.1, 0.2, 0.3]

    best_action_ratios = []

    # 对每个 epsilon 值进行实验并收集结果
    for epsilon in epsilon_values:
        epsilonGreedy_solver = EpsilonGreedy(bandit, epsilon, 0.5)
        best_action_ratio = epsilonGreedy_solver.run(1000)
        best_action_ratios.append(epsilonGreedy_solver.best_action_ratio)

    # 绘制结果
    save_dir = "./result"  # 你可以修改为你希望保存的目录

    plt.figure(figsize=(12, 6))

    # 绘制不同 epsilon 的结果
    for epsilon, best_action_ratio in zip(epsilon_values, best_action_ratios):
        plt.plot(best_action_ratio, label=f'epsilon={epsilon}')

    plt.title('Different epsilon of EpsilonGreedy: Best Action Ratios - Steps')
    plt.xlabel('Steps')
    plt.ylabel('Action Ratios')
    plt.legend()
    plt.tight_layout()

    # 保存图表到指定目录
    os.makedirs(save_dir, exist_ok=True)
    plt.savefig(f'./result/epsilonGreedy_different_epsilon_result.jpg')


def plot_cumulative_rewards(cumulative_rewards_dict, algorithm_names, save_path="./result/cumulative_rewards.jpg"):
    """ 绘制累计奖励对比图并保存为独立的图片 """
    plt.figure(figsize=(12, 6))
    for algo_name in algorithm_names:
        plt.plot(cumulative_rewards_dict[algo_name], label=algo_name, linewidth=2)
    plt.title('Cumulative Rewards Comparison')
    plt.xlabel('Steps')
    plt.ylabel('Cumulative Rewards')
    plt.legend()
    plt.tight_layout()
    os.makedirs(os.path.dirname(save_path), exist_ok=True)
    plt.savefig(save_path)

def plot_cumulative_regret(cumulative_regrets_dict, algorithm_names, save_path="./result/cumulative_regrets.jpg"):
    """ 绘制累计懊悔对比图并保存为独立的图片 """
    plt.figure(figsize=(12, 6))
    for algo_name in algorithm_names:
        plt.plot(cumulative_regrets_dict[algo_name], label=algo_name, linewidth=2)
    plt.title('Cumulative Regret Comparison')
    plt.xlabel('Steps')
    plt.ylabel('Cumulative Regrets')
    plt.legend()
    plt.tight_layout()
    os.makedirs(os.path.dirname(save_path), exist_ok=True)
    plt.savefig(save_path)

def plot_best_action_ratio(best_action_ratios_dict, algorithm_names, save_path="./result/best_action_ratio.jpg"):
    """ 绘制最佳行动占比对比图并保存为独立的图片 """
    plt.figure(figsize=(12, 6))
    for algo_name in algorithm_names:
        plt.plot(best_action_ratios_dict[algo_name], label=algo_name, linewidth=2)
    plt.title('Best Action Ratios Comparison')
    plt.xlabel('Steps')
    plt.ylabel('Best Action Ratios')
    plt.legend()
    plt.tight_layout()
    os.makedirs(os.path.dirname(save_path), exist_ok=True)
    plt.savefig(save_path)

def main():
    np.random.seed(1)  # 设定随机种子，确保实验可重复
    K = 10  # 拉杆数
    bandit = BernoulliBandit(K)

    # 创建算法实例
    random_solver = RandomSelection(bandit)
    epsilonGreedy_solver = EpsilonGreedy(bandit, 0.01, 0.5)
    ucb_solver = UCB(bandit, 0.5, 0.5)
    thompsonSampling_solver = ThompsonSampling(bandit)
    gradientBandit_solver = GradientBandit(bandit, 1)

    # 运行1000步
    num_steps = 5000
    random_solver.run(num_steps)
    epsilonGreedy_solver.run(num_steps)
    ucb_solver.run(num_steps)
    thompsonSampling_solver.run(num_steps)
    gradientBandit_solver.run(num_steps)

    # 调用绘制结果的函数
    plot_results(random_solver.cumulative_rewards, random_solver.cumulative_regrets, random_solver.best_action_ratio, "Random")
    plot_results(epsilonGreedy_solver.cumulative_rewards, epsilonGreedy_solver.cumulative_regrets, epsilonGreedy_solver.best_action_ratio, "ϵ-EpsilonGreedy")
    plot_results(ucb_solver.cumulative_rewards, ucb_solver.cumulative_regrets, ucb_solver.best_action_ratio, "UCB")
    plot_results(thompsonSampling_solver.cumulative_rewards, thompsonSampling_solver.cumulative_regrets, thompsonSampling_solver.best_action_ratio, "ThompsonSampling")
    plot_results(gradientBandit_solver.cumulative_rewards, gradientBandit_solver.cumulative_regrets, gradientBandit_solver.best_action_ratio,"GradientBandit")

    # 存储结果，绘制最终对比图
    cumulative_rewards_dict = {
        "ϵ-EpsilonGreedy": epsilonGreedy_solver.cumulative_rewards,
        "UCB": ucb_solver.cumulative_rewards,
        "ThompsonSampling": thompsonSampling_solver.cumulative_rewards,
        "GradientBandit": gradientBandit_solver.cumulative_rewards
    }
    cumulative_regrets_dict = {
        "ϵ-EpsilonGreedy": epsilonGreedy_solver.cumulative_regrets,
        "UCB": ucb_solver.cumulative_regrets,
        "ThompsonSampling": thompsonSampling_solver.cumulative_regrets,
        "GradientBandit": gradientBandit_solver.cumulative_regrets
    }
    best_action_ratios_dict = {
        "ϵ-EpsilonGreedy": epsilonGreedy_solver.best_action_ratio,
        "UCB": ucb_solver.best_action_ratio,
        "ThompsonSampling": thompsonSampling_solver.best_action_ratio,
        "GradientBandit": gradientBandit_solver.best_action_ratio
    }
    algorithm_names = ["ϵ-EpsilonGreedy", "UCB", "ThompsonSampling", "GradientBandit"]

    # 调用绘图函数分别保存三张图片
    plot_cumulative_rewards(cumulative_rewards_dict, algorithm_names, save_path="./result/cumulative_rewards.jpg")
    plot_cumulative_regret(cumulative_regrets_dict, algorithm_names, save_path="./result/cumulative_regrets.jpg")
    plot_best_action_ratio(best_action_ratios_dict, algorithm_names, save_path="./result/best_action_ratio.jpg")

    # 输出每个算法的结果字典（只显示最后一步的结果）
    print("Cumulative Rewards (last step):")
    for algo_name in algorithm_names:
        print(f"{algo_name}: {cumulative_rewards_dict[algo_name][-1]}")  # 显示最后一步的累计奖励

    print("\nCumulative Regrets (last step):")
    for algo_name in algorithm_names:
        print(f"{algo_name}: {cumulative_regrets_dict[algo_name][-1]}")  # 显示最后一步的累计懊悔

    print("\nBest Action Ratios (last step):")
    for algo_name in algorithm_names:
        print(f"{algo_name}: {best_action_ratios_dict[algo_name][-1]}")  # 显示最后一步的最佳行动占比


if __name__ == '__main__':
    main()
    # epsilonGreedy_different_epsilon_plot_results()