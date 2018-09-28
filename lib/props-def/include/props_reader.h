	// classes example
#include <iostream>
	using namespace std;

class PropsReader {
    int width, height;
  public:
    void set_values (int,int);
    int area() {return width*height;}
};
