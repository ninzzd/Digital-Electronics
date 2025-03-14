#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <tuple>
#include <math.h>


using namespace std;

uint64_t pow2(int n){
    if(n == 0) return 1;
    return 2*pow2(n-1);
}
uint64_t ones(uint64_t n){ // only for bitmask inputs
    if(n == 0) return 0;
    else return (n%2?0ULL:1ULL) + ones(n >> 1ULL);
}
string getMinterm(int n,int order,int* ones){
    string s = "";
    *ones = 0;
    while(n > 0){
        s += to_string(n%2);
        *ones+=n%2;
        n/=2;
    }
    if(n >= pow2(order)) return "";
    else{
        int len = s.length();
        for(int i = 0;i < order - len;i++){
            s+='0';
        }
    }
    return s;
}
int getAdjacentIndex(string s1, string s2){
    if(s1.length() != s2.length()) return -2;
    else{
        int uneq = 0;
        int eq = 0;
        int j = 0;
        for(int i = 0;i < s1.length();i++){
            if(s1[i] != s2[i]){
                if((s1[i] == '0' && s2[i] == '1')||(s2[i] == '0' && s1[i] == '1')){
                    uneq++;
                    j = i;
                }
            }
            else eq++;
        }
        if(uneq + eq == s1.length() && uneq == 1) return j;
        else return -1;
    }
}
bool compare(tuple<int,string,uint64_t> a,tuple<int,string,uint64_t> b){
    return (get<0>(a) == get<0>(b)?get<1>(a) < get<1>(b):get<0>(a) < get<0>(b));
}
bool compare2(tuple<int,string,uint64_t> a,tuple<int,string,uint64_t> b){
    int a1, b1;
    a1 = b1 = 0;
    for(int i = 0;i < 64;i++){
        if((get<2>(a) >> i)%2)a1++;
        if((get<2>(b) >> i)%2)b1++;
    }
    return a1 > b1;
}
void display(vector<tuple<int,string,uint64_t>> v){
    cout << endl;
    for(int i = 0;i < v.size();i++){
        cout << "Ones: " << get<0>(v[i]) << " | " << "Implicants: " << get<1>(v[i]) << " | " << "Minterms: (";
        int flag = 0;
        for(int j = 0;j < 64;j++){
            if((get<2>(v[i]) >> j)%2 == 1){
                if(!flag){
                    flag++;
                    cout << j;
                }
                else cout << "," << j;
            }
        }
        cout << ")" << endl;
    }
}
int main(){
    int n;
    vector<string> vars; // MSB->LSB
    uint64_t minterms = 0;
    uint64_t dont_cares = 0;
    vector<tuple<int,string,uint64_t>> implicants;
    cout << "Enter the number of input variables:" << endl;
    cin >> n;
    cout << "Enter the characters representing the input variables (from MSB to LSB)" << endl;
    for(int i = 0;i < n;i++){
        string s;
        cin >> s;
        vars.push_back(s);
        // if((c>='a' && c<='z')||(c>='A' && c<='Z'));
        // else{
        //     cout << "Invalid Character. Accepted characters: A->Z and a->z. Please type again!" << endl;
        //     i--;
        // }
    }
    // cout << endl << vars << endl;
    cout << "Enter the activated minterms: (To stop, type the integer '-1')" << endl;
    int count = 0;
    while(true){
        int input;
        cin >> input;
        count++;
        if(count >= pow2(n)){
            cout << "Too many minterms. Invalid function!";
            exit(-1);
        }
        else{
            if(input == -1)
                break;
            else if(input >= 0 && input < pow2(n)){
                int ones = 0;
                string s = getMinterm(input,n,&ones);
                minterms = minterms | (1ULL << input);
                // cout << input << endl;
                // cout << s << endl;
                // cout << (1ULL << input) << endl;
                implicants.push_back(make_tuple(ones,s,1ULL << input));
            }
            else{
                cout << "Invalid Input. This input will be discarded. Try again!" << endl;
            }
        }
    }
    cout << "Enter the don't cares minterms: (To stop, type the integer '-1')" << endl;
    while(true){
        int input;
        cin >> input;
        count++;
        if(count >= pow2(n)){
            cout << "Too many minterms. Invalid function!";
            exit(-1);
        }
        else{
            if(input == -1)
                break;
            else if(input >= 0 && input < pow2(n)){
                int ones = 0;
                string s = getMinterm(input,n,&ones);
                dont_cares = dont_cares | (1ULL << input);
                // cout << input << endl;
                // cout << s << endl;
                // cout << (1ULL << input) << endl;
                implicants.push_back(make_tuple(ones,s,1ULL << input));
            }
            else{
                cout << "Invalid Input. This input will be discarded. Try again!" << endl;
            }
        }
    }
    int stage = 0;
    while(true){
        vector<tuple<int,string,uint64_t>> new_implicants;
        uint64_t accounted = 0;
        sort(implicants.begin(),implicants.end(),compare);
        for(int i = 0;i < implicants.size()-1;i++){
            if(get<1>(implicants[i]) == get<1>(implicants[i+1])){
                implicants.erase(implicants.begin()+i);
                i--;
                count++;
            }
        }
        cout << endl << "Stage: " << stage << endl;
        display(implicants);
        int present, next, nnext;
        count = 0;
        for(present = 0;present < implicants.size();){
            for(next = present+1;next < implicants.size();next++) if(get<0>(implicants[next]) == get<0>(implicants[present]) + 1) break;
            for(nnext = next+1;nnext < implicants.size();nnext++) if(get<0>(implicants[nnext]) == get<0>(implicants[next]) + 1) break;
            // cout << "Present: " << present << ", Next: " << next << ", Next-to-Next: " << nnext << endl;
            for(int i = present;i < next && i < implicants.size();i++){
                for(int j = next;j < nnext && j < implicants.size();j++){
                    int index = -1;
                    if((index = getAdjacentIndex(get<1>(implicants[i]),get<1>(implicants[j]))) >= 0){
                        accounted = accounted | (1ULL << i | 1ULL << j);
                        string new_implicant = get<1>(implicants[i]);
                        int ones;
                        uint64_t minterms;
                        new_implicant[index] = '_';
                        ones = get<0>(implicants[i]);
                        minterms = get<2>(implicants[i]) | get<2>(implicants[j]);
                        new_implicants.push_back(make_tuple(ones,new_implicant,minterms));
                        count++;
                    }
                }
            }
            present = next;
            next = nnext;
        }
        if(count == 0) break;
        // display(new_implicants);
        for(int i = 0;i < implicants.size();i++){
            if((accounted >> i)%2){
                accounted = (accounted >> (i+1))>>(i) | accounted%pow2(i);
                implicants.erase(implicants.begin() + i);
                i--;
            }
            // for(int j = 0;accounted >> j > 0;j++)
            //     cout << (accounted >> j)%2;
            // cout << endl;
            // display(implicants);
        }
        implicants.insert(implicants.end(),new_implicants.begin(),new_implicants.end());
        stage++;
    }
    for(int i = 0;i < implicants.size();i++){
        string s = "";
        count = 0;
        for(int j = 0;j < n;j++){
            string minterm = "";
            if(get<1>(implicants[i])[j] != '_'){
                if(get<1>(implicants[i])[j] == '0') {
                    minterm += vars[n-1-j];
                    minterm += (char)'\'';
                }
                else if(get<1>(implicants[i])[j] == '1'){
                    minterm += vars[n-1-j];
                }
                if (count>0) minterm+=".";
                count++;
            }
            minterm.append(s);
            s = minterm;
        }
        get<1>(implicants[i]) = s;
    }

    // Sorts the vector based on the number of minterms covered per implicant (descending order)
    // sort(implicants.begin(),implicants.end(),compare2);
    cout << endl << "Prime Implicants Table (before the removal of don't care minterms): " << endl;
    display(implicants);
    // -------------------------------------------------
    for(uint64_t i = 0;i < implicants.size();i++){
        for(uint64_t j = 0;j < 64;j++){
            if((get<2>(implicants[i]) >> j)%2 == 1ULL && (dont_cares >> j)%2 == 1ULL){
                get<2>(implicants[i]) = ((get<2>(implicants[i]) >> (j+1ULL)) << (j+1ULL)) | (get<2>(implicants[i])%(pow2(j)));
            }
        }
    }

    sort(implicants.begin(),implicants.end(),compare2);
    cout << endl << "Prime Implicants Table (after the removal of don't care minterms): " << endl;
    display(implicants);
    uint64_t EP_implicants = 0;
    uint64_t minterms_covered = 0;

    for(uint64_t i = 0;i < 64;i++){
        if((minterms >> i)%2 == 0) continue;
        int first_index = -1;
        int cnt = 0;
        for(uint64_t j = 0;j < implicants.size();j++){
            if((get<2>(implicants[j]) >> i)%2 == 1){
                if(first_index<0)first_index = j;
                cnt++;
            }
        } 
        if(cnt == 1 & first_index != -1) {
            EP_implicants = EP_implicants | (1ULL << first_index);
            minterms_covered = minterms_covered | get<2>(implicants[first_index]);
        }
    }
    bool complete = false;
    if(minterms_covered == minterms){
        complete = true;
    }
    else{
        for(uint64_t i = 0;i < implicants.size();i++){
            if((EP_implicants >> i)%2 == 0){
                if(ones(minterms_covered | get<2>(implicants[i]) > ones(minterms_covered))){
                    EP_implicants = EP_implicants | (1ULL << i);
                    minterms_covered = (minterms_covered | get<2>(implicants[i]));
                    if(minterms_covered == minterms) break; 
                    else continue;
                }
            }
        }
        complete = true;
    }
    cout<< endl << "f(";
    for(uint64_t i = 0;i < vars.size();i++){
        if(!i) cout << vars[i];
        else cout << "," << vars[i];
    }
    cout << ") =";

    count = 0;
    for(uint64_t i = 0;i < implicants.size();i++){
        if((EP_implicants >> i)%2 == 1){
            if(!count) cout << " " << get<1>(implicants[i]);
            else cout << " + " << get<1>(implicants[i]);
            count++;
        }
    }
    cout << endl;
    return 0;
}