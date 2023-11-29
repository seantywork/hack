# include "1576.cc"

int main(){

    int T = 0;

    Solution sol;

    cin >> T;

    for(int R =0; R < T; R++){

        string quiz;

        string ans;

        cin >> quiz;

        ans = sol.modifyString(quiz);

        cout << ans << endl;

    }
    
    return 0;
}

