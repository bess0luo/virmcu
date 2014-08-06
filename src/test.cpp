/*
 ============================================================================
 Name        : test.cpp
 Author      : test2
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C++,
 ============================================================================
 */

#include <iostream>
#include <vector>
#include "CVirMcu.h"
#include <boost/foreach.hpp>
class CVirMcu;


using namespace std;

int main(void) {
	unsigned char myarray [] = { 0x02,0x04,0x00,0x00,0x00,0x00 };
	std::vector<unsigned char> m_ROM;
	m_ROM.insert(m_ROM.begin(),myarray,myarray+sizeof(myarray)) ;

	BOOST_FOREACH(const unsigned char &chartep1 , myarray)
	{
		printf("%d",chartep1);
	}



	CVirMcu tem(m_ROM);
	tem.Run();

	cout << "!!!Hello World!!!" << endl; /* prints !!!Hello World!!! */
	return 0;
}
