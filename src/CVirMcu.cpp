/*
 * CVirMcu.cpp
 *
 *  Created on: Aug 4, 2014
 *      Author: ranger.shi
 */

#include "CVirMcu.h"

CVirMcu::CVirMcu(const std::vector<unsigned char> & rom) :
		Sys(this), Rges(this) {

	Sys.a.SetAddr(0xE0);
	Sys.b.SetAddr(0xF0);
	Sys.dptr.SetAddr(0x82);
	Sys.psw.SetAddr(0xD0);
	Sys.sp.SetAddr(0x81);

	Rges.R0.SetAddr(0x00);
	Rges.R1.SetAddr(0x01);
	Rges.R2.SetAddr(0x02);
	Rges.R3.SetAddr(0x03);
	Rges.R4.SetAddr(0x04);
	Rges.R5.SetAddr(0x05);
	Rges.R6.SetAddr(0x06);
	Rges.R7.SetAddr(0x07);
	m_ChipRam.clear();
	uint64_t tem = 0;
	m_ChipRam.insert(m_ChipRam.begin(), 256, tem);
	Sys.sp = 0x07;
	m_ChipROM.insert(m_ChipROM.begin(), rom.begin(), rom.end());
}

CVirMcu::~CVirMcu() {

}
void CVirMcu::Opcode_C0_PUSH_Direct(void) {
	unsigned short addr = 0;
	addr = GetOpcodeData<unsigned short>();
	Push(m_ChipRam[addr]);
	Sys.PC += 3;
}
void CVirMcu::Opcode_22_RET(void) {
	Sys.PC = GetOpRamData<uint64_t>(Sys.sp());
	--Sys.sp;
}
void CVirMcu::Opcode_D0_POP_Direct(void) {
	uint64_t tep = GetOpRamData<uint64_t>(Sys.sp());
	unsigned short addr = 0;
	addr = GetOpcodeData<unsigned short>();
	m_ChipRam[addr] = tep;
	--Sys.sp;
	Sys.PC += 3;
}
void CVirMcu::Opcode_D5_DJNZ_Direct_Rel(void) {
	uint32_t addr = GetOpcodeData<uint32_t>();
	Sys.PC += 5;
	if (--m_ChipRam[(addr & 0xFFFF)] != 0) {
		Sys.PC = (addr >> 16);
	}
}
void CVirMcu::Opcode_E4_CLR_A(void) {
	Sys.a = 0;
	Sys.PC++;
}
void CVirMcu::Opcode_E5_MOV_A_Direct(void) {
	unsigned short addr = 0;
	addr = GetOpcodeData<unsigned short>();
	Sys.a = m_ChipRam[addr];
	Sys.PC += 3;
}
void CVirMcu::Opcode_84_DIV_AB(void) {
	uint64_t data1;
	uint64_t data2;
	data1 = Sys.a();
	data2 = Sys.b();
	if (data2 == 0) {
		goto Ret;
	}
	Sys.a = data1 / data2;
	Sys.b = data1 % data2;
	Sys.psw.SetOv(0);
	Ret: Sys.psw.SetCy(0);
	Sys.PC++;
}

void CVirMcu::Opcode_A4_MUL_AB(void) {
	//LEARN here have many thing to do

	uint64_t a = Sys.a();
	uint64_t b = Sys.b();
	a = b * a;

	Sys.a = 0;
	Sys.b = a;
	Sys.PC++;

// temp=(INT16U)Sys.a()*(INT16U)Sys.b();
// Sys.psw().ov=(temp>255)?1:0;
// Sys.a = (INT8U)temp;
// Sys.b = (INT8U)(temp>>8);

}

void CVirMcu::Opcode_02_LJMP_Addr16(void) {
	Sys.PC = GetOpcodeData<unsigned short>();
}
void CVirMcu::Opcode_00_NOP(void) {
	Sys.PC++;
}

void CVirMcu::Opcode_12_LCALL_Addr16(void) {
	unsigned short addr = 0;
	addr = GetOpcodeData<unsigned short>();
	Sys.PC += 3;
	Push(Sys.PC);
	Sys.PC = addr;
}

