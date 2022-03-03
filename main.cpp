
#include <iostream>
#include <map>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <sstream>
using namespace std;
#define rep(i,a,b) for(int i=a; i<b; i++)
//calculate the unnormalized posterior probabilities of the given message
double calculateUPP(map<string, int> word, string msg, int k, int totalWords_each, int n, int words_all){
    // we need  p_ham and P(wi|ham)
    int count =0;
    map<string,int>::iterator search;
    search = word.find(msg);   
    if(search !=word.end()){
        count =search->second;
        // cout << count <<endl;
    }
    long double rvalue;
    // if(count==0){
    //     rvalue= (double)(1)/(double)(words_all);
    //     // cout << count <<endl;
    // }else rvalue= (double)(count+k)/(double)(totalWords_each+k*n);

    rvalue= (double)(count+k)/(double)(totalWords_each+k*n);
    // cout <<rvalue <<endl;
    // cout << "rvalue " << rvalue <<endl;
    // cout << log(rvalue) <<endl;
    return rvalue;
    
}

void getMsgs(string filename, vector<vector<string> > &msg){
    ifstream myfile(filename); 
    vector<string> s;
    if (myfile.is_open())
    {            
        string line;
        string str;
        while (getline(myfile, line, '\n')){
            stringstream ss;
                ss << line;
                while (getline(ss, str, ' ') ){     // スペース（' '）で区切って，格納
                    s.push_back(str);
                }
            msg.push_back(s);
            s.clear();
        }
        myfile.close();
    }else cout <<"unable to open!" <<endl;
}

void checkDuplicated(string filename, map<string, int> &word, int &distinct, int &total_word, int &_totalLines){
        ifstream myfile(filename); 
        if (myfile.is_open())
        {
            string line;
            while (getline(myfile, line)){
                _totalLines++;
            }
            myfile.close();
            ifstream myfile(filename);
            string temp;
            while ( myfile >> temp )
            {
                total_word++;
            //   cout << temp << endl;
            if(!word.empty()){
                map<string,int>::iterator search;
                search = word.find(temp);
                if(search !=word.end()){
                    search->second = search->second+1;
                    
                }else{
                    word.insert(pair<string, int>(temp,1));
                    distinct++;
                }
            }   else {
                word.insert(pair<string, int>(temp,1));
                distinct++;
            }
         }
        }else std::cout << "Unable to open file"; 
        myfile.close();

}


int main(int argc, char* argv[]){
    if (argc <2){
        cout << "please input at least one file" << endl;
    return 0;
    }
    // store the training files
    string train_files[2];

    //stor the testing files
    string test_files[2];
    int k=1;  
    // its better to put try catch here;otherwise will face a consequence
    rep(i,0,2){
        train_files[i] = argv[i+1];
        // cout << train_files[i] << endl;  
    }
    rep(i,0,2){
        test_files[i] = argv[i+3];
        // cout << test_files[i] << endl;  
    }  
    k =atoi(argv[5]);


    int totalLines_all = 0;
    int _totalwords=0;
    vector<vector<string> > msgs[2];
    vector<int> numberofLines;
    vector<int> totalNumberofWords_all;
    vector<int> totalNumberoDistinctfWords_all;
    double log_UPP_spam, log_UPP_ham;
    vector<map<string,int> > map_all;
    // all the distinct words in ham and spam
    map<string, int> distinct_Words_all;
    rep(i,0, 2){
        map<string, int> word;
        int totalLines=0;
        int totalNnumberofDistinctdWords=0;
        int totalNumberofWords=0;
        int temp=0;
        checkDuplicated(train_files[i],word,totalNnumberofDistinctdWords,totalNumberofWords,totalLines);
        checkDuplicated(train_files[i],distinct_Words_all,temp,temp,temp);
        
        // cout << "the total number of lines in " << train_files[i] << " is " <<totalLines <<"\n"
        // << " and the total number of words is " << totalNumberofWords <<"\n"
        // << " and the total number of distinct words is " << totalNnumberofDistinctdWords << endl;
        totalLines_all+=totalLines;
        numberofLines.push_back(totalLines);
        totalNumberofWords_all.push_back(totalNumberofWords);
        totalNumberoDistinctfWords_all.push_back(totalNnumberofDistinctdWords);
        _totalwords+= totalNumberofWords;
        map_all.push_back(word); 
    }
    rep(i,0, 2){
        getMsgs(test_files[i], msgs[i]);
    }

    // cout << "the total number of distinct words is " << distinct_Words_all.size() <<endl;
    // cout << "the total number of  words is " << _totalwords <<endl;
    int totalDistinctWords=distinct_Words_all.size();

    //-------------------calculations start here-------------------

    //calculate conditional prob for spam, ham
    long double p_ham =(double)(numberofLines[0]+k)/(double)(totalLines_all+k*(2));
    long double p_spam = (double)(numberofLines[1]+k)/(double)(totalLines_all+k*(2));
    // calculate unnormalized posterior prob
    log_UPP_ham = log(p_ham);
    log_UPP_spam = log(p_spam);
    
    // rep(i,0,totalNumberoDistinctfWords_all.size())
    //  totalDistinctWords += totalNumberoDistinctfWords_all[i];


    rep(i,0,2){ // a dataset
        rep(j,0,msgs[i].size()){ //one line
            rep(n,0,map_all.size()){
                rep(m,0,msgs[i][j].size()){ // a word in the line
                    // cout << msgs[i][j][m] << " ";
                    if(n==0)
                        log_UPP_ham += log(calculateUPP(map_all[n], msgs[i][j][m], k, totalNumberofWords_all[n],totalDistinctWords,_totalwords));
                    else
                        log_UPP_spam += log(calculateUPP(map_all[n], msgs[i][j][m], k, totalNumberofWords_all[n],totalDistinctWords,_totalwords));

                }
            }
            (log_UPP_ham>log_UPP_spam) ? cout << "ham ": cout <<"spam ";
            cout << log_UPP_ham << " " <<log_UPP_spam << endl;
            log_UPP_ham=log(p_ham);
            log_UPP_spam=log(p_spam);
        }
    }





    // rep(i,0,map_all.size()){ 
    //     map<string,int>::iterator w;
    //     for(w= map_all[i].begin(); w!=map_all[i].end(); w++){
    //         if(i==0){
    //             cout << w->first << ": "<< w->second <<endl;
    //             log_UPP_spam+=log(calculateUPP(, k, totalNumberofWords_all[i],totalDistinctWords,_totalwords));
    //             // cout << UPP_spam <<endl;
    //         }
    //         else{
    //             cout << w->first << ": "<< w->second <<endl;
    //             log_UPP_ham+=log(calculateUPP(w->second, k, totalNumberofWords_all[i],totalDistinctWords,_totalwords));
    //         }
    //     }
    // }

        //calculations

        // double p_w_ham =(double)(totalNumberoDistinctfWords_all[0]+k)/(double)(totalNumberofWords_all[0]+k*words_all.size());
        // double p_w_spam =(double)(totalNumberoDistinctfWords_all[1]+k)/(double)(totalNumberofWords_all[1]+k*words_all.size());
        cout << "P(ham) = " << p_ham << endl;
        cout << "P(spam) = " << p_spam <<endl;
        // I found something wrong with using log funcitons. It gives me a huge negative number back.
        //


 return 0;

}