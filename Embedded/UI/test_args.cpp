#include <iostream>
#include<windows.h>

using namespace std;

// Take args, double them and return to py script???

int main(int argc, char *argv[]) 
{
    cout << "You have entered " << argc 
         << " arguments:" << "\n"; 
  
    for (int i = 0; i < argc; ++i) 
        cout << argv[i] << "\n"; 
    
	cout << "wait\n";
	
	Sleep(3000);
	
	cout << "program end\n";
	
    return 0;
}