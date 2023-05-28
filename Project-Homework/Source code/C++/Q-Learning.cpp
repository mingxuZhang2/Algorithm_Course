#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <map>
#include <stack>
using namespace std;

class Maze {
    std::vector<std::vector<int>> maze_data;
    std::pair<int, int> robot_position;

public:
    Maze(const std::vector<std::vector<int>>& data);
    std::vector<std::string> can_move_actions(const std::pair<int, int>& position);
    void reset_robot();
    std::string sense_robot();
    double move_robot(const std::string& action);
    void print_maze();
    std::pair<int, int> get_robot_position() const;  // 新增函数，获取机器人当前位置
};

Maze::Maze(const std::vector<std::vector<int>>& data) : maze_data(data) {
    reset_robot();
}

std::vector<std::string> Maze::can_move_actions(const std::pair<int, int>& position) {
    std::vector<std::string> valid_actions;
    int x = position.first;
    int y = position.second;
    int width = maze_data[0].size();
    int height = maze_data.size();

    if (x > 0 && maze_data[y][x - 1] == 0) {
        valid_actions.push_back("l");
    }
    if (x < width - 1 && maze_data[y][x + 1] == 0) {
        valid_actions.push_back("r");
    }
    if (y > 0 && maze_data[y - 1][x] == 0) {
        valid_actions.push_back("u");
    }
    if (y < height - 1 && maze_data[y + 1][x] == 0) {
        valid_actions.push_back("d");
    }

    return valid_actions;
}

void Maze::reset_robot() {
    for (int y = 0; y < maze_data.size(); y++) {
        for (int x = 0; x < maze_data[0].size(); x++) {
            if (maze_data[y][x] == 3) {
                robot_position = std::make_pair(x, y);

                return;
            }
        }
    }
}

std::string Maze::sense_robot() {
    return std::to_string(robot_position.first) + "," + std::to_string(robot_position.second);
}

double Maze::move_robot(const std::string& action) {
    int x = robot_position.first;
    int y = robot_position.second;
    double reward = 0.0;

    if (action == "u" && y > 0 && maze_data[y - 1][x] != 1) {
        robot_position.second--;
        reward-=1;
    } else if (action == "r" && x < maze_data[0].size() - 1 && maze_data[y][x + 1] != 1) {
        robot_position.first++;
        reward-=1;
    } else if (action == "d" && y < maze_data.size() - 1 && maze_data[y + 1][x] != 1) {
        robot_position.second++;
        reward-=1;
    } else if (action == "l" && x > 0 && maze_data[y][x - 1] != 1) {
        robot_position.first--;
        reward-=1;
    } else {
        reward = -10.0;  // Invalid move, penalize
    }

    if (maze_data[robot_position.second][robot_position.first] == 2) {
        reward = 50.0;  // Reached the goal
    }

    return reward;
}

