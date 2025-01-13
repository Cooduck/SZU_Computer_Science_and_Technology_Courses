"""
main.py
这段代码实现了一个简易的俄罗斯方块游戏互动环境，提供了三种操作模式：玩家操控模式、AI操控模式和退出游戏选项。
通过命令行界面，玩家可以选择手动操控游戏，或让预训练的AI模型进行操作，并显示游戏的得分、消除行数和坚持回合数等信息。
"""
from Tetris import manual_operation
from Test import ai_operation
import pyfiglet

if __name__ == '__main__':
    print("*" * 80)
    title = pyfiglet.figlet_format("Welcome to Tetris")
    print(title)
    # print("Welcome to Tetris")
    print("*" * 80)

    while True:
        print("输入1：玩家操控模式，输入2：AI操控模式，输入3：退出")
        choice = input("请输入您的选择：")
        if choice == '1':
            print("进入玩家操控模式...")
            print("操作规则：")
            print("←：左移方块")
            print("→：右移方块")
            print("↑：顺时针旋转方块")
            print("↓：加速下落方块")
            score, tot_clear_lines, tot_blocks = manual_operation()
            print(f"本轮游戏结束，得分为{score}，消除行数为{tot_clear_lines}，坚持回合数为{tot_blocks}")
        elif choice == '2':
            print("进入AI操控模式...")
            score, tot_clear_lines, tot_blocks = ai_operation("./Best Model/best_model.pth")
            print(f"本轮游戏结束，得分为{score}，消除行数为{tot_clear_lines}，坚持回合数为{tot_blocks}")
        elif choice == '3':
            print("退出游戏")
            break
        else:
            print("无效输入，请输入1或2！")


