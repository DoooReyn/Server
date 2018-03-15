#ifndef __NOCOPYABLE_H__
#define __NOCOPYABLE_H__
#include <iostream>
using namespace std;

class Nocopyable
{
protected:
	Nocopyable() {};
	~Nocopyable() {};
private:
	Nocopyable(const Nocopyable&);
	const Nocopyable& operator = (const Nocopyable&);
};
#endif
