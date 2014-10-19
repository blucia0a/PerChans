#include <vector>

template<class T>
class PC_Channel{

public:
  void add(); /*tentative*/
  T *get();   /*tentative*/
  void commit(); /*non-tentative*/

  /*default: ask for 10 entries in the pipe
    each of size sizeof(T)*/
  PC_Channel( int num = 10, int size = sizeof(T) );

protected:
  vector<T> entries;

};
