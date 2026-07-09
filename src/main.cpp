#include <iostream>
#include <string>
#include<bits/stdc++.h>
using namespace std;
int main() {
  // Flush after every std::cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  // TODO: Uncomment the code below to pass the first stage
  while(1){
  cout << "$ ";
  string s;
  getline(cin, s);
  if(s=="exit") break;
  else if(s.length()>=5 && s.substr(0,5)=="echo "){
    cout<< s.substr(5)<<endl;
  }
  else if(s=="exit") break;
  
  else if(s.length()>=5 && s.substr(0,5)=="type "){
  string cmd=s.substr(5);
  if(cmd=="echo" || cmd=="type" || cmd=="exit"){
    cout<<cmd<<" is a shell builtin\n";
  }
  else cout<<cmd<<": not found\n";}
  else {cout<<s<<": "<<"command not found\n";
}

}
}