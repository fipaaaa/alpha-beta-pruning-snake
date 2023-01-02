#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <jsoncpp/json.h>
#include <cmath>
#include <ctime>

#define mp make_pair
#define x first
#define y second

using namespace std;

typedef pair<int,int> PII;

const int WIN = 100000000;
const int HASH_SIZE = 1 << 20;//1左移二十位

int n, m, R;
int obs[20][20], disteps[300], pow_9[200];
int dx[4] = {-1, 0, 1, 0}, dy[4] = {0, 1, 0, -1};
PII snake[2][300];
int NEXT_STEP, Time;
int hash_move[HASH_SIZE];

int get_length(int r)
{
    if (r <= 10) return r + 1;
    return (r - 10) / 3 + 11;
}

void print()
{
    for (int i = 1; i <= m; i ++ )
    {
        for (int j = 1; j <= n; j ++ )
            printf ("%d", obs[j][i]);
        puts ("");
    }
    puts("");
}

int sum, DEPTH;

void output(int d)
{
    Json::Value ret;
    ret["response"]["direction"] = d;
    ret["response"]["myinformatin"] = DEPTH;
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
    system("pause");
}

void input()
{
    string str, temp;
    while(getline(cin, temp)) str += temp;

    //str = string("{\"requests\":[{\"y\":1,\"x\":1,\"width\":11,\"obstacle\":[{\"y\":1,\"x\":5},{\"y\":11,\"x\":6},{\"y\":2,\"x\":7},{\"y\":10,\"x\":4},{\"y\":3,\"x\":8},{\"y\":9,\"x\":3},{\"y\":3,\"x\":9},{\"y\":9,\"x\":2},{\"y\":5,\"x\":1},{\"y\":7,\"x\":10}],\"height\":10},{\"direction\":3},{\"direction\":0},{\"direction\":3},{\"direction\":0},{\"direction\":0},{\"direction\":3},{\"direction\":3},{\"direction\":0},{\"direction\":0},{\"direction\":0},{\"direction\":0},{\"direction\":0},{\"direction\":0},{\"direction\":3},{\"direction\":2},{\"direction\":3},{\"direction\":3},{\"direction\":3},{\"direction\":2},{\"direction\":1},{\"direction\":1},{\"direction\":2},{\"direction\":1},{\"direction\":2},{\"direction\":3},{\"direction\":3},{\"direction\":0},{\"direction\":3},{\"direction\":2},{\"direction\":2},{\"direction\":3},{\"direction\":0},{\"direction\":0},{\"direction\":3},{\"direction\":0},{\"direction\":0},{\"direction\":0},{\"direction\":1},{\"direction\":2},{\"direction\":2},{\"direction\":1},{\"direction\":0},{\"direction\":0},{\"direction\":1},{\"direction\":2},{\"direction\":2},{\"direction\":1},{\"direction\":1}],\"responses\":[{\"direction\":1},{\"direction\":2},{\"direction\":2},{\"direction\":2},{\"direction\":1},{\"direction\":2},{\"direction\":1},{\"direction\":1},{\"direction\":1},{\"direction\":2},{\"direction\":3},{\"direction\":3},{\"direction\":3},{\"direction\":2},{\"direction\":1},{\"direction\":2},{\"direction\":2},{\"direction\":1},{\"direction\":0},{\"direction\":0},{\"direction\":1},{\"direction\":1},{\"direction\":0},{\"direction\":0},{\"direction\":1},{\"direction\":0},{\"direction\":1},{\"direction\":2},{\"direction\":2},{\"direction\":3},{\"direction\":2},{\"direction\":1},{\"direction\":1},{\"direction\":1},{\"direction\":2},{\"direction\":2},{\"direction\":2},{\"direction\":3},{\"direction\":3},{\"direction\":3},{\"direction\":0},{\"direction\":0},{\"direction\":3},{\"direction\":0},{\"direction\":3},{\"direction\":0},{\"direction\":0},{\"direction\":0}]}");

    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);

    n = input["requests"][(Json::Value::UInt)0]["height"].asInt();
    m = input["requests"][(Json::Value::UInt)0]["width"].asInt();

    for (int i = 1; i <= n; i ++ ) obs[i][0] = obs[i][m + 1] = 1;//画边界
    for (int i = 1; i <= m; i ++ ) obs[0][i] = obs[n + 1][i] = 1;

    int t = input["requests"][(Json::Value::UInt)0]["x"].asInt();

    snake[t != 1][0] = mp(1, 1);//用数组代替push//mp
    snake[t == 1][0] = mp(n, m);

    int obsCount = input["requests"][(Json::Value::UInt)0]["obstacle"].size();

    for (int i = 0, x, y; i < obsCount; i ++ )
    {
        x = input["requests"][(Json::Value::UInt)0]["obstacle"][i]["x"].asInt();
        y = input["requests"][(Json::Value::UInt)0]["obstacle"][i]["y"].asInt();
        obs[x][y] = 1;//障碍物所在位置赋值1
    }

    R = input["responses"].size();//我方走过的地方
    for (int i = 0, d; i < R; i ++ )//输入两边的蛇走过的地方，模拟至当前位置
    {
        d = input["responses"][(Json::Value::UInt)i]["direction"].asInt();
        snake[0][i + 1] = mp(snake[0][i].x + dx[d], snake[0][i].y + dy[d]);

        d = input["requests"][(Json::Value::UInt)(i + 1)]["direction"].asInt();
        snake[1][i + 1] = mp(snake[1][i].x + dx[d], snake[1][i].y + dy[d]);
    }

    for (int i = 0; i < 2; i ++ )//我方与对方
        for (int j = R - get_length(R) + 1; j <= R; j ++ )
            obs[snake[i][j].x][snake[i][j].y] = i + 2;//在obs数组里i---2代表我方，3代表对方,j----只存现有的蛇身

    for (int i = 0, j = 0; i < 300; i ++ )
        if (get_length(i) == get_length(i + 1))//不生长
            disteps[j ++ ] = i + 1;//把不生长的回合数计在distep表里

    pow_9[0] = 10000;
    for (int i = 1; i < 200; i ++ ) pow_9[i] = pow_9[i - 1] * 0.93;//不同距离对应的价值
    //for (int i = 0; i < 200; i ++ ) printf ("%d %d\n", i, pow_9[i]);
}