void Maze::print_maze() {
    for (const auto& row : maze_data) {
        for (const auto& cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
}

std::pair<int, int> Maze::get_robot_position() const {
    return robot_position;
}

class QRobot {
    Maze maze;
    std::pair<int, int> state;
    std::string action;

    double alpha;
    double gamma;
    double epsilon0;
    double epsilon;
    int t;

    std::vector<std::pair<std::pair<int, int>, std::string>> history;
    std::map<std::pair<int, int>, std::map<std::string, double>> q_table;

public:
    QRobot(const Maze& maze, double alpha = 0.5, double gamma = 0.8, double epsilon0 = 0.99);
    std::vector<std::string> current_state_valid_actions();
    void reset();
    double update_parameter();
    std::string sense_state();
    void create_Qtable_line(const std::pair<int, int>& state);
    void update_Qtable(double r, const std::string& action, const std::pair<int, int>& next_state);
    std::pair<std::string, double> train_update();
    std::pair<std::string, double> test_update();
    std::vector<std::pair<int, int>> get_path();
    std::pair<int, int> get_robot_position() const {
    return maze.get_robot_position();
}

};

QRobot::QRobot(const Maze& maze, double alpha, double gamma, double epsilon0)
    : maze(maze), alpha(alpha), gamma(gamma), epsilon0(epsilon0) {
    reset();
}

std::vector<std::string> QRobot::current_state_valid_actions() {
    return maze.can_move_actions(state);
}

void QRobot::reset() {
    maze.reset_robot();
    state = maze.get_robot_position();  // 更新为迷宫中机器人的当前位置
    create_Qtable_line(state);
    history.clear();
    t = 0;
    epsilon = epsilon0;
}

double QRobot::update_parameter() {
    t++;
    if (epsilon < 0.01) {
        epsilon = 0.01;
    } else {
        epsilon -= t * 0.1;
    }
    return epsilon;
}

std::string QRobot::sense_state() {
    return std::to_string(state.first) + "," + std::to_string(state.second);
}

void QRobot::create_Qtable_line(const std::pair<int, int>& state) {
    if (q_table.find(state) == q_table.end()) {
        q_table[state] = {
            {"u", 0.0},
            {"r", 0.0},
            {"d", 0.0},
            {"l", 0.0}
        };
    }
}

void QRobot::update_Qtable(double r, const std::string& action, const std::pair<int, int>& next_state) {
    double current_r = q_table[state][action];
    double update_r = r + gamma * std::max_element(q_table[next_state].begin(), q_table[next_state].end(),
        [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
            return a.second < b.second;
        })->second;
    q_table[state][action] += alpha * (update_r - current_r);
}

std::pair<std::string, double> QRobot::train_update() {
    state = maze.get_robot_position();  // 更新为迷宫中机器人的当前位置
    create_Qtable_line(state);

    std::string action;
    double reward;

    if (std::rand() / double(RAND_MAX) < epsilon) {
        std::vector<std::string> valid_actions = current_state_valid_actions();
        action = valid_actions[std::rand() % valid_actions.size()];
    } else {
        action = std::max_element(q_table[state].begin(), q_table[state].end(),
            [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
                return a.second < b.second;
            })->first;
    }

    reward = maze.move_robot(action);
    std::pair<int, int> next_state = maze.get_robot_position();  // 更新为迷宫中机器人的当前位置
    create_Qtable_line(next_state);
    update_Qtable(reward, action, next_state);

    history.push_back(std::make_pair(state, action));

    return std::make_pair(action, reward);
}

std::pair<std::string, double> QRobot::test_update() {
    state = maze.get_robot_position();  // 更新为迷宫中机器人的当前位置
    create_Qtable_line(state);

    std::string action = std::max_element(q_table[state].begin(), q_table[state].end(),
        [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
            return a.second < b.second;
        })->first;

    double reward = maze.move_robot(action);

    return std::make_pair(action, reward);
}

std::vector<std::pair<int, int>> QRobot::get_path() {
    std::vector<std::pair<int, int>> path;
    for (const auto& entry : history) {
        path.push_back(entry.first);
    }
    path.push_back(state);
    return path;
}
int maze2[500][500];
int size = 1000;
std::vector<std::vector<int>> input_maze() {

    std::cout<<"please input the size of maze"<<endl;
    cin>>size;
    std::vector<std::vector<int>> maze(size, std::vector<int>(size));
    std::cout << "Enter the maze (0 for open path, 1 for wall, 2 for goal, 3 for start):" << std::endl;

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            std::cin >> maze[i][j];
            maze2[i][j]=maze[i][j];
        }
    }

    return maze;
}

void print_path(const std::vector<std::pair<int, int>>& path) {
    std::cout << "Path: ";
    for (const auto& position : path) {
        std::cout << "(" << position.first << ", " << position.second << ") -> ";
    }
    std::cout << "Goal" << std::endl;
}
bool visited[500][500]={0};
int counter=0;
int dfs_path_y[50000];
int dfs_path_x[50000];

bool dfs(int x, int y) {
    visited[y][x] = true;
    dfs_path_y[++counter]=y;
    dfs_path_x[counter]=x;
    if (maze2[y][x] == 2) {
        return true;  // 找到了终点
    }

    std::vector<std::pair<int, int>> neighbors = {
        {x - 1, y},  // 左
        {x + 1, y},  // 右
        {x, y - 1},  // 上
        {x, y + 1}   // 下
    };

    for (const auto& neighbor : neighbors) {
        int nx = neighbor.first;
        int ny = neighbor.second;

        if (nx >= 0 && nx < size && ny >= 0 && ny < size && maze2[ny][nx] != 1 && !visited[ny][nx]) {
            if (dfs(nx, ny)) {
                return true;  // 找到了终点，返回true
            }
        }
    }
    counter--;
    visited[y][x]=0;
    return false;  // 没有找到终点，返回false
}

