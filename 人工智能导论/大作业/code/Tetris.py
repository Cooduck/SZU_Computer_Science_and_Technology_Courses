"""
Tetris.py
这个代码实现了一个简单的俄罗斯方块游戏，玩家可以通过键盘控制方块的移动（左右、下落、旋转）。
"""
import pygame
import numpy as np
from pygame.locals import *
import random
from Tetris_config import *


class GameArea:
    """ GameArea类用于管理游戏的网格区域，包括方块的放置、行的清除、检测游戏结束等 """

    def __init__(self, width, height):
        """ 初始化游戏区域 """
        self.width = width
        self.height = height
        self.area = [[0] * width for _ in range(height)]  # 游戏区域初始化为空
        self.game_over = False

    def reset(self):
        """ 重置游戏区域 """
        self.area = [[0] * self.width for _ in range(self.height)]
        self.game_over = False

    def is_full(self, row):
        """ 检查某一行是否已满 """
        return all(x == 1 for x in self.area[row])

    def clear_full_lines(self):
        """ 清除已满的行，并将上方的行下移，返回清除的行数 """
        remove_idxs = []
        for i in range(self.height):
            if self.is_full(i):
                remove_idxs.append(i)
        if remove_idxs:
            for idx1 in remove_idxs:
                # 移动上面的行
                for idx2 in range(idx1, 0, -1):
                    self.area[idx2] = self.area[idx2 - 1]
                self.area[0] = [0] * self.width  # 新行填充为空
            return len(remove_idxs)
        else:
            return 0

    def dock(self, block):
        """ 将方块固定在当前网格中，并检查是否触发游戏结束 """
        if not self.check_move(block, 1, 1):
            cur_pos_x = block.current_pos_x
            cur_pos_y = block.current_pos_y
            for i in range(4):  # 每个方块都有4个坐标
                x_offset, y_offset = block.get_shape()[i]
                # 只有在方块不超出上边界并不与已近放置的方块重合时才固定
                if cur_pos_y + y_offset >= 0 and self.area[cur_pos_y + y_offset][cur_pos_x + x_offset] == 0:
                    self.area[cur_pos_y + y_offset][cur_pos_x + x_offset] = 1
                elif cur_pos_y + y_offset < 0 or self.area[cur_pos_y + y_offset][cur_pos_x + x_offset] == 1:
                    self.game_over = True
                    break
        return self.game_over

    def get_state(self, block, pos_x, pos_y):
        """ 返回加入当前方块后的游戏区域状态 """
        now_state = [row[:] for row in self.area]
        for i in range(4):
            x_offset, y_offset = block.get_shape()[i]  # 获取每个坐标的偏移量
            now_state[pos_y + y_offset][pos_x + x_offset] = 1
        return np.array(now_state).flatten()

    def check_move(self, block, idx=0, delta=0):
        """ 检查方块在当前方向上的移动是否合法（是否会与其他方块重合或超出边界） """
        pos_x = block.current_pos_x + (delta if idx == 0 else 0)
        pos_y = block.current_pos_y + (delta if idx == 1 else 0)
        for i in range(4):  # 每个方块都有4个坐标
            x_offset, y_offset = block.get_shape()[i]  # 获取每个坐标的偏移量
            if pos_y + y_offset >= self.height:  # 超过底部
                return False
            if pos_x + x_offset < 0 or pos_x + x_offset >= self.width:  # 超过左右边界
                return False
            if pos_y + y_offset >= 0 and self.area[pos_y + y_offset][pos_x + x_offset] != 0:  # 碰到其他方块
                return False
        return True

    def get_holes(self):
        """ 计算游戏区域中的空洞数 """
        num_holes = 0
        for col in range(0, self.width):
            row = 0
            while row < self.height and self.area[row][col] == 0:
                row += 1
            row += 1
            while row < self.height:
                if self.area[row][col] == 0:
                    num_holes += 1
                row += 1
        return num_holes

    def get_height_and_bumpiness(self):
        """ 计算每一列的高度和凹凸度（每列相邻两列的高度差的绝对值之和） """
        heights = [0] * self.width  # 假设每列的初始高度为0
        for col in range(self.width):  # 遍历每一列
            for row in range(self.height):  # 从底到顶遍历每一列
                if self.area[row][col] == 1:
                    heights[col] = self.height - row  # 记录方块的最高位置
                    break  # 找到第一个方块就可以停止检查这列
        currs = np.array(heights[:-1])
        nexts = np.array(heights[1:])
        diffs = np.abs(currs - nexts)
        bumpiness = np.sum(diffs)
        return sum(heights), bumpiness.tolist()


