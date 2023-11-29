# include "include/algo.h"

int main(){

    int T = 0;

    int t = 0;

    cin >> T;

    cin >> t;

    for(int R =0; R < T; R++){

        string in;

        vector<string> line; 

        for(int i=0; i< t; i++){
            
            cin >> in;

            line.push_back(in);

        }

        for(int i=0; i< t; i++){
            
            cout << line[i] << " ";

        }

        cout << endl;
        

    }
    
    return 0;
}

