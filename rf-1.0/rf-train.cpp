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
	int s, ch[2], ans;
	double theta;
	vector<int> ph;
}typedef Node;

vector<Node> v_node;
vector<pair<int, pair<int, int> > > x_y_idx;

int get_z(int idx, vector<int> &v) {
	int ret = 0;
	for(int i = 0; i < v.size(); ++i) {
		ret += X[idx][v[i]];
	}
	return ret;
}

int dt_dfs(int L, int R, int phi_dim) {
	Node cur_node;
	double t_impurity = 1e9;
	int leaf_check;
	for(int i = 0; i < phi_dim; ++i) {
		cur_node.ph.push_back(rand() % X[0].size());
	}
	//for(int i = 0; i < X[0].size(); ++i) {
		for(int j = L; j < R; ++j) {
			int idx = (x_y_idx[j].second).second;

			x_y_idx[j].first = 0;
			for(int k = 0; k < phi_dim; ++k) {
				x_y_idx[j].first += X[idx][k];
			}
		}
		sort(x_y_idx.begin() + L, x_y_idx.begin() + R);
		vector<pair<double, int> > pos_cnt, pos_tot;
		vector<int> leaf_flag;
		pos_cnt.push_back(make_pair(-1e7, 0));
		pos_tot.push_back(make_pair(-1e7, 0));
		leaf_flag.push_back(L);
		for(int j = L; j < R; ++j) {
			int cnt = pos_cnt[pos_cnt.size() - 1].second;
			int tot = pos_tot[pos_tot.size() - 1].second;
			int idx = j;
			while(j < R && x_y_idx[j].first == x_y_idx[idx].first) {
				if((x_y_idx[j].second).first > 0) {
					cnt += 1;
				}
				tot += 1;
				++j;
			}
			--j;
			if(j == R - 1) {
				pos_cnt.push_back(make_pair(1e7, cnt));
				pos_tot.push_back(make_pair(1e7, tot));
			} else {
				pos_cnt.push_back(make_pair(1. * (x_y_idx[idx].first + x_y_idx[j + 1].first) /  2, cnt));
				pos_tot.push_back(make_pair(1. * (x_y_idx[idx].first + x_y_idx[j + 1].first) /  2, tot));
			}
			leaf_flag.push_back(j + 1);
		}

		//cout << "pos_cnt.size() " << pos_cnt.size() << endl;
		for(int j = 1; j < pos_cnt.size(); ++j) {  //ignore 0 for avoid devide by 0
			double gini[2];
			gini[0] = 1 - pow(1. * pos_cnt[j].second / pos_tot[j].second, 2) - pow(1. * (pos_tot[j].second - pos_cnt[j].second) / pos_tot[j].second, 2);
			gini[1] = 1 - pow(1. * (pos_cnt[pos_cnt.size() - 1].second - pos_cnt[j].second) / (pos_tot[pos_tot.size() - 1].second - pos_tot[j].second), 2) - pow(1. * (pos_tot[pos_tot.size() - 1].second - pos_tot[j].second - pos_cnt[pos_cnt.size() - 1].second + pos_cnt[j].second) / (pos_tot[pos_tot.size() - 1].second - pos_tot[j].second), 2);
			double impurity = 0;
			if(pos_tot[j].second > 0) {
				impurity += pos_tot[j].second * gini[0];
			}
			if(pos_tot[pos_tot.size() - 1].second - pos_tot[j].second > 0) {
				impurity += (pos_tot[pos_tot.size() - 1].second - pos_tot[j].second) * gini[1];
			}
			//cout << "impurity " << impurity << endl;
			if(impurity < t_impurity) {
				cur_node.s = 1;
				cur_node.theta = pos_cnt[j].first;
				leaf_check = leaf_flag[j];
				t_impurity = impurity;
			}
			//if sign == +1
			/*
			impurity = 1 - pow(1. * (pos_tot[j].second - pos_cnt[j].second) / (R - L), 2) - pow(1. * (pos_cnt[pos_cnt.size() - 1].second - pos_cnt[j].second) / (R - L), 2);
			cout << "impurity " << impurity << endl;
			if(impurity < t_impurity) {
				cur_node.s = 1;
				//cur_node.i = i;
				cur_node.theta = pos_cnt[j].first;
				leaf_check = leaf_flag[j];
				t_impurity = impurity;
			}
			*/
			//if sign == -1
			/*
			impurity = 1 - pow(1. * (pos_cnt[j].second) / (R - L), 2) - pow(1. * (pos_tot[pos_tot.size() - 1].second - pos_tot[j].second - pos_cnt[pos_cnt.size() - 1].second + pos_cnt[j].second) / (R - L), 2);
			cout << "impurity " << impurity << endl;
			if(impurity < t_impurity) {
				cur_node.s = -1;
				//cur_node.i = i;
				cur_node.theta = pos_cnt[j].first;
				leaf_check = leaf_flag[j];
				t_impurity = impurity;
			}
			*/
		}
		//cout << "t_impurity " << t_impurity << endl;
	//}

	//cout << "L R leaf_check " << L << ' ' << R << ' ' << leaf_check << endl;
	if(leaf_check == L || leaf_check == R) {
		cur_node.ch[0] = cur_node.ch[1] = -1;
		cur_node.ans = 0;
		for(int i = L; i < R; ++i) {
			cur_node.ans += (x_y_idx[i].second).first;
		}
		if(cur_node.ans > 0) {
			cur_node.ans = 1;
		} else {
			cur_node.ans = -1;
		}
	} else {
		//if output of h_s_i_theta positive go to ch[1], negative go to ch[0]
		if(cur_node.s == -1) {
			cur_node.ch[1] = dt_dfs(L, leaf_check, phi_dim);
			cur_node.ch[0] = dt_dfs(leaf_check, R, phi_dim);
		} else {
			cur_node.ch[0] = dt_dfs(L, leaf_check, phi_dim);
			cur_node.ch[1] = dt_dfs(leaf_check, R, phi_dim);
		}
	}
	v_node.push_back(cur_node);
	return v_node.size() - 1;
}