class Block:
    """ Block类表示一个俄罗斯方块，包括方块的形状、位置、旋转状态等 """

    def __init__(self, block, block_id, pos_x=5, pos_y=1):
        """ 初始化方块，随机选择方块的初始旋转状态并设置初始位置 """
        self.block_id = block_id
        self.block = block
        self.shape = random.randint(0, len(block) - 1)
        self.current_pos_x = pos_x
        self.current_pos_y = pos_y

    def get_next_shape(self):
        """ 获取下一个旋转状态的方块 """
        new_block = Block(self.block, self.block_id, self.current_pos_x, self.current_pos_y)
        new_block.shape = (self.shape + 1) % len(self.block)
        return new_block

    def rotate_k_time(self, k):
        """ 将方块旋转k次 """
        self.shape = (self.shape + k) % len(self.block)

    def get_shape(self):
        """ 获取当前旋转状态下方块的形状 """
        return self.block[self.shape]

    def get_block_shape(self):
        """ 将方块的形状转化为4x4的矩阵表示 """
        block_shape = np.zeros((4, 4), dtype=int)
        for x, y in self.block[self.shape]:
            # 将方块的坐标转化为矩阵中的对应位置
            block_shape[x + 2, y + 2] = 1  # +2 是为了确保坐标在 4x4 的范围内
        return block_shape.tolist()


