/*
 * CMcuTest.cpp
 *
 *  Created on: Aug 6, 2014
 *      Author: ranger.shi
 */

#include <vector>
#include "CMcuTest.h"

CMcuTest::CMcuTest() {
	// LEARN Auto-generated constructor stub
	unsigned char myarray [] = { 0x02,0x04,0x00,0x00,0x00,0x00 };
	std::vector<unsigned char> Pcode;
	Pcode.insert(Pcode.begin(),myarray,myarray+sizeof(myarray ));
	ptmcu = new CVirMcu(Pcode);
}
void CMcuTest::TestReg()
{

}
CMcuTest::~CMcuTest() {
	// LEARN Auto-generated destructor stub
	if(ptmcu != NULL)
	{
		delete ptmcu;
	}
}

