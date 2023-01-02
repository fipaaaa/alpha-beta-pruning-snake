#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<list>
#include<string>
#include<ctime>
#include"jsoncpp/json.h"
#include<cmath>
using namespace std;
int n, m;
const int maxn = 25;
const int WIN = 100000000;
const int dx[4] = { -1,0,1,0 };
const int dy[4] = { 0,1,0,-1 };
bool invalid[maxn][maxn];
//int dist[maxn][maxn];
int w_pow[200]; int disteps[300];
int sum = 0, DEPTH;
//--------------------------------------------
const int HASH_SIZE = 1 << 20;//1宸︾Щ浜屽崄浣?
int hash_move[HASH_SIZE];
//------------------------------------------
int timeout = (int)((0.9689) * (double)CLOCKS_PER_SEC); //玄学参数 算力提升100%

int NEXT_STEP, Time;

struct point
{
	int x, y;
	point(int _x, int _y)
	{
		x = _x;
		y = _y;
	}
	point() {};
};



Json::Value ret;
void output(int d)
{

	ret["response"]["direction"] = d;

	ret["response"]["myinformatin"] = sum;




	Json::FastWriter writer;

	cout << writer.write(ret) << endl;
	system("pause");
}



















list<point> snake[2]; // 0琛ㄧず鑷繁鐨勮泧锛?琛ㄧず瀵规柟鐨勮泧
int possibleDire[10];
int posCount;


bool whetherGrow(int num)  //鏈洖鍚堟槸鍚︾敓闀?
{
	if (num <= 9) return true;
	if ((num - 9) % 3 == 0) return true;
	return false;
}

int get_length(int total)//褰撳墠铔囩殑闀垮害
{
	if (total <= 10) return total + 1;
	return (total - 10) / 3 + 11;
}


void deleteEnd(int id)     //鍒犻櫎铔囧熬
{
	snake[id].pop_back();
}


/*
void move_taill(total){

	if (get_length(total) == get_length(total + 1))//濡傛灉闀垮害涓嶅彉锛屼絾鍙堝悜鍓嶈蛋浜?
	{



		PII &r0 = snake[0][R + 1 - get_length(R + 1)], &r1 = snake[1][R + 1 - get_length(R + 1)];
		obs[r0.x][r0.y] = obs[r1.x][r1.y] = 0;//璧嬩负绌哄湴0


	}



}
*/





void move(int id, int dire, int num)  //编号为id的蛇朝向dire方向移动一步
{
	point p = *(snake[id].begin());
	int x = p.x + dx[dire];
	int y = p.y + dy[dire];
	snake[id].push_front(point(x, y));
	if (!whetherGrow(num))
		deleteEnd(id);
}

int move1(int id, int dire, int total) {
	point p = *(snake[id].begin());
	int x = p.x + dx[dire];
	int y = p.y + dy[dire];
	snake[id].push_front(point(x, y));
	if (id == 1) {
		total++;
	}
	return total;
}
void move_tail(int total) {
	if (get_length(total) == get_length(total + 1)) {
		snake[0].pop_back();
		snake[1].pop_back();
	}//长度不变
}
void remove_tail(int total, point p0, point p1) {
	if (get_length(total) == get_length(total + 1))
	{
		snake[0].push_back(p0);
		snake[1].push_back(p1);
	}
}

int remove1(int id, int num) {
	if (id == 1) {
		num--;
	}
	snake[id].pop_front();
	return num;
}
void outputSnakeBody(int id)    //璋冭瘯璇彞
{
	cout << "Snake No." << id << endl;
	for (list<point>::iterator iter = snake[id].begin(); iter != snake[id].end(); ++iter)
		cout << iter->x << " " << iter->y << endl;
	cout << endl;
}

bool isInBody(int x, int y)   //鍒ゆ柇(x,y)浣嶇疆鏄惁鏈夎泧
{
	for (int id = 0; id <= 1; id++)
		for (list<point>::iterator iter = snake[id].begin(); iter != snake[id].end(); ++iter)
			if (x == iter->x && y == iter->y)
				return true;
	return false;
}

