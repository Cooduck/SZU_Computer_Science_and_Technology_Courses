"""
TetrisAI.py
该代码实现了一个供智能体使用的俄罗斯方块游戏，TetrisAI类继承自Tetris类。
TetrisAI类基于AI智能体增强了游戏的基本功能，并且能够通过人工操作来生成游戏经验数据。
"""
from Tetris import *
import copy
import json
import os


class TetrisAI(Tetris):
    """ 继承Tetris类，增加AI智能体相关功能 """
    def __init__(self, block_size, area_width, area_height, border_width, border_color,
                 screen_width, screen_height, bg_color):
        super().__init__(block_size, area_width, area_height, border_width, border_color,
                         screen_width, screen_height, bg_color)
        # 展示游戏界面的参数
        self.screen = None
        self.font1 = None
        self.font_pos_x = None
        self.font1_height = None
        self.clock = None

    def reset(self, is_display=False):
        """ 重置游戏，返回初始状态 """
        self.game_over = False
        self.game_area.reset()
        self.score = 0
        self.current_block = None
        self.fall_time = 0
        self.is_display = True
        self.tot_blocks = 0
        self.tot_clear_lines = 0
        self.is_display = is_display
        self.init_game_area()
        state = self.get_state_properties(self.current_block, self.game_area)
        return state

    def get_state_properties(self, block, game_area):
        """ 获取当前状态的属性，包括游戏区域和当前方块的特征 """
        _ = game_area.dock(block)
        clear_lines = game_area.clear_full_lines()
        holes = game_area.get_holes()
        height, bumpiness = game_area.get_height_and_bumpiness()
        state = []
        state.extend(np.array(game_area.area).flatten().tolist())
        state.extend(np.array(block.get_block_shape()).flatten().tolist())
        state.extend([clear_lines, holes, bumpiness, height])
        return state

    def get_next_states(self):
        """ 获取当前方块的所有可能状态 """
        states = {}
        if self.current_block.block_id <= 2:
            num_rotations = 4
        elif 2 < self.current_block.block_id <= 5:
            num_rotations = 2
        else:
            num_rotations = 1

        for i in range(num_rotations):
            new_block = copy.deepcopy(self.current_block)
            new_block.rotate_k_time(i)
            valid_l = 0
            while self.game_area.check_move(new_block, 0, -(valid_l + 1)):
                valid_l += 1
            valid_r = 0
            while self.game_area.check_move(new_block, 0, (valid_r + 1)):
                valid_r += 1
            origin_pos_x = new_block.current_pos_x
            for j in range(-valid_l, valid_r + 1):
                new_game_area = copy.deepcopy(self.game_area)
                new_block.current_pos_x = origin_pos_x + j

                while new_game_area.check_move(new_block, 1, 1):
                    new_block.current_pos_y += 1

                next_state = self.get_state_properties(new_block, new_game_area)
                states[(i, j)] = next_state
        # 打乱状态后再返回
        return states

    def take_action(self, action):
        """ 执行动作，包含旋转和移动 """
        rotate, move = action
        while rotate:
            new_block = self.current_block.get_next_shape()
            if self.game_area.check_move(new_block):
                self.current_block = new_block
                rotate -= 1
            else:
                break
            self.display()

        while move > 0:
            if self.game_area.check_move(self.current_block, 0, 1):
                self.current_block.current_pos_x += 1
                move -= 1
            else:
                break
            self.display()
        while move < 0:
            if self.game_area.check_move(self.current_block, 0, -1):
                self.current_block.current_pos_x -= 1
                move += 1
            else:
                break
            self.display()

    def auto_fall(self):
        """ 自动下落方块 """
        while self.game_area.check_move(self.current_block, 1, 1):
            self.current_block.current_pos_y += 1
            self.display()

    def step(self, action, is_display=True):
        """ 执行动作并返回奖励、下一状态及是否游戏结束 """
        # 执行动作
        if action is not None:
            self.take_action(action)

        # 自动下落
        self.auto_fall()

        # 返回状态状态并计算得分
        reward = self.cal_score()

        if self.game_over:
            pygame.quit()

        return reward, self.game_area.game_over

    def manual_operation(self):
        """ 人工操作，模拟人类玩家操作 """
        if self.current_block.block_id <= 2:
            num_rotations = 4
        elif 2 < self.current_block.block_id <= 5:
            num_rotations = 2
        else:
            num_rotations = 1

        rotate = 0
        move = 0
        flag = 1
        while flag:
            new_block = self.current_block.get_next_shape()
            for event in pygame.event.get():
                if event.type == KEYDOWN:
                    if event.key == K_LEFT and self.game_area.check_move(self.current_block, 0, -1):
                        move -= 1
                        self.current_block.current_pos_x -= 1
                    elif event.key == K_RIGHT and self.game_area.check_move(self.current_block, 0, 1):
                        move += 1
                        self.current_block.current_pos_x += 1
                    elif event.key == K_UP and self.game_area.check_move(new_block):
                        rotate = (rotate + 1) % num_rotations
                        self.current_block = new_block
                    elif event.key == K_DOWN:
                        flag = 0
                        self.auto_fall()
                    elif event.key == K_ESCAPE: # 提前结束
                        return 0, (0, 0), 0, 2
            self.display()

        state = self.get_state_properties(self.current_block, self.game_area)
        self.get_new_block()
        self.tot_blocks += 1
        self.game_over = self.game_area.game_over
        reward = 0
        clear_line = state[-4]
        if clear_line != 0:
            if clear_line == 1:
                reward += 1
            elif clear_line == 2:
                reward += 3
            elif clear_line == 3:
                reward += 6
            elif clear_line >= 4:
                reward += 10
        self.score += reward
        self.tot_clear_lines += clear_line
        if self.game_over:
            pygame.quit()
        return reward, (rotate, move), state, self.game_over



def generate_manual_experience(env):
    """ 生成经验数据 """
    experience_data = []
    state = env.reset(is_display=True)
    done = False
    while not done:
        reward, action, next_state, done = env.manual_operation()
        if done == 2:   # 提前结束
            return experience_data
        # 保存当前经验
        experience_data.append({
            'state': state,
            'action': action,
            'reward': reward,
            'next_state': next_state,
            'done': done
        })
        state = next_state  # 更新状态
    return experience_data


def save_experience_to_file(experience_data, file_path):
    """ 将经验数据保存到文件 """
    # 检查文件是否存在
    if os.path.exists(file_path):
        # 如果文件存在，读取现有的经验数据
        with open(file_path, 'r') as f:
            # 尝试读取数据
            try:
                existing_data = json.load(f)
            except json.JSONDecodeError:
                # 如果文件为空或无法解码，初始化为空列表
                existing_data = []
    else:
        # 如果文件不存在，初始化为空列表
        existing_data = []

    # 将新经验追加到现有经验列表中
    existing_data.extend(experience_data)

    # 保存更新后的经验数据
    with open(file_path, 'w') as f:
        json.dump(existing_data, f)

    print(f"经验已保存到 {file_path}")


# 运行脚本，生成经验数据并保存
if __name__ == '__main__':
    file_path = './experience_data.json'  # 存储经验的文件路径
    env = TetrisAI(SIZE, AREA_WIDTH, AREA_HEIGHT, BORDER_WIDTH, BORDER_COLOR, SCREEN_WIDTH, SCREEN_HEIGHT, BG_COLOR)  # 创建环境
    experience_data = generate_manual_experience(env)  # 生成经验数据
    save_experience_to_file(experience_data, file_path)  # 保存经验数据到文件
