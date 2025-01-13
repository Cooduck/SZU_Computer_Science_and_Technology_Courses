# multiAgents.py(简校滨_高性能班_2022150176)
# --------------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


from util import manhattanDistance
from game import Directions
import random, util
from game import Agent
from util import AStar

class ReflexAgent(Agent):
    """
    A reflex agent chooses an action at each choice point by examining
    its alternatives via a state evaluation function.

    The code below is provided as a guide.  You are welcome to change
    it in any way you see fit, so long as you don't touch our method
    headers.
    (简校滨_高性能班_2022150176)
    """

    def getAction(self, gameState):
        """
        You do not need to change this method, but you're welcome to.

        getAction chooses among the best options according to the evaluation function.

        getAction takes a GameState and returns some Directions.X for some X in the set {NORTH, SOUTH, WEST, EAST, STOP}
        (简校滨_高性能班_2022150176)
        """
        # Collect legal moves and successor states
        legalMoves = gameState.getLegalActions()

        # Choose one of the best actions
        scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
        bestScore = max(scores)
        bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
        chosenIndex = random.choice(bestIndices)  # Pick randomly among the best

        "Add more of your code here if you want to"
        return legalMoves[chosenIndex]

    def evaluationFunction(self, currentGameState, action):
        # 获取执行动作后的游戏状态
        successorGameState = currentGameState.generatePacmanSuccessor(action)

        # 提取后继状态中的一些有用信息
        newPos = successorGameState.getPacmanPosition()  # Pacman的新位置
        newFood = successorGameState.getFood()  # 新的食物布局
        newGhostPos = successorGameState.getGhostPositions()  # 幽灵的新位置列表
        newGhostStates = successorGameState.getGhostStates()  # 获取幽灵的状态信息
        newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]  # 获取每个幽灵的恐惧时间
        walls = successorGameState.getWalls()  # 获取墙的布局
        capsules = successorGameState.getCapsules()  # 获取胶囊的位置

        # 设置危险区域
        # 如果地图是默认的mediumClassic，设定特定区域为危险区域
        if walls[7][4] and walls[7][5] and walls[7][6] and walls[8][4] and walls[8][6] and walls[9][4] \
                and walls[10][4] and walls[11][4] and walls[11][6] and walls[12][4] and walls[12][5] and walls[12][6]:
            dangerArea = [(8, 5), (9, 5), (9, 6), (10, 5), (10, 6), (11, 5)]  # 定义这些位置为危险区域
        else:
            dangerArea = []     # 其他地图则不需要设置危险区域

        # 初始化评估分数
        score = successorGameState.getScore()  # 初始得分等于游戏状态的得分
        calPathDis = AStar(walls)  # 初始化A*算法实例，用于计算路径距离

        # 考虑距离最近食物的距离
        foodList = newFood.asList()  # 获取食物列表（True代表有食物，False代表没有食物）
        if successorGameState.getNumFood() != 0:  # 如果还有食物
            # 使用A*算法计算Pacman到每个食物的距离，找到最近的食物
            foodDistances = [calPathDis.aStarSearch(newPos, food, newGhostPos) for food in foodList]
            minFoodDistance = min(foodDistances)  # 获取最小的食物距离
            score += 10 / (1 + minFoodDistance)  # 根据食物距离调整得分，距离越近得分越高

        # 考虑距离幽灵的距离
        for i, ghostPos in enumerate(newGhostPos):
            ghostDistances = calPathDis.aStarSearch(newPos, ghostPos)  # 计算Pacman到幽灵的距离
            if ghostDistances <= 5 and newScaredTimes[i] <= 3:  # 幽灵被吓到的剩余时间小于等于3，且距离Pacman较近
                score -= 30 * (6 - ghostDistances)  # 如果幽灵离得很近，扣分
            elif newScaredTimes[i] >= 10 and ghostDistances <= 4:  # 幽灵被吓到的剩余时间较长，且距离较近
                if ghostPos in [(22, 4), (22, 5), (22, 3), (21, 4)]:    # 当地图为openClassic时，避免在ghost复活点附近吃掉ghost
                    score -= 30 * (6 - ghostDistances)  # 如果幽灵离得很近，扣分
                else:
                    score += 15 / (1 + ghostDistances)  # 如果幽灵被吓到，且距离较近，奖励分，因为吃掉被吓到的幽灵能得到更高的分数

        # 如果Pacman进入了危险区域，扣除更多的分数
        if newPos in dangerArea:
            score -= 50  # 扣分

        # 考虑胶囊的距离
        for capsule in capsules:
            capsuleDistance = calPathDis.aStarSearch(newPos, capsule, newGhostPos)  # 计算Pacman到能量豆的距离
            if capsuleDistance <= 5:  # 如果距离较近
                score += 2 * (6 - capsuleDistance)  # 奖励分数
        # 如果吃到了胶囊，则得到奖励分数
        if len(currentGameState.getCapsules()) > len(capsules):
            score += 15  # 奖励分数

        return score  # 返回最终的评估得分


