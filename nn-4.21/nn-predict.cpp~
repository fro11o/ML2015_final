#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <random>
using namespace std;

const int MAXN = 100000;
const int MAXL = 10;
const int MAXD = 100;
const double eps = 1e-9;

vector<string> D;
vector<int> X[MAXN];
vector<int> Y;
vector<int> DIM;
double NN_X[MAXL][MAXD];
double DELTA[MAXL][MAXD];
double W[MAXL][MAXD][MAXD];
vector<double> SCALE;
vector<double> MIN;

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

double tanh(double s) {
	if(s > 0) {
		return (1 - exp(-2*s)) / (1 + exp(-2*s));
	} else {
		return (exp(2*s) - 1) / (exp(2*s) + 1);
	}
}

double tanhh(double s) {
	if(s > 0) {
		return 4.0 / pow(1 + exp(-2*s), 2) * exp(-2*s);
	} else {
		return 4.0 / pow(exp(2*s) + 1, 2) * exp(2*s);
	}
}

void read_model(string s) {
	ifstream f(s);
	int n;
	f >> n;
	for(int i = 0; i < n; ++i) {
		double tmp; f >> tmp;
		SCALE.push_back(tmp);
	}
	for(int i = 0; i < n; ++i) {
		double tmp; f >> tmp;
		MIN.push_back(tmp);
	}
	f >> n;
	for(int i = 0; i < n; ++i) {
		int tmp; f >> tmp;
		DIM.push_back(tmp);
	}
	for(int i = 1; i < DIM.size(); ++i) {
		for(int j = 0; j < DIM[i - 1]; ++j) {
			for(int k = 1; k < DIM[i]; ++k) {
				f >> W[i][j][k];
				//cout << W[i][j][k] << endl;
			}
		}
	}
	f.close();
}

void solve(string s) {
	ofstream f(s);

	int acc = 0;
	for(int i = 0; i < D.size(); ++i) {
		memset(NN_X, 0, sizeof(NN_X));

		for(int j = 1; j < DIM[0]; ++j) {
			NN_X[0][j] = 1.0 * X[i][j - 1] - MIN[j - 1];
			if(SCALE[j - 1] > eps) {
				NN_X[0][j] /= SCALE[j - 1];
			}
		}
		for(int j = 1; j < DIM.size(); ++j) {  //j for layer
			NN_X[j - 1][0] = 1.0;
			for(int k = 0; k < DIM[j - 1]; ++k) {
				for(int l = 1; l < DIM[j]; ++l) {
					//cout << NN_X[j - 1][k] << ' ' << tanh(NN_X[j - 1][k]) << endl;
					if(j == 1) {
						NN_X[j][l] += W[j][k][l] * NN_X[j - 1][k];
					} else {
						NN_X[j][l] += W[j][k][l] * tanh(NN_X[j - 1][k]);
					}
				}
				//cout << "j-1 k " << j - 1 << ' ' << k << ' ' << NN_X[j - 1][k] << endl;
			}
		}

		//---nn-2.1 write new data begin---
		cout << Y[i];
		for(int j = 1; j < DIM[DIM.size() - 2]; ++j) {
			cout << " " << j << ":" << NN_X[DIM.size() - 2][j];
		}
		cout << endl;
		//---nn-2.1 write new data end---

		if(tanh(NN_X[DIM.size() - 1][1]) > 0) {
			f << 1 << endl;
		} else {
			f << -1 << endl;
		}
		if(Y[i] * tanh(NN_X[DIM.size() - 1][1]) > 0) {
			++acc;
		}
	}
	cout << "Accuracy " << 1. * acc / D.size()  << " (" << acc << "/" << D.size() << ")" << endl;
	f.close();
}

int main(int argc, char *argv[]) {
	if(argc != 4) {
		cout << "usage nn-predict [predict_dat] [model_file] [output]" << endl;
		return 0;
	}

	srand(time(NULL));

	read_data(argv[1]);

	parse_data();

	read_model(argv[2]);

	solve(argv[3]);

	return 0;
}
