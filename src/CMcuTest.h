/*
 * CMcuTest.h
 *
 *  Created on: Aug 6, 2014
 *      Author: ranger.shi
 */

#ifndef CMCUTEST_H_
#define CMCUTEST_H_
#include "CVirMcu.h"
#include <ostream>
using namespace std;
class CMcuTest {
public:
	CVirMcu *ptmcu;
	CMcuTest();
	void TestReg();
	virtual ~CMcuTest();
};

#endif /* CMCUTEST_H_ */