def scoreEvaluationFunction(currentGameState):
    """
    This default evaluation function just returns the score of the state.
    The score is the same one displayed in the Pacman GUI.

    This evaluation function is meant for use with adversarial search agents
    (not reflex agents).
    (简校滨_高性能班_2022150176)
    """
    return currentGameState.getScore()


class MultiAgentSearchAgent(Agent):
    """
    This class provides some common elements to all of your
    multi-agent searchers.  Any methods defined here will be available
    to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

    You *do not* need to make any changes here, but you can if you want to
    add functionality to all your adversarial search agents.  Please do not
    remove anything, however.

    Note: this is an abstract class: one that should not be instantiated.  It's
    only partially specified, and designed to be extended.  Agent (game.py)
    is another abstract class.
    (简校滨_高性能班_2022150176)
    """

    def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
        self.index = 0 # Pacman is always agent index 0
        # self.evaluationFunction = util.lookup(evalFn, globals())
        self.evaluationFunction = betterEvaluationFunction
        self.depth = int(depth)


class MinimaxAgent(MultiAgentSearchAgent):
    """
    Your minimax agent (question 2)
    (简校滨_高性能班_2022150176)
    """

    def getAction(self, gameState):
        """
        Returns the minimax action from the current gameState using self.depth
        and self.evaluationFunction.

        Here are some method calls that might be useful when implementing minimax.

        gameState.getLegalActions(agentIndex):
        Returns a list of legal actions for an agent
        agentIndex=0 means Pacman, ghosts are >= 1

        gameState.generateSuccessor(agentIndex, action):
        Returns the successor game state after an agent takes an action

        gameState.getNumAgents():
        Returns the total number of agents in the game

        gameState.isWin():
        Returns whether or not the game state is a winning state

        gameState.isLose():
        Returns whether or not the game state is a losing state
        (简校滨_高性能班_2022150176)
        """
        "*** YOUR CODE HERE ***"
        # 获取所有合法的动作
        legalMoves = gameState.getLegalActions()

        # 对每个合法动作计算Minimax值，并存储结果
        scores = [self.minimax(1, gameState.generateSuccessor(0, action), 1) for action in legalMoves]

        # 找到评分最高的动作
        bestScore = max(scores)

        # 找到所有具有最佳得分的动作的索引
        bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]

        # 随机选择一个最佳动作
        chosenIndex = random.choice(bestIndices)

        # 返回选择的动作
        return legalMoves[chosenIndex]

    def minimax(self, agentIndex, gameState, depth):
        # 递归终止条件：到达最大深度，或当前状态为胜利或失败状态
        if depth == self.depth * 2 or gameState.isWin() or gameState.isLose():
            return self.evaluationFunction(gameState)   # 对叶节点进行评估

        if agentIndex == 0:  # Pacman的回合
            # 获取Pacman的所有合法动作
            pacmanLegalMoves = gameState.getLegalActions(agentIndex)

            # 对每个合法动作计算其后继状态的Minimax值
            scores = [self.minimax(1, gameState.generateSuccessor(agentIndex, action), depth + 1) for action in
                      pacmanLegalMoves]

            # 返回最大得分
            bestScore = max(scores)
            return bestScore

        else:  # 幽灵的回合
            # 获取幽灵的所有合法动作
            ghostLegalMoves = gameState.getLegalActions(agentIndex)

            # 如果是最后一个幽灵，则递归到Pacman的回合
            if agentIndex == gameState.getNumAgents() - 1:
                scores = [self.minimax(0, gameState.generateSuccessor(agentIndex, action), depth + 1) for action in
                          ghostLegalMoves]
            else:
                # 否则继续递归到下一个幽灵
                scores = [self.minimax(agentIndex + 1, gameState.generateSuccessor(agentIndex, action), depth) for
                          action in ghostLegalMoves]

            # 返回最小得分
            bestScore = min(scores)
            return bestScore

