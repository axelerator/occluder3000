#ifndef DEBUG_H
#define DEBUG_H

  #ifdef DEBUG_ENABLE
    #include <iostream>
    #define DEBUG(x) std::cout << "[DEBUG] " << __FILE__ << " line:" << __LINE__ << ": " << x << std::endl
    #define WARN(x) std::cout <<  "[WARN]  " << __FILE__ << " line:" << __LINE__ << ": " << x << std::endl
    #define ERROR(x) std::cout << "[ERROR] " << __FILE__ << " line:" << __LINE__ << ": " << x << std::endl
  #else
    #define DEBUG(x) ;
    #define WARN(x) ;
    #define ERROR(x) ;
  #endif
  
#endif
