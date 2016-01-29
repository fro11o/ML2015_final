#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <random>
#include <climits>
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
double TMP_W[MAXL][MAXD][MAXD];
vector<double> AVG;
vector<double> SD;

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

void scale_data() {
	for(int i = 0; i < X[0].size(); ++i) {
		double avg = 0;
		for(int j = 0; j < D.size(); ++j) {
			avg += X[j][i];
		}
		avg /= D.size();
		double var = 0;
		for(int j = 0; j < D.size(); ++j) {
			var += pow(X[j][i] - avg, 2);
		}
		var /= D.size();
		AVG.push_back(avg);
		SD.push_back(sqrt(var));
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

vector<int> AUTO_X[MAXN];
vector<int> AUTO_DIM;
double AUTO_NN_X[MAXL][MAXD];
double AUTO_DELTA[MAXL][MAXD];
double AUTO_TMP_W[MAXL][MAXD][MAXD];
vector<double> AUTO_AVG;
vector<double> AUTO_SD;

void nn_solve(double lrate, int n) {
	memset(AUTO_TMP_W, 0, sizeof(AUTO_TMP_W));
	for(int i = 0; i < n; ++i) {
		int idx = rand() % D.size();
		memset(AUTO_NN_X, 0, sizeof(AUTO_NN_X));
		memset(AUTO_DELTA, 0, sizeof(AUTO_DELTA));
		cout << "nn_solve i " << i << endl;

		for(int j = 1; j < AUTO_DIM[0]; ++j) {
			AUTO_NN_X[0][j] = 1.0 * X[idx][j - 1] - AVG[j - 1];
			if(SD[j - 1] > eps) {
				AUTO_NN_X[0][j] /= SD[j - 1];
			}
		}
		cout << "nn_solve i " << i << endl;
		for(int j = 1; j < AUTO_DIM.size(); ++j) {  //j for layer
			AUTO_NN_X[j - 1][0] = 1.0;
			for(int k = 1; k < AUTO_DIM[j - 1]; ++k) {
				for(int l = 1; l < AUTO_DIM[j]; ++l) {
					if(j == 1) {
						AUTO_NN_X[j][l] += W[j][k][l] * AUTO_NN_X[j - 1][k];
					} else {
						AUTO_NN_X[j][l] += W[j][k][l] * tanh(AUTO_NN_X[j - 1][k]);
					}
				}
			}
		}
		cout << "nn_solve i " << i << endl;
		for(int j = AUTO_DIM.size() - 1; j > 0; --j) {  //j for layer
			for(int k = 1; k < AUTO_DIM[j]; ++k) {  //k for dimension in j-layer
				if(j == AUTO_DIM.size() - 1) {
					AUTO_DELTA[j][k] = -2.0 * (AUTO_NN_X[j - 2][k] - AUTO_NN_X[j][k]);
					continue;
				}
				for(int l = 1; l < AUTO_DIM[j + 1]; ++l) {
					AUTO_DELTA[j][k] += AUTO_DELTA[j + 1][l] * W[j + 1][k][l] * tanhh(AUTO_NN_X[j][k]);
				}
			}
			break;  //only run j = size - 1
		}
		for(int j = AUTO_DIM.size() - 1; j > 0; --j) {  //j for layer
			for(int k = 1; k < AUTO_DIM[j - 1]; ++k) {
				for(int l = 1; l < AUTO_DIM[j]; ++l) {
					AUTO_TMP_W[j][k][l] += AUTO_NN_X[j - 1][k] * AUTO_DELTA[j][l];
				}
			}
			break;  //only run j = size - 1
		}
	}
	for(int i = AUTO_DIM.size() - 1; i >00; --i) {
		for(int j = 1; j < AUTO_DIM[i - 1]; ++j) {
			for(int k = 1; k < AUTO_DIM[i]; ++k) {
				//cout << "i j k " << i << ' ' << j << ' ' << k<< ' ' << lrate << ' ' << (TMP_W[i][j][k] / n) << endl;
				W[i][j][k] -= lrate * (AUTO_TMP_W[i][j][k] / n);
				W[i - 1][k][j] -= lrate * (AUTO_TMP_W[i][j][k] / n);
			}
		}
		break;  //only run i = size - 1
	}
}

void init_w() {

	default_random_engine generator;
	normal_distribution<double> distribution(0.0, 1.0);

	AUTO_DIM.push_back(DIM[0]);
	for(int i = 1; i < DIM.size(); ++i) {

		for(int j = 1; j < DIM[i]; ++j) {
			W[i][0][j] = distribution(generator);
		}
		for(int j = 1; j < DIM[i - 1]; ++j) {
			for(int k = 1; k < DIM[i]; ++k) {
				W[i][j][k] = distribution(generator);
				W[i + 1][k][j] = W[i][j][k];
			}
		}
		
		AUTO_DIM[AUTO_DIM.size() - 1] = DIM[i];
		AUTO_DIM.push_back(DIM[i - 1]);
		cout << "i " << i << endl;
		for(int t = 0; t < 100; ++t) {  //epho
			nn_solve(0.003, 10);
		}
		cout << "i " << i << endl;
	}
}

void dnn_solve(double lrate, int n) {
	//cout << "nn_solve" << endl;
	memset(TMP_W, 0, sizeof(TMP_W));
	for(int i = 0; i < n; ++i) {
		int idx = rand() % D.size();
		memset(NN_X, 0, sizeof(NN_X));
		memset(DELTA, 0, sizeof(DELTA));

		for(int j = 1; j < DIM[0]; ++j) {
			NN_X[0][j] = 1.0 * X[idx][j - 1] - AVG[j - 1];
			if(SD[j - 1] > eps) {
				NN_X[0][j] /= SD[j - 1];
			}
		}
		for(int j = 1; j < DIM.size(); ++j) {  //j for layer
			NN_X[j - 1][0] = 1.0;
			for(int k = 0; k < DIM[j - 1]; ++k) {
				for(int l = 1; l < DIM[j]; ++l) {
					if(j == 1) {
						NN_X[j][l] += W[j][k][l] * NN_X[j - 1][k];
					} else {
						NN_X[j][l] += W[j][k][l] * tanh(NN_X[j - 1][k]);
					}
				}
			}
		}
		for(int j = DIM.size() - 1; j > 0; --j) {  //j for layer
			for(int k = 1; k < DIM[j]; ++k) {  //k for dimension in j-layer
				if(j == DIM.size() - 1) {
					DELTA[j][k] = -2.0 * (Y[idx] - NN_X[j][k]);
					//cout << "DELTA " << DELTA[j][k] << " NN_X " << NN_X[j][k] << endl;
					continue;
				}
				//cout << "DELTA " << DELTA[j][k] << endl;
				for(int l = 1; l < DIM[j + 1]; ++l) {
					DELTA[j][k] += DELTA[j + 1][l] * W[j + 1][k][l] * tanhh(NN_X[j][k]);
					//cout << "DELTA[j + 1][l] " << DELTA[j + 1][l] << " W[j + 1][k][l] " << W[j + 1][k][l] << " NN_X[j][k] " << NN_X[j][k] << " tanhh " << tanhh(NN_X[j][k]) << endl;
				}
				//cout << "DELTA " << DELTA[j][k] << endl;
			}
		}
		for(int j = 1; j < DIM.size(); ++j) {  //j for layer
			for(int k = 0; k < DIM[j - 1]; ++k) {
				for(int l = 1; l < DIM[j]; ++l) {
					TMP_W[j][k][l] += NN_X[j - 1][k] * DELTA[j][l];
				}
			}
		}
	}
	for(int i = 1; i < DIM.size(); ++i) {
		for(int j = 0; j < DIM[i - 1]; ++j) {
			for(int k = 1; k < DIM[i]; ++k) {
				//cout << "i j k " << i << ' ' << j << ' ' << k<< ' ' << lrate << ' ' << (TMP_W[i][j][k] / n) << endl;
				W[i][j][k] -= lrate * (TMP_W[i][j][k] / n);
			}
		}
	}
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
	f_name << AVG.size() << endl;
	for(int i = 0; i < AVG.size(); ++i) {
		f_name << AVG[i] << ' ';
	}
	f_name << endl;
	for(int i = 0; i < SD.size(); ++i) {
		f_name << SD[i] << ' ';
	}
	f_name << endl;
	f_name << DIM.size() << endl;
	for(int i = 0; i < DIM.size(); ++i) {
		f_name << DIM[i] << ' ';
	}
	f_name << endl;

	for(int i = 1; i < DIM.size(); ++i) {
		for(int j = 0; j < DIM[i - 1]; ++j) {
			for(int k = 1; k < DIM[i]; ++k) {
				f_name << W[i][j][k] << ' ';
			}
			f_name << endl;
		}
	}
	f_name.close();
}

int main(int argc, char *argv[]) {
	if(argc <= 5) {
		cout << "usage nn-train [train_dat] [# of iter] [N' each time] [lrate] [<# of 1 layer>, <# of 2 layer>, ...]" << endl;
		return 0;
	}

	srand(time(NULL));

	for(int i = 5; i < argc; ++i) {
		if(stoi(argv[i]) >= MAXD) {
			cout << "MAXD error" << endl;
			return 0;
		}
	}
	
	srand(time(NULL));

	read_data(argv[1]);

	parse_data();

	scale_data();

	DIM.push_back(X[0].size() + 1);
	for(int i = 5; i < argc; ++i) {
		DIM.push_back(stoi(argv[i]) + 1);
	}
	DIM.push_back(2);

	init_w();
	cout << "QQ" << endl;

	for(int iter = 0; iter < stoi(argv[2]); ++iter) {
		dnn_solve(stod(argv[4]), D.size() * stoi(argv[3]) / 100);
	}

	write_model(argv[1]);

	return 0;
}