bool validDirection(int id, int k)  //鍒ゆ柇褰撳墠绉诲姩鏂瑰悜鐨勪笅涓€鏍兼槸鍚﹀悎娉?
{
	point p = *(snake[id].begin());
	int x = p.x + dx[k];
	int y = p.y + dy[k];
	if (x > n || y > m || x < 1 || y < 1) return false;
	if (invalid[x][y]) return false;
	if (isInBody(x, y)) return false;
	return true;
}

int Rand(int p)   //闅忔満鐢熸垚涓€涓?鍒皃鐨勬暟瀛?
{
	return rand() * rand() * rand() % p;
}

void general_map(int map[25][25])//鏍规嵁褰撳墠褰㈠娍鐢熸垚闅滅鐗╁湴鍥撅紙瀵规柟铔囦綋锛屾垜鏂硅泧浣擄紝闅滅鐗╁潎涓洪殰纰嶏級,n妫嬬洏楂樺害 m涓哄綋鍓嶆鐩樼殑瀹藉害
{
	for (int i = 0; i <= n + 1; i++)//-1鍒濆鍖?
		for (int j = 0; j <= m + 1; j++)
			map[i][j] = 1;

	//************杈圭晫*************
	for (int i = 1; i <= n; i++)
		for (int j = 1; j <= m; j++)
			map[i][j] = invalid[i][j];//闅滅涓?
	//************澶嶅埗闅滅鐗?*********
	list<point>::iterator it = snake[0].begin();
	list<point>::iterator it1 = snake[1].begin();
	for (it; it != snake[0].end(); it++)
	{
		map[it->x][it->y] = 2;//鎴戞柟璧拌繃涓?锛?

	}

	for (it1; it1 != snake[1].end(); it1++)
	{
		map[it1->x][it1->y] = 3;//瀵规柟璧拌繃涓?
	}
	//*****************澶嶅埗铔囪韩***************
}

bool canmove(int id) {
	bool flag;
	int i;
	for (i = 0; i < 4; i++) {
		if (validDirection(id, i)) {
			return true;
		}
	}
}
void shortest(int st[][25], int dist[][25], int temp_map[][25], int id)
{
	point my_head(snake[id].front());

	int visit[25][25] = { 0 };

	visit[my_head.x][my_head.y] = 1;
	dist[snake[id].front().x][snake[id].front().y] = 0;
	deque<point> now_try;
	now_try.push_back(my_head);



	while (!now_try.empty())
	{
		point new_p(*now_try.begin());
		now_try.pop_front();
		for (int d = 0; d < 4; d++)
		{
			point p1;
			p1.x = new_p.x + dx[d];
			p1.y = new_p.y + dy[d];
			//向某方向走一步，now_p存新方向的点
			if (dist[p1.x][p1.y] > dist[new_p.x][new_p.y] + 1 && temp_map[p1.x][p1.y] != 1 && st[p1.x][p1.y] <= dist[new_p.x][new_p.y] + 1)//新的待入队节点合法
			{
				now_try.push_back(p1);
				dist[p1.x][p1.y] = dist[new_p.x][new_p.y] + 1;
				//visit[p1.x][p1.y] = 1;
			}
		}

	}
};

