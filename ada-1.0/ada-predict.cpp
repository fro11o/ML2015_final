#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

const int MAXN = 100000;
const double eps = 1e-6;

vector<string> D;
vector<int> X[MAXN];
vector<int> Y;
vector<double> alpha;
vector<int> g_s;
vector<int> g_i;
vector<double> g_theta;

void read_data(string f_name) {
	ifstream f;
	f.open(f_name);
	string fstr;
	while(getline(f, fstr)) {
		D.push_back(fstr);
	}
	f.close();
}

void read_model(string f_name) {
	ifstream f;
	f.open(f_name);
	int N;
	f >> N;
	int s, dim;
	double a, theta;
	for(int i = 0; i < N; ++i) {
		f >> a >> s >> dim >> theta;
		alpha.push_back(a);
		g_s.push_back(s);
		g_i.push_back(dim);
		g_theta.push_back(theta);
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

void write_data(string s) {
	s += ".model";
	int sz = -1;
	for(int i = 0; i < s.size(); ++i) {
		if(s[i] == '/')
			sz = i;
	}
	if(sz != -1) {
		s = s.substr(sz + 1);
	}
	
	ofstream f_name(s);
	f_name << alpha.size() << endl;
	for(int i = 0; i < alpha.size(); ++i) {
		f_name << alpha[i] << ' ' << g_s[i] << ' ' << g_i[i] << ' ' << g_theta[i] << endl;
	}
	f_name.close();
}

void write_predict(string s) {
	ofstream f(s);
	int err = 0, tot = 0;
	for(int i = 0; i < D.size(); ++i) {
		double score = 0;
		for(int j = 0; j < alpha.size(); ++j) {
			if(g_s[j] * (X[i][g_i[j]] - g_theta[j]) > 0)
				score += alpha[j];
			else
				score -= alpha[j];
		}
		if(score > 0)  f << "1" << endl;
		else  f << "-1" << endl;

		++tot;
		if(score * Y[i] < 0)  ++err;
	}
	f.close();
	cout << "Accuracy: " << 1 - 1. * err / tot << ". (" << tot - err << "/" << tot << ")" << endl;
}


int main(int argc, char *argv[]) {
	if(argc != 4) {
        cout << "usage: ./a.out [test.dat] [model.dat] [output]" << endl;
		return 0;
	}

	read_data(argv[1]);
	read_model(argv[2]);

	parse_data();

	//print_data();	
	
	write_predict(argv[3]);

	return 0;
}

