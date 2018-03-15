#ifndef __SIGAAL_CATCH_H__
#define __SIGAAL_CATCH_H__

#include "type_define.h"

//typedef std::function<void(int32 signum)> CatchCallback;
typedef void(*CatchCallback)(int32 signum);


void SetSignedCatched(CatchCallback& cb);

#endif
