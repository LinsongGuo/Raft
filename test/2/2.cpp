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
	int n = 10000, cnt = 0, cnt2 = 1, len = 5;
	cout << n << endl;
	for(int i = 0; i < 10; ++i) {
		++cnt;
		cout << "put" << ' ' << cnt << ' ' << cnt << endl;
	}
	for(int i = 10; i < n; ++i) {
		if(cnt2 > cnt) {
			++cnt;
			cout << "put" << ' ' << cnt << ' ' << cnt << endl;
		}
		else {
			if(rand() & 1) {
				++cnt;
				cout << "put" << ' ' << cnt << ' ' << cnt << endl;
			}
			else {
				cout << "get" << ' ' << cnt2 << ' ' << cnt2 << endl;
				++cnt2;
			}
		}	
	}
	return 0; 
}