class AlphaBetaAgent(MultiAgentSearchAgent):
    """
    Your minimax agent with alpha-beta pruning (question 3)
    (简校滨_高性能班_2022150176)
    """
    def getAction(self, gameState):
        """
        Returns the minimax action using self.depth and self.evaluationFunction
        (简校滨_高性能班_2022150176)
        """
        "*** YOUR CODE HERE ***"
        alpha = float("-inf")  # 初始化Alpha值（Pacman的最佳分数）
        beta = float("inf")  # 初始化Beta值（幽灵的最差分数）
        bestAction = []  # 用于存储Pacman的最佳行动

        # 获取Pacman（index 0）所有合法的动作
        legalMoves = gameState.getLegalActions(0)

        # 对每个合法动作进行评估
        for action in legalMoves:
            successor = gameState.generateSuccessor(0, action)  # 生成Pacman执行该动作后的后继状态
            score = self.alphaBeta(1, successor, 1, alpha, beta)  # 递归评估该后继状态的分数
            if score > alpha:  # 如果当前分数优于当前最佳分数
                alpha = score  # 更新Alpha值为新的最佳分数
                bestAction.clear()         # 清空bestAction
                bestAction.append(action)  # 更新最佳行动为当前动作
            elif score == alpha:
                bestAction.append(action)  # 添加当前行动到最佳行动中

        return random.choice(bestAction)  # 随机返回Pacman的最佳行动

    def alphaBeta(self, agentIndex, gameState, depth, alpha, beta):
        """
        Alpha-Beta pruning implementation.
        (简校滨_高性能班_2022150176)
        """
        # 终止条件：达到最大深度或游戏结束（胜利或失败）
        if depth == self.depth * 2 or gameState.isWin() or gameState.isLose():
            return self.evaluationFunction(gameState)  # 返回当前游戏状态的评估值

        if agentIndex == 0:  # Pacman的回合（最大化者）
            value = float("-inf")  # 初始化Pacman的值为负无穷（尽量最大化）
            for action in gameState.getLegalActions(agentIndex):  # 遍历所有合法动作
                successor = gameState.generateSuccessor(agentIndex, action)  # 生成后继状态
                value = max(value, self.alphaBeta(1, successor, depth + 1, alpha, beta))  # 从所有动作中选择最大值
                if value > beta:  # 如果当前值大于Beta值，说明可以剪枝
                    return value
                alpha = max(alpha, value)  # 更新Alpha值为当前最大值
            return value  # 返回Pacman的最佳分数（最大化）

        else:  # 幽灵的回合（最小化者）
            value = float("inf")  # 初始化幽灵的值为正无穷（尽量最小化）
            for action in gameState.getLegalActions(agentIndex):  # 遍历所有幽灵的合法动作
                successor = gameState.generateSuccessor(agentIndex, action)  # 生成后继状态
                if agentIndex == gameState.getNumAgents() - 1:  # 如果是最后一个幽灵的回合
                    value = min(value, self.alphaBeta(0, successor, depth + 1, alpha, beta))  # 递归进行Pacman的回合
                else:
                    value = min(value, self.alphaBeta(agentIndex + 1, successor, depth, alpha, beta))  # 递归进行下一个幽灵的回合
                if value < alpha:  # 如果当前值小于Alpha值，说明可以剪枝
                    return value
                beta = min(beta, value)  # 更新Beta值为当前最小值
            return value  # 返回幽灵的最佳分数（最小化）


