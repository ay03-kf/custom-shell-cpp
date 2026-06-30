#include <iostream>
#include <string>
#include<bits/stdc++.h>
int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage
  while(1){
  std::cout << "$ ";
  std::string s;
  std::getline(std::cin, s);
  if(s.length()>=5 && s.substr(0,5)=="echo "){
    std::cout<< s.substr(5)<<std::endl;
  }
  else if(s=="exit") break;
  else {std::cout<<s<<": "<<"command not found\n";}
}

}
