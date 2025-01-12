#include<iostream>
using namespace std;

int tree[20] = {0,36,8,50,6,22,45,107,1,7,20,35,40,47,99,1001};

int func7(int esi, int node){
    if(node > 15) return -1;
    int edx = tree[node];
    if(edx > esi){
        return 2*func7(esi, 2*node);
    }
    else if(edx == esi){
        return 0;
    }
    else{
    	return 1 + 2*func7(esi, 2*node + 1);
    }
}

signed main() {
	ios::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);
	
	for(int i = 0; i <= 1001; i++){
		int ans = func7(i, 1);
		if(ans == 3) cout << i << endl;
	}
	
	return 0;
}



