#include <bits/stdc++.h>
using namespace std;

const int maxn = 1e5 + 10;
string keys[maxn];

string get(int len) {
	string res = "";
	for(int i = 0; i < len; ++i) {
		switch(rand() % 3) {
		case 0:
			res += char('0' + rand() % 10);
			break;
		case 1:
			res += char('a' + rand() % 26);
			break;
		case 2:
			res += char('A' + rand() % 26);
			break;
		}
	}
	return res;
}


int main() {
	srand(time(NULL));
	int n = 1000, cnt = 0;
	cout << n << endl;
	for(int i = 0; i < 10; ++i) {
		string key = get(rand() % 10 + 1);
		string args = get(rand() % 10 + 1);
		cout << "put" << ' ' << key << ' ' << args << endl;
		keys[++cnt] = key;
	}
	for(int i = 10; i < n; ++i) {
		if(rand() & 1) {
			string key = get(rand() % 10 + 1);
			string args = get(rand() % 10 + 1);
			cout << "put" << ' ' << key << ' ' << args << endl;
			keys[++cnt] = key;
		} 
		else {
			if(rand() % 9) {
				cout << "get" << ' ' << keys[rand() % cnt + 1] << endl;
			}
			else {
				cout << "get" << ' ' << get(rand() % 5 + 1) << endl;
			}
		}
	}
	return 0; 
}