void move_tail()//去除尾部
{
    if (get_length(R) == get_length(R + 1))//长度不变
    {
        PII &r0 = snake[0][R + 1 - get_length(R + 1)], &r1 = snake[1][R + 1 - get_length(R + 1)];//r0,r1储存我方、对方地方目前位置
        obs[r0.x][r0.y] = obs[r1.x][r1.y] = 0;//该位置在地图上赋值为0，即未被占领
    }
}

void remove_tail()
{
    if (get_length(R) == get_length(R + 1))
    {
        PII &r0 = snake[0][R + 1 - get_length(R + 1)], &r1 = snake[1][R + 1 - get_length(R + 1)];
        obs[r0.x][r0.y] = 2, obs[r1.x][r1.y] = 3;
    }
}

void move(int u, int d)//u代表我方及对方只取值0/1，d存下一步的方向
{
    PII &t = snake[u][R + 1];
    t = mp(snake[u][R].x + dx[d], snake[u][R].y + dy[d]);
    obs[t.x][t.y] = u + 2;
    if (u == 1) R ++ ;
}

void remove(int u)//恢复
{
    if (u == 1) R -- ;
    PII &t = snake[u][R + 1];
    obs[t.x][t.y] = 0;
}

int special_work(int u)
{
    if (!u)
    {
        bool flag = 0;
        for (int i = 0; i < 4; i ++ )
        {
            PII t = mp(snake[1][R].x + dx[i], snake[1][R].y + dy[i]);
            if (!obs[t.x][t.y])
            {
                flag = 1;
                break;
            }
        }
        if (flag) return -WIN;
        return 0;
    }
    else
    {
        if (abs(snake[0][R + 1].x - snake[1][R].x) + abs(snake[0][R + 1].y - snake[1][R].y) == 1) return 10;
        return -WIN;
    }
}