int evaluate(int total)
{
	int dist1[25][25] = { 0 }, dist2[25][25] = { 0 }, temp_map[25][25];
	int st[25][25] = { 0 };
	memset(st, 0, sizeof st);
	memset(dist1, 0x70, sizeof dist1);
	memset(dist2, 0x70, sizeof dist2);

	//***************************************
	list<point>::iterator it0 = snake[0].end();
	list<point>::iterator it1 = snake[1].end();
	it0--;
	it1--;
	int k = 0;
	for (k = total - snake[0].size() + 1; k <= total; k++)
	{

		st[it0->x][it0->y] = disteps[k] - total;
		st[it1->x][it1->y] = disteps[k] - total;
		if (it0 != snake[0].begin())
			it0--;
		if (it1 != snake[1].begin())
			it1--;

	}

	/*cout << endl <<"St::"<< endl;

	for (int i = 1; i <= n + 1; i++)
		for (int j = 1; j <= m + 1; j++)
		{
			cout << st[i][j] << " ";
			if (j == m + 1)
				cout << endl;
		}
	cout << endl;
	*/
	general_map(temp_map);
	//***************************************
/*	cout << "Map:" << endl;
	for (int i = 0; i <= n + 1; i++)
		for (int j = 0; j <= m + 1; j++)
		{

				cout << temp_map[i][j] << " ";
			if (j == m + 1)
				cout << endl;
		}
	cout << endl;*/
	//***************************************
	shortest(st, dist1, temp_map, 0);
	shortest(st, dist2, temp_map, 1);

	/*	cout << snake[0].front().x << ',' << snake[0].front().y<<endl;
		cout << "my_dist1[]:" << endl;
		for (int i = 0; i <= n + 1; i++)
			for (int j = 0; j <= m + 1; j++)
			{
				if (dist1[i][j] == 1886417008)
					cout << "aa" << ' ';
				else if (dist1[i][j] < 10)
					cout << ' ' << dist1[i][j] << ' ';
				else
					cout << dist1[i][j] << " ";
				if (j == m + 1)
					cout << endl;
			}
		cout << endl;
		cout << snake[1].front().x << ',' << snake[1].front().y << endl;
		cout<<"op_dist2[]:"<<endl;
		for (int i = 0; i <= n + 1; i++)
			for (int j = 0; j <= m + 1; j++)
			{
				if (dist2[i][j] == 1886417008)
					cout << "aa" << ' ';
				else if (dist2[i][j] < 10)
					cout << ' ' << dist2[i][j] << ' ';
				else
					cout << dist2[i][j] << " ";
				if (j == m + 1)
					cout << endl;
			}

			*/

	int value1 = 0, value2 = 0;
	for (int i = 1; i <= n; i++)
		for (int j = 1; j <= m; j++)
		{
			if (dist1[i][j] < dist2[i][j])
				value1 += w_pow[dist1[i][j]];
			else if (dist2[i][j] < dist1[i][j])
				value2 += w_pow[dist2[i][j]];

		}
	return value1 - value2;




};

int noplace(int id) {
	if (id == 0) {
		if (canmove(1)) {
			return -WIN;
		}
		return 0;//鍙屾柟閮戒笉鑳借蛋浜?
	}
	else if (id == 1) {
		point p0 = *(snake[0].begin());
		point p1 = *(snake[1].begin());
		int snake0x = p0.x;
		int snake0y = p0.y;
		int snake1x = p1.x;
		int snake1y = p1.y;
		if (abs(snake0x - snake1x) + abs(snake0y - snake1y) == 1) {//鎴戞柟璧簡鍚э紵锛燂紵
			return 10;
		}
		return -WIN;
	}
	//return -WIN;
}
int returnvalue;
int alpha_beta(int depth, int id, int alpha, int beta, int total, bool firstpoint, int hash_value) {//firstpoint闄や簡绗竴灞備负1锛屽叾浠栧眰鍧囦负0
	ret["response"]["total"] = total;

	if((clock() - Time)>=timeout){
        
        output(returnvalue);
        exit(0);

    }

	int now = -1000000000, bestmove = 0;
	int i, j;
	bool isdeletetail = 0;
	if (depth == 0) {
		return evaluate(total);
	}
	//----------------------------------------------
	int best = hash_move[hash_value & (HASH_SIZE - 1)];

	/////

//if(!id) move_taill(total);





	point p0, p1;
	if (!whetherGrow(total)) {//对方尾部是否消失
		p0 = snake[0].back();
		p1 = snake[1].back();//保存尾部
	}

	int map[25][25];
	general_map(map);
	bool endflag = 1;//濡傛灉鏈€鍚庤繕鏄竴锛屼唬琛ㄦ棤璺彲璧帮紝灏辩粨鏉?
	if (!id) {
		move_tail(total);
	}
	for (int i = best, j = 0; j < 4; i = (i + 1) & 3, j++)
	{
		point p = *(snake[id].begin());//璇诲彇鏈€杩戠殑浣嶇疆

		int snakex = p.x + dx[i];
		int snakey = p.y + dy[i];
		if (map[snakex][snakey] != 0) continue;//闅滅
		total = move1(id, i, total);//绉诲姩
		int idnext = (id == 1 ? 0 : 1);
		endflag = 0;//杩樻湁璺彲浠ヨ蛋
		int score = -alpha_beta(depth - 1, idnext, -beta, -alpha, total, 0, hash_value * 20011 + i);//閫掑綊
		ret["response"]["score"] = score;
		total = remove1(id, total);//鎭㈠绉诲姩
		if (score > now) {
			bestmove = i;
			now = score;
			ret["response"]["now"] = now;
			if (alpha < score) {//鍓灊
				alpha = score;
				if (firstpoint == 1) {
					returnvalue = i;
				}
			}
			if (score > beta) {//鍓灊
				break;
			}
		}
	}
	if (endflag) {
		now = noplace(id);//鏃犺矾鍙蛋澶勭悊
	}
	////

	if (!id) {
		remove_tail(total, p0, p1);
	}

	hash_move[hash_value & (HASH_SIZE - 1)] = bestmove;//hash
	return now;
}


