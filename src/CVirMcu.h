/*
 * CVirMcu.h
 *
 *  Created on: Aug 4, 2014
 *      Author: ranger.shi
 */

#ifndef CVIRMCU_H_
#define CVIRMCU_H_

#include <cstdio>
#include <exception>
#include <map>
#include <stdarg.h>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>
#include <stdarg.h>
class CVirMcu;
class MdSys;
class MdRges;
template<class T>
class CUPReg {
	unsigned char m_Addr;
	CVirMcu *pmcu;
public:
	CUPReg(CVirMcu *mcu) {
		m_Addr = 255;
		pmcu = mcu;
	}

	int64_t& GetRegRe(void);

	void SetAddr(unsigned char Addr) {
		m_Addr = Addr;
	}

	CUPReg<T>& operator>>=(int data);
	T operator>>(int data);
	CUPReg<T>&operator|=(int data);
	CUPReg<T>&operator|=(const T &data);
	T operator&(int data) ;
	CUPReg<T>&operator&=(int data);
	T operator<<(int data);
	CUPReg<T>&operator<<=(int data);
	CUPReg<T>&operator^=(int data);
	CUPReg<T>&operator^=(T &data);
	CUPReg<T>&operator~();
	CUPReg<T>&operator+=(T &data) ;
	CUPReg<T>&operator+=(int data);
	T operator+(T &data);
	T operator+(int data);
	T operator-(int data);
	T operator-(T &data);
	CUPReg<T>&operator-=(int data);
	uint64_t operator()(void);
//	CUPReg<T>&operator=(T &data);
	CUPReg<T>&operator=(uint64_t &data);

	CUPReg<T>&operator=(int data);
	CUPReg<T>& operator++(int );
	CUPReg<T>&operator--();
	bool operator!=(int data);
	bool operator!=(T& data);
	bool operator<(int data);
	bool operator<(T& data);
	bool operator==(int data);
	bool operator==(T& data);
};

typedef CUPReg<uint64_t> Reg64;
typedef CUPReg<uint32_t> Reg32;
typedef CUPReg<uint16_t> Reg16;

class Cpsw: public CUPReg<uint64_t> {
//    INT8U p :1;
//    INT8U null :1;
//    INT8U ov:1;
//    INT8U rs0:1;
//    INT8U rs1:1;
//    INT8U f0:1;
//    INT8U ac:1;
//    INT8U cy:1;

	bool setPostion(bool flag, int pos) {
		if (flag) {
			this->GetRegRe() |= ((uint64_t) 1 << pos);
		} else {
			this->GetRegRe() &= (~(((uint64_t) 1 << pos)));
		}
		return flag;
	}
	bool getPostion(int pos) {
		return 0 != (this->GetRegRe() & ((uint64_t) 1 << pos));
	}
public:

	bool GetP() {
		return getPostion(0);
	}

	bool SetP(bool flag) {
		return setPostion(flag, 0);
	}

	bool GetOv() {
		return getPostion(2);
	}

	bool SetOv(bool flag) {
		return setPostion(flag, 2);
	}

	bool GetAc() {
		return getPostion(6);
	}

	bool SetAc(bool flag) {
		return setPostion(flag, 6);
	}


	bool GetCy() {
		return getPostion(7);
	}

	bool SetCy(bool flag) {
		return setPostion(flag, 7);
	}

	Cpsw(CVirMcu *mcu) :
			CUPReg<uint64_t>(mcu) {

	}

};

class MdSys {
public:
	Reg64 a;
	Reg64 b;
	Reg64 sp;
	Cpsw psw;
	Reg64 dptr;
	Reg64 PC;

	MdSys(CVirMcu *mcu) :
			a(mcu), b(mcu), sp(mcu), psw(mcu), dptr(mcu), PC(mcu) {
	}

};
class MdRges {

public:

	MdRges(CVirMcu *mcu) :
			R0(mcu), R1(mcu), R2(mcu), R3(mcu), R4(mcu), R5(mcu), R6(mcu), R7(mcu) {

	}
	Reg64 R0;
	Reg64 R1;
	Reg64 R2;
	Reg64 R3;
	Reg64 R4;
	Reg64 R5;
	Reg64 R6;
	Reg64 R7;
};

class CVirMcu {

	MdSys Sys;
	MdRges Rges;

	std::vector<int64_t> m_ChipRam;
	std::vector<unsigned char> m_ChipROM;
	friend class CUPReg<uint64_t> ;
	friend class CUPReg<uint32_t> ;
	friend class CUPReg<uint16_t> ;
	friend class Cpsw;
public:
	template<typename T>
	T GetOpcodeData() {
		T temp = 0;
		memcpy(&temp, &m_ChipROM[Sys.PC()+1], sizeof(temp));
		return temp;
	}

	template<typename T,typename T2>
	T GetOpRamData(T2 t2) {
		T temp = 0;
		memcpy(&temp, &m_ChipRam[t2], sizeof(T));
		return temp;
	}

	unsigned char GetOpCode() {
		unsigned char temp = 0;
		memcpy(&temp, &m_ChipROM[Sys.PC()], sizeof(temp));
		return temp;
	}
	bool Push(Reg64 data)
	{
		m_ChipRam[Sys.sp()]=data();
		(Sys.sp)++;
		return true;
	}
	bool Push(uint64_t data)
	{
		m_ChipRam[Sys.sp()]=data;
		(Sys.sp)++;
		return true;
	}


	void MD_ADD(uint64_t data);
	void MD_SUBB(uint64_t data);
	int Run();
	void StepRun( unsigned char code);
	void Opcode_00_NOP(void);
	void Opcode_02_LJMP_Addr16(void);
	void Opcode_12_LCALL_Addr16(void);
	void Opcode_05_INC_Direct(void);
	void Opcode_15_DEC_Direct(void);
	void Opcode_25_ADD_A_Direct(void);
	void Opcode_24_ADD_A_Data(void) {
		uint64_t data = 0;
		data = GetOpcodeData<uint64_t>();
		MD_ADD(data);
		Sys.PC += (1 + sizeof(uint64_t));
	}
	void Opcode_94_SUBB_A_Data(void);
	void Opcode_95_SUBB_A_Direct(void);
    void Opcode_C0_PUSH_Direct(void);
    void Opcode_D0_POP_Direct(void);
    void Opcode_D5_DJNZ_Direct_Rel();
	void Opcode_22_RET(void);
	void Opcode_E4_CLR_A(void);
	void Opcode_A4_MUL_AB(void);
	void Opcode_E5_MOV_A_Direct(void);
	void Opcode_84_DIV_AB(void);
	CVirMcu(const std::vector<unsigned char> & rom);
	virtual ~CVirMcu();
};

#endif /* CVIRMCU_H_ */
