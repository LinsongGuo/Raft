#include <bits/stdc++.h>
using namespace std;

int main() {
	int n;
	cin >> n;
	string opt, key, args;
	
	ofstream fout1("test1.in");
	fout1 << 5000 << std::endl;
	for(int i = 1; i <= 5000; ++i) {
		cin >> opt;
		if(opt == "put") {
			cin >> key >> args;
			fout1 << opt << ' ' << key << ' ' << args << std::endl;	
		}
		else {
			cin >> key;
			fout1 << opt << ' ' << key << ' ' << std::endl;	
		}
	}
	fout1.close();

	ofstream fout2("test2.in");
	fout2 << 5000 << std::endl;
	for(int i = 1; i <= 5000; ++i) {
		cin >> opt;
		if(opt == "put") {
			cin >> key >> args;
			fout2 << opt << ' ' << key << ' ' << args << std::endl;	
		}
		else {
			cin >> key;
			fout2 << opt << ' ' << key << ' ' << std::endl;	
		}
	}
	fout2.close();
	return 0;
}