int mtd(int alpha, int beta, int depth, int test, int total) {
	int best_value;
	do {
		// 杩涜闆跺绐楀彛璇曟帰
		best_value = alpha_beta(depth, 0, test - 1, test, total, 1, 0);
		// 濡傛灉鏄痑lpha鑺傜偣
		if (best_value < test) {
			// 鏇存柊浼板€间笂闄愶紝骞跺皢姝ゅ仛涓烘柊鐨勮瘯鎺㈠€?
			test = beta = best_value;
			// 鍚﹀垯鏄痓eta鑺傜偣
		}
		else {
			// 鏇存柊浼板€间笅闄?
			alpha = best_value;
			// 鏂扮殑璇曟帰鍊?
			test = best_value + 1;
		}
	} while (alpha < beta);
	return best_value;
}



int strategy(int total)
{
	//print();

	int test = -WIN;
	int depth = 6;
	while (1)
	{
		//test = MTDF(test, ,depth);
		test = mtd(-WIN, WIN, depth, -WIN, total);
		//printf ("Depth: %d\n", depth);
		//printf ("Test: %d\n", test);
		//printf ("Next %d\n", NEXT_STEP);
		//getchar ();
		depth += 2;
		DEPTH = depth / 2;
	}

	return NEXT_STEP;
}






int main()
{
	memset(invalid, 0, sizeof(invalid));
	string str;
	string temp;
	while (getline(cin, temp))
		str += temp;

	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);

	n = input["requests"][(Json::Value::UInt) 0]["height"].asInt();  //妫嬬洏楂樺害
	m = input["requests"][(Json::Value::UInt) 0]["width"].asInt();   //妫嬬洏瀹藉害

	int x = input["requests"][(Json::Value::UInt) 0]["x"].asInt();  //璇昏泧鍒濆鍖栫殑淇℃伅
	if (x == 1)
	{
		snake[0].push_front(point(1, 1));
		snake[1].push_front(point(n, m));
	}
	else
	{
		snake[1].push_front(point(1, 1));
		snake[0].push_front(point(n, m));
	}
	//澶勭悊鍦板浘涓殑闅滅鐗?
	int obsCount = input["requests"][(Json::Value::UInt) 0]["obstacle"].size();

	for (int i = 0; i < obsCount; i++)
	{
		int ox = input["requests"][(Json::Value::UInt) 0]["obstacle"][(Json::Value::UInt) i]["x"].asInt();
		int oy = input["requests"][(Json::Value::UInt) 0]["obstacle"][(Json::Value::UInt) i]["y"].asInt();
		invalid[ox][oy] = 1;
	}

	//鏍规嵁鍘嗗彶淇℃伅鎭㈠鐜板満
	int total = input["responses"].size();

	int dire;
	for (int i = 0; i < total; i++)
	{
		dire = input["responses"][i]["direction"].asInt();
		move(0, dire, i);

		dire = input["requests"][i + 1]["direction"].asInt();
		move(1, dire, i);
	}

	if (!whetherGrow(total)) // 鏈洖鍚堜袱鏉¤泧鐢熼暱
	{
		deleteEnd(0);
		deleteEnd(1);
	}

	srand((unsigned)time(0) + total);
	w_pow[0] = 10000;
	for (int i = 1; i < 200; i++)
		w_pow[i] = w_pow[i - 1] * 0.93;

	for (int i = 0, j = 0; i < 300; i++)
		if (get_length(i) == get_length(i + 1))
			disteps[j++] = i + 1;
	Time=clock();
	output(strategy(total));

	return 0;
}