


#ifndef DEBUG_H__
#define DEBUG_H__

#include <crtdbg.h>
#include <windows.h>
#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>



#define trace_msg(stream_args) do { std::stringstream s; s <<  __FILE__ << "(" << __LINE__ <<  ") : " << __FUNCTION__ << " " << stream_args << "\n"; OutputDebugStringA (s.str().c_str()); } while (0)

#endif