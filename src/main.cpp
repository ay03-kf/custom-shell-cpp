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
  std::cin>>s;
  if(s=="exit") break;
  std::cout<<s<<": "<<"command not found\n";}

}
