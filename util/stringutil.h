//
// C++ Interface: stringutil
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef STRINGUTIL_H
#define STRINGUTIL_H
#include <string>

/**
Convience function for strings

	@author Axel Tetzlaff <axel.tetzlaff@gmx.de>
*/
namespace Occluder {

class StringUtil {
  public:
    StringUtil();

    ~StringUtil();
    inline static std::string trim_right ( const std::string &source , const std::string& t = " " ) {
      std::string str = source;
      return str.erase ( str.find_last_not_of ( t ) + 1 );
    }

    inline static std::string trim_left ( const std::string& source, const std::string& t = " " ) {
      std::string str = source;
      return str.erase ( 0 , source.find_first_not_of ( t ) );
    }

    inline static std::string trim ( const std::string& source, const std::string& t = " " ) {
      std::string str = source;
      return trim_left ( trim_right ( str , t ) , t );
    }

};
}
#endif