int main() {
    std::vector<std::vector<int>> maze_data = input_maze();
    Maze maze(maze_data);
    QRobot q_robot(maze);

    int max_episodes = 5000;
    int max_steps = 1000;
    std::pair<std::string, double> result;
    int train[10000];
    int cnt=0;
    for (int episode = 1; episode <= max_episodes; ++episode) {
        q_robot.reset();
       // std::cout << "Episode: " << episode << std::endl;

        for (int step = 1; step <= max_steps; ++step) {
            q_robot.update_parameter();
            result = q_robot.train_update();
            if (maze_data[q_robot.get_robot_position().second][q_robot.get_robot_position().first] == 2) {
                //std::cout << "Reached goal in " << step << " steps" << std::endl;
                train[++cnt]=step;
                break;
            }
            //cout<<"x "<<q_robot.get_robot_position().second<<" y "<<q_robot.get_robot_position().first<<endl;
            //system("pause");
        }

        if (maze_data[q_robot.get_robot_position().second][q_robot.get_robot_position().first] != 2) {
            //std::cout << "Could not reach goal within " << max_steps << " steps" << std::endl;
        }

        double epsilon = q_robot.update_parameter();
      //  std::cout << "Epsilon: " << epsilon << std::endl;
    }
    for(int i=4950;i<=cnt;i++){
        cout<<"For the "<<i<<" epochs the steps is "<<train[i]<<endl;
    }
    q_robot.reset();
    cout<<"(0,0)->";
    for (int step = 1; step <= max_steps; ++step) {
        result = q_robot.test_update();
        if (maze_data[q_robot.get_robot_position().second][q_robot.get_robot_position().first] == 2) {
            cout<<"("<<q_robot.get_robot_position().second<<","<<q_robot.get_robot_position().first<<")"<<endl;
            std::cout << "Reached goal in " << step << " steps" << std::endl;
            break;
        }
        cout<<"("<<q_robot.get_robot_position().second<<","<<q_robot.get_robot_position().first<<")->";
    }

    if (maze_data[q_robot.get_robot_position().second][q_robot.get_robot_position().first] != 2) {
        std::cout << "Could not reach goal within " << max_steps << " steps" << std::endl;
    }

    std::vector<std::pair<int, int>> path = q_robot.get_path();
    cout<<"Now there is the deep first search! "<<endl;
    dfs(0, 0);
    cout<<"the total search count is "<<counter<<endl;
    for(int i=1;i<counter;i++){
        cout<<"("<<dfs_path_y[i]<<","<<dfs_path_x[i]<<")"<<"->";
    }
    cout<<"("<<dfs_path_y[counter]<<","<<dfs_path_x[counter]<<")"<<endl;
    system("pause");
    return 0;
}


