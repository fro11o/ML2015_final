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
const double MAX_FEATURE_NUM = 1e7;

vector<string> D;
vector<double> X[MAXN];
vector<int> Y;
vector<double> U;
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
			X[i].push_back(stod(sub));
		}
	}
}

void u_init() {
	for(int i = 0; i < D.size(); ++i) {
		U.push_back(1);
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


int main(int argc, char *argv[]) {
	if(argc != 3) {
        cout << "usage: ./a.out [train.dat] [# of iter]" << endl;
		return 0;
	}

	read_data(argv[1]);

	parse_data();

	//print_data();	

	u_init();
	for(int t = 0; t < stoi(argv[2]); ++t) {
		int t_s, t_i;
		double t_score = 1e9, t_theta;
		for(int i = 0; i < X[0].size(); ++i) {
			vector<pair<double, double> > v;
			for(int j = 0; j < D.size(); ++j) {
				v.push_back(make_pair(X[j][i], Y[j] * U[j]));
			}
			sort(v.begin(), v.end());
			vector<pair<double, double> > pos_sum, neg_sum;
			pos_sum.push_back(make_pair(-MAX_FEATURE_NUM, 0));
			neg_sum.push_back(make_pair(-MAX_FEATURE_NUM, 0));
			for(int j = 0; j < v.size(); ++j) {
				double pos = pos_sum[pos_sum.size() - 1].second;
				double neg = neg_sum[neg_sum.size() - 1].second;
				double loc = v[j].first;
				int idx = j;
				while(j < v.size() && fabs(v[j].first - v[idx].first) < eps) {
					if(v[j].second > 0) {
						pos += v[j].second;
					} else {
						neg += v[j].second;
					}
					++j;
				}
				--j;
				if(j == v.size() - 1) {
					pos_sum.push_back(make_pair(MAX_FEATURE_NUM, pos));
					neg_sum.push_back(make_pair(MAX_FEATURE_NUM, neg));
				} else {
					pos_sum.push_back(make_pair((v[idx].first + v[j + 1].first) / 2, pos));
					neg_sum.push_back(make_pair((v[idx].first + v[j + 1].first) / 2, neg));
				}
			}
			for(int j = 0; j < pos_sum.size(); ++j) {
				double score;
				//if sign == +1
				score = pos_sum[j].second - (neg_sum[neg_sum.size() - 1].second - neg_sum[j].second);
				if(score < t_score) {
					t_s = 1;
					t_i = i;
					t_theta = pos_sum[j].first;
					t_score = score;
				}
				//if sign == -1
				score = (pos_sum[pos_sum.size() - 1].second - pos_sum[j].second) - neg_sum[j].second;
				if(score < t_score) {
					t_s = -1;
					t_i = i;
					t_theta = pos_sum[j].first;
					t_score = score;
				}
			}
		}
		double total_u = 0;
		for(int i = 0; i < D.size(); ++i) {
			total_u += U[i];
		}
		double et = t_score / total_u;
		double diamond = sqrt((1 - et) / et);
		for(int i = 0; i < D.size(); ++i) {
			if(Y[i] * t_s * (X[i][t_i] - t_theta) > 0) {
				U[i] /= diamond;
			} else {
				U[i] *= diamond;
			}
		}
		alpha.push_back(log(diamond));
		g_s.push_back(t_s);
		g_i.push_back(t_i);
		g_theta.push_back(t_theta);
	}

	write_data(argv[1]);

	return 0;
}