void CVirMcu::Opcode_05_INC_Direct(void) {
	unsigned short addr = 0;
	addr = GetOpcodeData<unsigned short>();
	m_ChipRam[addr]++;
	Sys.PC += 3;
}

void CVirMcu::Opcode_15_DEC_Direct(void) {
	unsigned short addr = 0;
	addr = GetOpcodeData<unsigned short>();
	m_ChipRam[addr]--;
	Sys.PC += 3;
}

void CVirMcu::MD_ADD(uint64_t data) {
	uint64_t a = Sys.a();
	uint64_t tep = a + data;
	uint64_t tep2 = 0;
	int flagAC = 0;
	Sys.psw.SetAc((a & 0xffffffff) + (data & 0xffffffff) > 0xffffffff);
	Sys.psw.SetCy((tep < a) || (tep < data));
	flagAC += Sys.psw.GetCy();
	tep2 = (a & 0x7fffffffffffffff) + (data & 0x7fffffffffffffff);
	flagAC += (tep2 > (a & 0x7fffffffffffffff));
	Sys.psw.SetOv(flagAC == 1 ? 1 : 0);
	Sys.a = tep;

}

void CVirMcu::Opcode_25_ADD_A_Direct(void) {
	unsigned short addr = 0;
	addr = GetOpcodeData<unsigned short>();
	MD_ADD(m_ChipRam[addr]);
	Sys.PC += 3;
}

void CVirMcu::MD_SUBB(uint64_t data) {
	uint64_t tepa = Sys.a();
	data += Sys.psw.GetCy();
	Sys.psw.SetAc((tepa & 0xFFFFFFFF) < (data & 0xFFFFFFFF));
	Sys.psw.SetCy(tepa < data);
	int flagac = 0;
	if (tepa < data) {
		flagac++;
	}

	if ((tepa & 0xFFFFFFFF) < (data & 0xFFFFFFFF)) {
		flagac++;
	}
	Sys.psw.SetOv(flagac == 1);
	Sys.a = (tepa - data);

}
void CVirMcu::Opcode_94_SUBB_A_Data(void) {
	uint64_t data = GetOpcodeData<uint64_t>();
	MD_SUBB(data);
	Sys.PC += (1 + sizeof(uint64_t));
}
void CVirMcu::Opcode_95_SUBB_A_Direct(void) {
	uint64_t addr = GetOpcodeData<uint64_t>();
	MD_SUBB(m_ChipRam[addr]);
	Sys.PC += (1 + sizeof(uint64_t));
}
int CVirMcu::Run() {
	while (1) {
		unsigned char opcode = GetOpCode();
		StepRun(opcode);
	}
	return 1;
}
void CVirMcu::StepRun(unsigned char code) {
//	INT8U tempa = Sys.a();
	switch (code) {
	case 0x00: {
		Opcode_00_NOP();
		break;
	}
	case 0x02: {
		Opcode_02_LJMP_Addr16();
		break;
	}
	case 0x05: {
		Opcode_05_INC_Direct();
		break;
	}
	case 0x12: {
		Opcode_12_LCALL_Addr16();
		break;
	}
	case 0x15: {
		Opcode_15_DEC_Direct();
		break;
	}
	case 0x24: {
		Opcode_24_ADD_A_Data();
		break;
	}
	case 0x25: {
		Opcode_25_ADD_A_Direct();
		break;
	}

	case 0x94: {
		Opcode_94_SUBB_A_Data();
		break;
	}
	case 0x95: {
		Opcode_95_SUBB_A_Direct();
		break;
	}

	default:
		break;
	}
//	if (tempa != Sys.a())
//			{
////               UpDataPFlag();
//	}
//    UpDataDebugInfo();

}

template<class T>
int64_t& CUPReg<T>::GetRegRe() {
	return pmcu->m_ChipRam[m_Addr];
}

template<class T>
CUPReg<T>& CUPReg<T>::operator >>=(int data) {
	(GetRegRe() >>= data);
	return *this;
}
template<class T>
CUPReg<T>& CUPReg<T>::operator |=(int data) {
	(GetRegRe() |= data);
	return *this;
}

