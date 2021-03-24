#include <iostream>
using namespace std;

#include "RenderLoop.h"

int main() {
    RenderLoop loop;
    loop.init();
    loop.mainLoop();
    return 0;
}