/*




3 0 1 1 1 1 1 1 1 1
1 0 0 0 1 0 1 0 0 1
1 0 1 0 1 0 1 0 1 1
1 0 1 0 0 0 1 0 0 1
1 0 1 1 1 0 1 1 0 1
1 0 0 0 1 0 0 0 0 1
1 1 1 0 1 1 1 1 0 1
1 1 1 0 1 1 1 0 0 1
1 1 1 0 0 0 0 0 1 1
1 1 1 1 1 1 1 0 0 2

3 0 1 1 1 1 1 1 1 1 1 1 1 1 1
1 0 0 0 1 0 1 0 0 1 0 0 0 0 1
1 0 1 0 1 0 1 0 1 1 0 1 1 1 1
1 0 1 0 0 0 1 0 0 1 0 1 0 0 1
1 0 1 1 1 0 1 1 0 1 0 1 0 1 1
1 0 0 0 1 0 0 0 0 1 0 1 0 0 1
1 1 1 0 1 1 1 1 0 1 0 1 0 1 1
1 1 1 0 1 1 1 0 0 1 0 0 0 0 1
1 1 1 0 0 0 0 0 1 1 1 1 1 0 1
1 1 1 1 1 1 1 0 0 0 0 0 0 0 2
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1 1 1 1 1 1 1 2

3 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
1 0 0 1 0 0 1 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 1
1 0 1 1 1 1 1 0 1 1 1 1 0 1 1 1 0 1 1 1 1 1 1 1 1 1 1 1 0 1
1 0 0 0 0 1 1 0 0 0 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1
1 0 1 1 0 1 1 0 1 1 1 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 1
1 0 1 0 0 0 0 0 0 0 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1
1 0 1 1 1 1 1 1 1 0 1 0 1 1 0 1 1 1 1 1 1 1 1 1 1 1 1 1 0 1
1 0 1 0 0 0 0 0 1 0 0 0 1 1 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
1 0 1 0 1 1 1 0 1 1 1 1 1 1 0 1 0 1 1 1 1 1 1 1 1 1 1 1 0 1
1 0 1 0 1 0 1 0 0 0 0 0 0 1 0 1 0 1 0 0 0 0 0 0 0 0 0 0 0 1
1 0 1 0 1 0 1 0 1 1 1 1 0 1 0 1 0 1 0 1 1 1 1 1 1 1 1 1 0 1
1 0 0 0 1 0 1 0 1 0 0 1 0 1 0 1 0 1 0 1 0 0 0 0 0 0 0 0 0 1
1 1 1 0 1 0 1 0 1 0 1 1 0 1 0 1 0 1 0 1 1 1 1 1 1 1 1 1 0 1
1 1 1 0 1 0 1 0 1 0 1 1 0 1 0 1 0 1 0 1 1 1 1 1 1 1 1 1 0 1
1 0 0 0 0 0 1 0 1 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
1 0 1 1 1 0 1 0 1 1 1 1 0 1 0 1 1 1 1 1 1 1 1 1 1 1 1 1 0 1
1 0 1 0 0 0 1 0 1 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1
1 0 1 1 1 1 1 0 1 1 1 1 0 1 1 1 0 1 1 1 1 1 1 1 1 1 1 1 0 1
1 0 1 0 0 0 0 0 0 0 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1
1 0 1 1 1 1 1 1 1 0 1 0 1 1 0 1 1 1 1 1 1 1 1 1 1 1 1 1 0 1
1 0 1 0 0 0 0 0 1 0 1 0 0 1 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
1 0 1 0 1 0 1 0 1 0 1 1 1 1 0 1 0 1 1 1 1 1 1 1 1 1 1 1 0 1
1 0 1 0 1 0 1 0 1 0 0 0 0 1 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 1
1 0 1 0 1 0 1 0 1 1 1 1 0 1 0 1 0 1 0 1 1 1 1 1 1 1 1 1 0 1
1 0 1 0 1 0 1 0 0 0 0 0 0 1 0 1 0 1 0 1 0 0 0 0 0 0 0 0 0 1
1 0 1 0 1 0 1 0 1 1 1 1 0 1 0 1 0 1 0 1 1 1 1 1 1 1 1 1 0 1
1 0 0 0 1 0 1 0 1 0 0 1 0 1 0 1 0 1 0 1 0 0 0 0 0 0 0 0 0 1
1 1 1 0 1 0 1 0 1 0 1 1 0 1 0 1 0 1 0 1 1 1 1 1 1 1 1 1 0 1
1 0 1 0 0 0 1 0 1 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1
1 0 1 1 1 0 1 0 1 1 1 1 0 1 0 1 1 1 1 1 1 1 1 1 1 1 1 1 0 2


3 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1
1 0 1 1 1 1 1 1 1 1 1 1 1 1 1 0 1 1 1 1
1 0 1 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 1
1 0 1 0 1 1 1 1 1 1 1 1 1 0 1 0 1 1 1 1
1 0 1 0 1 0 0 0 0 0 0 0 1 0 1 0 1 0 0 1
1 0 1 0 1 0 1 1 1 1 1 0 1 1 1 0 1 1 1 1
1 0 1 0 1 0 1 0 0 0 1 0 1 0 0 0 1 0 0 1
1 0 1 0 1 0 1 0 1 1 1 0 1 0 1 0 1 0 1 1
1 0 1 0 1 0 1 0 1 0 0 0 1 0 1 0 1 0 0 1
1 0 1 0 1 0 1 0 1 1 1 0 1 0 1 0 1 1 1 1
1 0 1 0 1 0 1 0 1 0 0 0 1 0 1 0 0 0 0 1
1 0 1 0 1 0 1 0 1 1 1 0 1 0 1 1 1 1 0 1
1 0 1 0 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 1
1 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 1
1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 1
1 0 1 0 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 1
1 0 1 0 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 1
1 0 1 0 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 2

*/