template<class T>
CUPReg<T>& CUPReg<T>::operator |=(const T& data) {
	(GetRegRe() |= data());
	return *this;
}

template<class T>
T CUPReg<T>::operator >>(int data) {
	return (GetRegRe() >> data);
}
template<class T>
T CUPReg<T>::operator &(int data) {
	return (GetRegRe() & data);
}
template<class T>
CUPReg<T>& CUPReg<T>::operator &=(int data) {
	GetRegRe() = (GetRegRe() & data);
	return *this;
}
template<class T>
T CUPReg<T>::operator <<(int data) {
	return ((GetRegRe() << data));
}
template<class T>
CUPReg<T>& CUPReg<T>::operator <<=(int data) {
	GetRegRe() = (GetRegRe() << data);
	return *this;
}
template<class T>
CUPReg<T>& CUPReg<T>::operator ^=(int data) {
	(GetRegRe() ^ data);
	return *this;
}
template<class T>
CUPReg<T>& CUPReg<T>::operator ^=(T& data) {
	(GetRegRe() ^ (data()));
	return *this;
}
template<class T>
CUPReg<T>& CUPReg<T>::operator ~() {
	GetRegRe() = (~(GetRegRe()));
	return *this;
}
template<class T>
CUPReg<T>& CUPReg<T>::operator +=(T& data) {
	(GetRegRe() = data() + GetRegRe());
	return *this;
}
template<class T>
T CUPReg<T>::operator +(T& data) {
	return (GetRegRe() + data());
}
template<class T>
T CUPReg<T>::operator +(int data) {
	return (GetRegRe() + data);
}
template<class T>
CUPReg<T>& CUPReg<T>::operator +=(int data) {
	GetRegRe() = GetRegRe() + data;
	return *this;
}
template<class T>
T CUPReg<T>::operator -(int data) {
	return (GetRegRe() - data);
}
template<class T>
T CUPReg<T>::operator-(T &data) {
	return (GetRegRe() - data());
}
template<class T>
CUPReg<T>& CUPReg<T>::operator -=(int data) {
	GetRegRe() = (GetRegRe() - data);
	return *this;
}
template<class T>
uint64_t CUPReg<T>::operator()(void) {
	return pmcu->m_ChipRam[m_Addr];
}

template<class T>
CUPReg<T>& CUPReg<T>::operator=(uint64_t &data) {
	pmcu->m_ChipRam[m_Addr] = data;
	return *this;
}

//template<class T>
//CUPReg<T>& CUPReg<T>:: operator=(T& data){
//	pmcu->m_ChipRam[m_Addr]= data;
//	return *this;
//	//return pmcu->m_ChipRam[m_Addr]= data();
//}
template<class T>
CUPReg<T>& CUPReg<T>::operator=(int data) {
	pmcu->m_ChipRam[m_Addr] = data;
	return *this;
}
template<class T>
CUPReg<T>& CUPReg<T>::operator ++(int) {
	++(pmcu->m_ChipRam[m_Addr]);
	return *this;
}
template<class T>
CUPReg<T>& CUPReg<T>::operator --() {
	--pmcu->m_ChipRam[m_Addr];
	return *this;
}

template<class T>
bool CUPReg<T>::operator !=(int data) {
	return pmcu->m_ChipRam[m_Addr] == data;
}
template<class T>
bool CUPReg<T>::operator <(int data) {
	return pmcu->m_ChipRam[m_Addr] < data;
}
template<class T>
bool CUPReg<T>::operator==(int data) {
	return (GetRegRe() == data);
}
template<class T>
bool CUPReg<T>::operator !=(T& data) {
	return pmcu->m_ChipRam[m_Addr] != data();
}
template<class T>
bool CUPReg<T>::operator <(T& data) {
	return pmcu->m_ChipRam[m_Addr] < data();
}
template<class T>
bool CUPReg<T>::operator==(T& data) {
	return pmcu->m_ChipRam[m_Addr] == data();
}
