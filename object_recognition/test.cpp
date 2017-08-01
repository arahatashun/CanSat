#include <iostream>
#include <camera.hpp>
#include <ctime>

int main(){
  Camera camera;
  int i=0;
  clock_t start = clock();
  while(True)
  {
  camera.takePhoto();
  camera.binarize();
  camera.countArea();
  camera.getCenter();
  clock_t end = clock();     // 終了時間
  std::cout << "duration = " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n"<<endl;
  }
}
