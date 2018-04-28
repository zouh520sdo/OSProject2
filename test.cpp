// constructing vectors
#include <iostream>
#include <vector>
void print (std::vector<char> &memo){
    int row=0;
    for (unsigned int i=0;i<32;i++){
        std::cout << "=";
    }
    std::cout <<"\n";
    for (unsigned int i=0;i<memo.size();i++){
        std::cout << memo[i];
        row++;
        if (row>=32){std::cout <<"\n"; row=0;}
    }
    for (unsigned int i=0;i<32;i++){
        std::cout << "=";
    }
    std::cout <<"\n";
}
int defrag(std::vector<char> &memo){
    int result=0;
    int empty=0;
    bool collect=true;
    int notempty=0;
    std::vector<int> e;
    std::vector<int> note;
    
    for (unsigned int i=0;i<memo.size();i++){
        if (memo[i]!='.' && collect==true){
            e.push_back(empty);
            collect=false;
            empty=0;
        }
        else if (memo[i]=='.'){
            collect=true;
            empty++;
        }
    }
    collect=true;
    for (unsigned int i=0;i<memo.size();i++){
        if ((memo[i]=='.' && collect==true)){
            note.push_back(notempty);
            collect=false;
            notempty=0;
        }
        else if (memo[i]!='.' && i!=memo.size()-1){
            collect=true;
            notempty++;
        }
        else if (memo[i]!='.' && i==memo.size()-1 ){
            collect=true;
            notempty++;
            note.push_back(notempty);
        }
    }
    
    for (unsigned int i=1;i<e.size();i++){
        e[i]+=e[i-1];
    }
    for (unsigned int i=0;i<note.size();i++){
		std::cout << e[i] << std::endl;
        result+=e[i]*note[i];
    }

    std::vector<char> copy;
    for (unsigned int i=0;i<memo.size();i++){
        if (memo[i]!='.'){
            copy.push_back(memo[i]);
        }
    }
    while (copy.size()!=memo.size()){
        copy.push_back('.');
    }
    memo=copy;
    return result;
}
void noncontipush(std::vector<char> &memo,char a,int max){
    int count=0;
    for (unsigned int i=0;i<memo.size();i++){
       if (memo[i]=='.'){
           memo[i]=a;
           count++;
           if (count==max) break;
        }
    }
}

int main ()
{
  // constructors used in the same order as described above:
  
    std::vector<char> memo={'a','.','b','.','.','n','.','.','b','.','c','n','b','.','.','n' };
    int d=defrag(memo);
    print(memo);
    std::cout<<d;
    return 0;
}

