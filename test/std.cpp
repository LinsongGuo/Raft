#include <bits/stdc++.h>
using namespace std;

map<string, string> mp;
int main() {
	int n;
	string opt, key, args;
	cin >> n;
	for(int i = 1; i <= n; ++i) {
		cin >> opt;
		if(opt == "put") {
			cin >> key >> args;
			mp[key] = args;
		}
		else {
			cin >> key;
			if(mp.count(key)) cout  << key << ' ' << mp[key] << endl;
			else cout << key << ' ' << "NOT FOUND" << endl;
		}
	}
	return 0;
}