def betterEvaluationFunction(currentGameState):
    # 获取当前游戏状态的一些重要信息
    newPos = currentGameState.getPacmanPosition()  # 获取Pacman当前位置
    newFood = currentGameState.getFood()  # 获取当前食物的位置布局
    newGhostPos = currentGameState.getGhostPositions()  # 获取幽灵的位置列表
    newGhostStates = currentGameState.getGhostStates()  # 获取幽灵的状态
    newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]  # 获取每个幽灵的恐惧时间
    walls = currentGameState.getWalls()  # 获取墙的布局
    capsules = currentGameState.getCapsules()  # 获取胶囊的位置

    # 设置危险区域
    # 如果地图是默认的mediumClassic，设定特定区域为危险区域
    if walls[7][4] and walls[7][5] and walls[7][6] and walls[8][4] and walls[8][6] and walls[9][4] \
            and walls[10][4] and walls[11][4] and walls[11][6] and walls[12][4] and walls[12][5] and walls[12][6]:
        dangerArea = [(8, 5), (9, 5), (9, 6), (10, 5), (10, 6), (11, 5)]  # 定义这些位置为危险区域
    # 如果地图是smallClassic，设定特定区域为危险区
    elif walls[7][4] and walls[7][5] and walls[7][6] and walls[8][4] and walls[8][6] and walls[9][6] \
            and walls[10][6] and walls[11][4] and walls[11][6] and walls[12][4] and walls[12][5] and walls[12][6]:
        dangerArea = [(8, 5), (9, 5), (9, 4), (10, 5), (10, 4), (11, 5)]  # 定义这些位置为危险区域
    else:
        dangerArea = []  # 其他地图则不需要设置危险区域

    # 初始化得分，当前得分是游戏状态中的基本得分
    score = currentGameState.getScore()

    # 使用A*算法来计算路径距离，辅助计算从Pacman到食物、幽灵等的距离
    calPathDis = AStar(walls)

    # 评估与食物的距离，并根据距离食物的远近调整得分
    foodList = newFood.asList()  # 获取食物的位置列表
    if currentGameState.getNumFood() != 0:  # 如果还有食物
        # 使用A*算法计算Pacman到每个食物的距离
        foodDistances = [calPathDis.aStarSearch(newPos, food, newGhostPos) for food in foodList]
        minFoodDistance = min(foodDistances)  # 找到距离最近的食物
        # 食物越近，得分越高。使用距离的倒数来计算得分（越近分数越高）
        score += 10 / (1 + minFoodDistance)

    # 评估与幽灵的距离
    for i, ghostPos in enumerate(newGhostPos):
        ghostDistances = calPathDis.aStarSearch(newPos, ghostPos)  # 计算Pacman到幽灵的距离
        if ghostDistances <= 5 and newScaredTimes[i] <= 3:  # 幽灵太近且被吓到的剩余时间过短
            score -= 30 * (6 - ghostDistances)
        elif newScaredTimes[i] >= 8 and ghostDistances <= 5:   # 幽灵被吓到的剩余时间还长且距离较近
            score += 100 / (1 + ghostDistances)

    # 如果Pacman进入了危险区域，给予更大的负面惩罚
    if newPos in dangerArea:
        score -= 50

    # 评估与胶囊的距离，胶囊对Pacman有很大的帮助
    for capsule in capsules:
        capsuleDistance = calPathDis.aStarSearch(newPos, capsule, newGhostPos)  # 计算到胶囊的距离
        if capsuleDistance <= 6:  # 如果胶囊离Pacman很近
            # 如果Pacman距离胶囊较近，奖励分数
            score += 3 * (7 - capsuleDistance)
    # 如果吃到了胶囊，给予额外奖励
    if (sum(newScaredTimes) / len(newScaredTimes)) > 0:
        # 通过幽灵的总恐惧时间来判断是否奖励
        score += 20

    # 返回最终的评估得分
    return score


# Abbreviation
better = betterEvaluationFunction