void shortest_path(int st[20][20], int d[20][20], int u)
{
    static PII q[200];
    int hh = 0, tt = 0;

    q[0] = snake[u][R];
    d[q[0].x][q[0].y] = 0;

    while (hh <= tt)
    {
        PII &t = q[hh ++ ];
        for (int i = 0; i < 4; i ++ )
        {
            PII p = mp(t.x + dx[i], t.y + dy[i]);//四个方向
            if (d[p.x][p.y] > d[t.x][t.y] + 1 && obs[p.x][p.y] != 1 && st[p.x][p.y] <= d[t.x][t.y] + 1)
            {                                     //地图上
                d[p.x][p.y] = d[t.x][t.y] + 1;
                q[ ++ tt] = p;
            }
        }
    }
}

int evaluate()
{
    static int st[20][20], d0[20][20], d1[20][20], w[20][20];

    memset(st, 0, sizeof st);
    memset(d0, 0x70, sizeof d0);
    memset(d1, 0x70, sizeof d1);

    for (int i = R - get_length(R) + 1; i <= R; i ++ )
        for (int j = 0; j < 2; j ++ )
            st[snake[j][i].x][snake[j][i].y] = disteps[i] - R;//st[当前蛇尾的x值][当前蛇尾的y值]= 不生长的回合数-我方回合数

    shortest_path(st, d0, 0);
    shortest_path(st, d1, 1);

    int s0 = 0, s1 = 0;
    for (int i = 1; i <= n; i ++ )
        for (int j = 1; j <= m; j ++ )
        {
            if (d0[i][j] < d1[i][j])
                s0 += pow_9[d0[i][j]];
            else if (d0[i][j] > d1[i][j])
                s1 += pow_9[d1[i][j]];
        }

    //int score0 = min(s0[0], s0[1]) * 2 + (s0[0] > s0[1]);
    //int score1 = min(s1[0], s1[1]) * 2 + (s1[0] > s1[1]);
    return s0 - s1;
}

int alphabeta(int depth, int u, int alpha, int beta, bool isroot, int hash_value)
{
    if (sum ++ > 400)//全局的循环次数，作者应该实践过400次不超时，这是前期没办法递归到指定层数的结束方法
    {
        //cout << clock() - Time << endl;
        output(NEXT_STEP);
        exit(0);
    }

    int current = -WIN, bestmove = 0;
    bool flag = 1;
    if (depth <= 0) return evaluate();//若递归层数已经到达0（后期）

    int best = hash_move[hash_value & (HASH_SIZE - 1)];//HASH_SIZE的初始值超级大，hash_value初始值为0//&对两个int按位进行与运算

    if (!u) move_tail();//看看下一步对方的尾巴有没有动
    for (int i = best, j = 0; j < 4 ; i = (i + 1) & 3, j ++ )
    {
        PII t = mp(snake[u][R].x + dx[i], snake[u][R].y + dy[i]);
        if (obs[t.x][t.y]) continue;//被走过或障碍

        flag = 0;//代表可以走
        move(u, i);
        int score = -alphabeta(depth - 1, !u, -beta, -alpha, 0, hash_value * 20011 + i);
        remove(u);

        if (score > current)
        {
            bestmove = i;
            current = score;
            if (alpha < score)
            {
                alpha = score;
                if (isroot) NEXT_STEP = i;
            }
            if (score > beta) break;
        }
    }
    if (flag) current = special_work(u);
    if (!u) remove_tail();

    hash_move[hash_value & (HASH_SIZE - 1)] = bestmove;

    return current;
}

int MTDF(int test, int depth)//主要函数顺序--2
{
    int score = test, beta, l = -WIN, r = WIN;//score=-1000000000
    while (l < r)//相当于无限循环，利用时间暂停
    {
        beta = (score == l ? score + 1 : score);//beta=-1000000000
        score = alphabeta(depth, 0, beta - 1, beta, 1, 0);//第一次调用alphabeta
        (score < beta ? r : l) = score;
    }
    return score;
}

int strategy()//主要函数顺序--1
{
    //print();

    int test = -WIN;//-10000000000
    int depth = 6;//目前设为六层递归
    while (1)
    {
        test = MTDF(test, depth);
        //printf ("Depth: %d\n", depth);
        //printf ("Test: %d\n", test);
        //printf ("Next %d\n", NEXT_STEP);
        //getchar ();
        depth += 2 ;
        DEPTH = depth / 2;
    }

    return NEXT_STEP;
}

int main()
{
    Time = clock();
    input();
    output(strategy());
    return 0;
}