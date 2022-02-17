#include <stdlib.h>
#include "armorDetector.h"

int main()
{
    std::string home_path = getenv("HOME");
    para::Parameter Para;
    Para.open(home_path+"/AutoAim_VREP/config/mypara.xml");

    ly::armorDetector armorDetector_;
    armorDetector_.Core();

    // while(1){
    //     std::cout << "w" << std::endl;
    // }
}