void write_model(string s) {
	s += ".model";
	int sz = -1;
	for(int i = 0; i < s.size(); ++i) {
		if(s[i] == '/')
			sz = i;
	}
	if(sz != -1) {
		s = s.substr(sz + 1);
	}
	
	ofstream f_name(s, fstream::app);
	f_name << v_node.size() << ' ' << v_node[0].ph.size() << endl;
	for(int i = 0; i < v_node.size(); ++i) {
		f_name << v_node[i].s;
		for(int j = 0; j < v_node[i].ph.size(); ++j) {
			f_name << ' ' << v_node[i].ph[j];
		}
		f_name << ' ' << v_node[i].theta << ' ' << v_node[i].ans
			   << ' ' << v_node[i].ch[0] << ' ' << v_node[i].ch[1] << endl;
	}
	f_name.close();
}

int main(int argc, char *argv[]) {
	if(argc != 4) {
        cout << "usage: ./a.out [train.dat] [phi_dim] [# of iter]" << endl;
		return 0;
	}

	srand(time(NULL));

	read_data(argv[1]);

	parse_data();

	//print_data();	
	for(int i = 0; i < D.size() / 2; ++i) {
		x_y_idx.push_back(make_pair(0, make_pair(0, 0)));  // 0 is a no-means number
	}

	srand(time(NULL));
	for(int t = 0; t < stoi(argv[3]); ++t) {
		for(int i = 0; i < D.size() / 2; ++i) {
			int dice = rand() % D.size();
			x_y_idx[i] = make_pair(0, make_pair(Y[dice], dice));
		}
		dt_dfs(0, D.size() / 2, stoi(argv[2]));
		write_model(argv[1]);
		v_node.clear();
	}

	return 0;
}