class Tetris:
    """ Tetris类用于管理整个俄罗斯方块游戏的状态和界面，包括初始化、操作、显示等功能 """

    def __init__(self, block_size, area_width, area_height, border_width, border_color,
                    screen_width, screen_height, bg_color):
        """ 初始化游戏相关参数，如方块大小、游戏区域大小、窗口尺寸等 """
        self.block_size = block_size
        self.area_width = area_width
        self.area_height = area_height
        self.border_width = border_width
        self.border_color = border_color
        self.screen_width = screen_width
        self.screen_height = screen_height
        self.bg_color = bg_color
        self.game_area = GameArea(area_width, area_height)
        self.block_types = [
            # "L" 方块
            [[(0, 0), (-1, 0), (1, 0), (-1, -1)],  # 旋转 0
            [(0, 1), (0, 0), (0, -1), (1, -1)],  # 旋转 1
            [(1, 1), (-1, 0), (0, 0), (1, 0)],  # 旋转 2
            [(-1, 1), (0, 1), (0, 0), (0, -1)]],  # 旋转 3
            # "J" 方块
            [[( -1, 0), (0, 0), (1, 0), (1, -1)],  # 旋转 0
            [(0, -1), (0, 0), (0, 1), (1, 1)],  # 旋转 1
            [(-1, 1), (-1, 0), (0, 0), (1, 0)],  # 旋转 2
            [(-1, -1), (0, -1), (0, 0), (0, 1)]],  # 旋转 3
            # "T" 方块
            [[(-1, 0), (0, -1), (0, 0), (1, 0)],  # 旋转 0
            [(0, -1), (0, 0), (0, 1), (1, 0)],  # 旋转 1
            [(-1, 0), (0, 0), (1, 0), (0, 1)],  # 旋转 2
            [(0, -1), (0, 0), (-1, 0), (0, 1)]],  # 旋转 3
            # "Z" 方块
            [[(-1, 0), (0, -1), (0, 0), (1, -1)],  # 旋转 0
            [(0, -1), (0, 0), (1, 0), (1, 1)]],  # 旋转 1
            # "S" 方块
            [[(-1, -1), (0, -1), (0, 0), (1, 0)],  # 旋转 0
            [(1, -1), (1, 0), (0, 0), (0, 1)]],  # 旋转 1
            # "I" 方块
            [[(-2, 0), (-1, 0), (0, 0), (1, 0)],  # 旋转 0
            [(0, -2), (0, -1), (0, 0), (0, 1)]],  # 旋转 1
            # "O" 方块
            [[(-1, -1), (0, -1), (-1, 0), (0, 0)]]  # 旋转 1
        ]
        self.score = 0  # 初始分数
        self.current_block = None  # 当前方块
        self.fall_time = 0  # 记录方块下落的时间
        self.fall_speed = 500  # 下落速度
        self.is_display = True  # 是否显示界面
        self.tot_blocks = 0  # 方块总数
        self.game_over = False  # 游戏是否结束
        self.tot_clear_lines = 0  # 总消除行数
        self.start_time = None  # 游戏开始时间

        # 展示游戏界面的参数
        self.screen = None  # 屏幕
        self.font1 = None  # 字体
        self.font_pos_x = None  # 字体位置（X轴）
        self.font1_height = None  # 字体高度
        self.clock = None  # 游戏时钟，控制帧率

    def reset(self, is_display=True):
        """ 重置游戏状态 """
        self.game_over = False
        self.game_area.reset()
        self.score = 0
        self.tot_clear_lines = 0
        self.current_block = None
        self.fall_time = 0
        self.tot_blocks = 0
        self.is_display = is_display
        self.init_game_area()

    def get_new_block(self):
        """ 获取一个新的方块 """
        idx = random.randint(0,6)
        self.current_block = Block(self.block_types[idx], idx)

    def print_text(self, screen, font, x, y, text, fcolor=(255, 255, 255)):
        """ 在屏幕上显示文本 """
        imgText = font.render(text, True, fcolor)
        screen.blit(imgText, (x, y))

    def take_action(self, action):
        """ 根据玩家输入的动作执行相应操作 """
        if action == 0 and self.game_area.check_move(self.current_block, 0, -1):
            self.current_block.current_pos_x -= 1
        elif action == 1 and self.game_area.check_move(self.current_block, 0, 1):
            self.current_block.current_pos_x += 1
        elif action == 2 and self.game_area.check_move(self.current_block, 1, 1):
            self.current_block.current_pos_y += 1
        elif action == 3:
            new_block = self.current_block.get_next_shape()
            if self.game_area.check_move(new_block):
                self.current_block = new_block

    def auto_fall(self):
        """ 自动下落处理 """
        if pygame.time.get_ticks() - self.fall_time > self.fall_speed:
            if self.game_area.check_move(self.current_block, 1, 1):
                self.current_block.current_pos_y += 1
            self.fall_time = pygame.time.get_ticks()

    def cal_score(self):
        """ 更新分数并返回当前回合得分 """
        reward = 0
        if not self.game_area.check_move(self.current_block, 1, 1):
            self.game_over = self.game_area.dock(self.current_block)
            clear_lines = self.game_area.clear_full_lines()
            if clear_lines != 0:
                if clear_lines == 1:
                    reward += 1
                elif clear_lines == 2:
                    reward += 3
                elif clear_lines == 3:
                    reward += 6
                elif clear_lines >= 4:
                    reward += 10
                self.score += reward
                self.tot_clear_lines += clear_lines
            self.get_new_block()
            self.tot_blocks += 1
        return reward

    def init_game_area(self):
        """ 初始化游戏 """
        pygame.init()
        self.start_time = pygame.time.get_ticks()
        if self.is_display:
            self.screen = pygame.display.set_mode((self.screen_width, self.screen_height))
            pygame.display.set_caption('俄罗斯方块')

            self.font1 = pygame.font.SysFont('SimHei', 24)
            self.font_pos_x = self.area_width * self.block_size + self.border_width + 10
            self.font1_height = int(self.font1.size('得分')[1])

        self.clock = pygame.time.Clock()  # 控制游戏帧率

        self.get_new_block()

    def display(self):
        """ 展示游戏界面 """
        if self.is_display:
            # 填充背景色
            self.screen.fill(self.bg_color)

            # 画游戏区域分隔线
            pygame.draw.line(self.screen, self.border_color,
                             (self.block_size * self.area_width + self.border_width // 2, 0),
                             (self.block_size * self.area_width + self.border_width // 2, self.screen_height),
                             self.border_width)
            # 画网格线 竖线
            for x in range(self.area_width):
                pygame.draw.line(self.screen, (0, 0, 0), (x * self.block_size, 0), (x * self.block_size, self.screen_height),
                                 1)
            # 画网格线 横线
            for y in range(self.area_height):
                pygame.draw.line(self.screen, (0, 0, 0), (0, y * self.block_size),
                                 (self.area_width * self.block_size, y * self.block_size), 1)

            # 绘制固定的方块
            for y in range(self.area_height):
                for x in range(self.area_width):
                    if self.game_area.area[y][x] == 1:  # 已经被固定的方块
                        pygame.draw.rect(self.screen, (255, 255, 0),
                                         pygame.Rect(x * self.block_size, y * self.block_size, self.block_size,
                                                     self.block_size))

            # 绘制当前方块
            for dx, dy in self.current_block.get_shape():
                pygame.draw.rect(self.screen, (255, 255, 0), pygame.Rect((self.current_block.current_pos_x + dx) * self.block_size,
                                                                    (self.current_block.current_pos_y + dy) * self.block_size,
                                                                    self.block_size, self.block_size))

            self.print_text(self.screen, self.font1, self.font_pos_x, 10, f'得分: ')
            self.print_text(self.screen, self.font1, self.font_pos_x, 10 + self.font1_height + 6, f'{self.score}')
            self.print_text(self.screen, self.font1, self.font_pos_x, 10 + (self.font1_height + 6) * 2, f'消除总行数: ')
            self.print_text(self.screen, self.font1, self.font_pos_x, 10 + (self.font1_height + 6) * 3,f'{self.tot_clear_lines}')
            elapsed_time = (pygame.time.get_ticks() - self.start_time) / 1000  # 转换为秒
            self.print_text(self.screen, self.font1, self.font_pos_x, 10 + (self.font1_height + 6) * 4, f'计时: ')
            self.print_text(self.screen, self.font1, self.font_pos_x, 10 + (self.font1_height + 6) * 5, f'{elapsed_time}s')

            self.clock.tick(60)
            pygame.display.flip()

    def step(self, action, is_display=True):
        """ 游戏每一步响应 """
        # 执行动作
        if action is not None:
            self.take_action(action)

        # 自动下落
        self.auto_fall()

        # 计算分数
        self.cal_score()

        # 展示游戏界面
        self.display()

        # 游戏结束退出
        if self.game_over:
            pygame.quit()

        return self.game_over


def manual_operation():
    """ 用户操控游戏模式 """
    done = False
    game = Tetris(SIZE, AREA_WIDTH, AREA_HEIGHT, BORDER_WIDTH, BORDER_COLOR, SCREEN_WIDTH, SCREEN_HEIGHT, BG_COLOR)
    game.reset(True)
    while not done:
        action = None
        for event in pygame.event.get():
            if event.type == KEYDOWN:
                if event.key == K_LEFT:
                    action = 0
                elif event.key == K_RIGHT:
                    action = 1
                elif event.key == K_DOWN:
                    action = 2
                elif event.key == K_UP:
                    action = 3

        done = game.step(action)
    return game.score, game.tot_clear_lines, game.tot_blocks


if __name__ == '__main__':
    manual_operation()
