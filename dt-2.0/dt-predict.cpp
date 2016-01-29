#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
using namespace std;

const int MAXN = 100000;

vector<string> D;
vector<int> X[MAXN];
vector<int> Y;

void read_data(string f_name) {
	ifstream f;
	f.open(f_name);
	string fstr;
	while(getline(f, fstr)) {
		D.push_back(fstr);
	}
	f.close();
}

void parse_data() {
	for(int i = 0; i < D.size(); ++i) {
		stringstream ss(D[i]);
		vector<string> v;
		string tmp;
		while(ss >> tmp) {
			v.push_back(tmp);
		}
		//cout << D[i] << endl;
		Y.push_back(stoi(v[0]));
		for(int j = 1; j < v.size(); ++j) {
			int idx = 0;
			while(v[j][idx] != ':') {
				++idx;
			}
			string sub = v[j].substr(idx + 1);
			X[i].push_back(stoi(sub));
		}
	}
}

void print_data() {
	cout << "# of data: " << Y.size() << endl;
	cout << "Dimension: " << X[0].size() << endl;
	for(int i = 0; i < Y.size(); ++i) {
		cout << Y[i] << '!';
		for(int j = 0; j < X[i].size(); ++j) {
			cout << X[i][j] << '@';
		}
		cout << endl;
	}
}



struct node {
	int s, i, ch[2], ans;
	double theta;
}typedef Node;

vector<Node> Tree[15000];
int T_ks;

void read_model(string s) {
	ifstream f(s);
	int n, dim;
	T_ks = 0;
	while(f >> n) {
		for(int i = 0; i < n; ++i) {
			Node no;
			f >> no.s;
			f >> no.i;
			f >> no.theta >> no.ans >> no.ch[0] >> no.ch[1];
			Tree[T_ks].push_back(no);
		}
		++T_ks;
	}
}

int predict(int tree, int idx, int x) {
	if(Tree[tree][x].ch[0] == -1) {
		return Tree[tree][x].ans;
	}
	int ns = Tree[tree][x].s;
	int ni = Tree[tree][x].i;
	int ntheta = Tree[tree][x].theta;
	if(ns * (X[idx][ni] - ntheta) > 0) {
		return predict(tree, idx, Tree[tree][x].ch[1]);
	} else {
		return predict(tree, idx, Tree[tree][x].ch[0]);
	}
}

void solve(string s) {
	ofstream f(s);
	int acc = 0;
	for(int i = 0; i < D.size(); ++i) {
		int cnt = 0;
		for(int j = 0; j < T_ks; ++j) {
			cnt += predict(j, i, Tree[j].size() - 1);
		}
		if(cnt > 0) {
			f << "+1" << endl;
		} else {
			f << "-1" << endl;
		}
		if(cnt * Y[i] > 0)
			++acc;
	}
	cout << "Accuracy " << 1. * acc / D.size()  << " (" << acc << "/" << D.size() << ")" << endl;
	f.close();
}


int main(int argc, char *argv[]) {
	if(argc != 4) {
        cout << "usage: ./a.out [test.dat] [model_file] [output]" << endl;
		return 0;
	}

	read_data(argv[1]);

	parse_data();

	//print_data();	
	
	read_model(argv[2]);

	solve(argv[3]);

	return 0;
}
