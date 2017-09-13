
#pragma once


#include "toolbox_tl.h"
#include <cstdio>

char DecimalChar() ;

const wchar_t HexChar[256][8] = {L"0x00", L"0x01", L"0x02", L"0x03", L"0x04", L"0x05", L"0x06", L"0x07", L"0x08", L"0x09", L"0x0A", L"0x0B", L"0x0C", L"0x0D", L"0x0E", L"0x0F", L"0x10", L"0x11", L"0x12", L"0x13", L"0x14", L"0x15", L"0x16", L"0x17", L"0x18", L"0x19", L"0x1A", L"0x1B", L"0x1C", L"0x1D", L"0x1E", L"0x1F", L"0x20", L"0x21", L"0x22", L"0x23", L"0x24", L"0x25", L"0x26", L"0x27", L"0x28", L"0x29", L"0x2A", L"0x2B", L"0x2C", L"0x2D", L"0x2E", L"0x2F", L"0x30", L"0x31", L"0x32", L"0x33", L"0x34", L"0x35", L"0x36", L"0x37", L"0x38", L"0x39", L"0x3A", L"0x3B", L"0x3C", L"0x3D", L"0x3E", L"0x3F", L"0x40", L"0x41", L"0x42", L"0x43", L"0x44", L"0x45", L"0x46", L"0x47", L"0x48", L"0x49", L"0x4A", L"0x4B", L"0x4C", L"0x4D", L"0x4E", L"0x4F", L"0x50", L"0x51", L"0x52", L"0x53", L"0x54", L"0x55", L"0x56", L"0x57", L"0x58", L"0x59", L"0x5A", L"0x5B", L"0x5C", L"0x5D", L"0x5E", L"0x5F", L"0x60", L"0x61", L"0x62", L"0x63", L"0x64", L"0x65", L"0x66", L"0x67", L"0x68", L"0x69", L"0x6A", L"0x6B", L"0x6C", L"0x6D", L"0x6E", L"0x6F", L"0x70", L"0x71", L"0x72", L"0x73", L"0x74", L"0x75", L"0x76", L"0x77", L"0x78", L"0x79", L"0x7A", L"0x7B", L"0x7C", L"0x7D", L"0x7E", L"0x7F", L"0x80", L"0x81", L"0x82", L"0x83", L"0x84", 
 L"0x85", L"0x86", L"0x87", L"0x88", L"0x89", L"0x8A", L"0x8B", L"0x8C", L"0x8D", L"0x8E", L"0x8F", L"0x90", L"0x91", L"0x92", L"0x93", L"0x94", L"0x95", L"0x96", L"0x97", L"0x98", L"0x99", L"0x9A", L"0x9B", L"0x9C", L"0x9D", L"0x9E", L"0x9F", L"0xA0", L"0xA1", L"0xA2", L"0xA3", L"0xA4", L"0xA5", L"0xA6", L"0xA7", L"0xA8", L"0xA9", L"0xAA", L"0xAB", L"0xAC", L"0xAD", L"0xAE", L"0xAF", L"0xB0", L"0xB1", L"0xB2", L"0xB3", L"0xB4", L"0xB5", L"0xB6", L"0xB7", L"0xB8", L"0xB9", L"0xBA", L"0xBB", L"0xBC", L"0xBD", L"0xBE", L"0xBF", L"0xC0", L"0xC1", L"0xC2", L"0xC3", L"0xC4", L"0xC5", L"0xC6", L"0xC7", L"0xC8", L"0xC9", L"0xCA", L"0xCB", L"0xCC", L"0xCD", L"0xCE", L"0xCF", L"0xD0", L"0xD1", L"0xD2", L"0xD3", L"0xD4", L"0xD5", L"0xD6", L"0xD7", L"0xD8", L"0xD9", L"0xDA", L"0xDB", L"0xDC", L"0xDD", L"0xDE", L"0xDF", L"0xE0", L"0xE1", L"0xE2", L"0xE3", L"0xE4", L"0xE5", L"0xE6", L"0xE7", L"0xE8", L"0xE9", L"0xEA", L"0xEB", L"0xEC", L"0xED", L"0xEE", L"0xEF", L"0xF0", L"0xF1", L"0xF2", L"0xF3", L"0xF4", L"0xF5", L"0xF6", L"0xF7", L"0xF8", L"0xF9", L"0xFA", L"0xFB", L"0xFC", L"0xFD", L"0xFE", L"0xFF"} ;

const wchar_t HexCharXP[256][8] = {L"[00]", L"[01]", L"[02]", L"[03]", L"[04]", L"[05]", L"[06]", L"[07]", L"[08]", L"[09]", L"[0A]", L"[0B]", L"[0C]", L"[0D]", L"[0E]", L"[0F]", L"[10]", L"[11]", L"[12]", L"[13]", L"[14]", L"[15]", L"[16]", L"[17]", L"[18]", L"[19]", L"[1A]", L"[1B]", L"[1C]", L"[1D]", L"[1E]", L"[1F]", L"[20]", L"[21]", L"[22]", L"[23]", L"[24]", L"[25]", L"[26]", L"[27]", L"[28]", L"[29]", L"[2A]", L"[2B]", L"[2C]", L"[2D]", L"[2E]", L"[2F]", L"[30]", L"[31]", L"[32]", L"[33]", L"[34]", L"[35]", L"[36]", L"[37]", L"[38]", L"[39]", L"[3A]", L"[3B]", L"[3C]", L"[3D]", L"[3E]", L"[3F]", L"[40]", L"[41]", L"[42]", L"[43]", L"[44]", L"[45]", L"[46]", L"[47]", L"[48]", L"[49]", L"[4A]", L"[4B]", L"[4C]", L"[4D]", L"[4E]", L"[4F]", L"[50]", L"[51]", L"[52]", L"[53]", L"[54]", L"[55]", L"[56]", L"[57]", L"[58]", L"[59]", L"[5A]", L"[5B]", L"[5C]", L"[5D]", L"[5E]", L"[5F]", L"[60]", L"[61]", L"[62]", L"[63]", L"[64]", L"[65]", L"[66]", L"[67]", L"[68]", L"[69]", L"[6A]", L"[6B]", L"[6C]", L"[6D]", L"[6E]", L"[6F]", L"[70]", L"[71]", L"[72]", L"[73]", L"[74]", L"[75]", L"[76]", L"[77]", L"[78]", L"[79]", L"[7A]", L"[7B]", L"[7C]", L"[7D]", L"[7E]", L"[7F]", L"[80]", L"[81]", L"[82]", L"[83]", L"[84]", 
 L"[85]", L"[86]", L"[87]", L"[88]", L"[89]", L"[8A]", L"[8B]", L"[8C]", L"[8D]", L"[8E]", L"[8F]", L"[90]", L"[91]", L"[92]", L"[93]", L"[94]", L"[95]", L"[96]", L"[97]", L"[98]", L"[99]", L"[9A]", L"[9B]", L"[9C]", L"[9D]", L"[9E]", L"[9F]", L"[A0]", L"[A1]", L"[A2]", L"[A3]", L"[A4]", L"[A5]", L"[A6]", L"[A7]", L"[A8]", L"[A9]", L"[AA]", L"[AB]", L"[AC]", L"[AD]", L"[AE]", L"[AF]", L"[B0]", L"[B1]", L"[B2]", L"[B3]", L"[B4]", L"[B5]", L"[B6]", L"[B7]", L"[B8]", L"[B9]", L"[BA]", L"[BB]", L"[BC]", L"[BD]", L"[BE]", L"[BF]", L"[C0]", L"[C1]", L"[C2]", L"[C3]", L"[C4]", L"[C5]", L"[C6]", L"[C7]", L"[C8]", L"[C9]", L"[CA]", L"[CB]", L"[CC]", L"[CD]", L"[CE]", L"[CF]", L"[D0]", L"[D1]", L"[D2]", L"[D3]", L"[D4]", L"[D5]", L"[D6]", L"[D7]", L"[D8]", L"[D9]", L"[DA]", L"[DB]", L"[DC]", L"[DD]", L"[DE]", L"[DF]", L"[E0]", L"[E1]", L"[E2]", L"[E3]", L"[E4]", L"[E5]", L"[E6]", L"[E7]", L"[E8]", L"[E9]", L"[EA]", L"[EB]", L"[EC]", L"[ED]", L"[EE]", L"[EF]", L"[F0]", L"[F1]", L"[F2]", L"[F3]", L"[F4]", L"[F5]", L"[F6]", L"[F7]", L"[F8]", L"[F9]", L"[FA]", L"[FB]", L"[FC]", L"[FD]", L"[FE]", L"[FF]"} ;


//	Convert numerical values into strings
//	still in use by biokernel
//inline const char * ToString(char * dest, char x)							{	sprintf(dest, 256, "%c", x) ;			return(dest) ;				}
//inline const char * ToString(char * dest, wchar_t x)						{	sprintf(dest, 256, "%c", x) ;			return(dest) ;				}	//	%C and %c expect a Unicode char for swprintf
//inline const char * ToString(char * dest, unsigned char x)				{	sprintf(dest, 256, "%hc", x) ;			return(dest) ;				}
inline const char * ToString(char * dest, short x)							{	sprintf(dest, "%hi", x) ;				return(dest) ;				}
inline const char * ToString(char * dest, unsigned short x)				{	sprintf(dest, "%hu", x) ;				return(dest) ;				}
inline const char * ToString(char * dest, int32_t x)							{	sprintf(dest, "%i", x) ;				return(dest) ;				}
inline const char * ToString(char * dest, uint32_t x)						{	sprintf(dest, "%u", x) ;				return(dest) ;				}
inline const char * ToString(char * dest, int64_t x)						{	sprintf(dest, "%lld", x) ;			return(dest) ;				}
inline const char * ToString(char * dest, uint64_t x)						{	sprintf(dest, "%lld", x) ;			return(dest) ;				}
const char			* ToString(char * dest, double x, int32_t decimal = 3) ;

//	unicode versions
//inline const wchar_t * ToString(wchar_t * dest, char x)					{	swprintf(dest, 256, L"%c", x) ;		return(dest) ;				}
//inline const wchar_t * ToString(wchar_t * dest, wchar_t x)				{	swprintf(dest, 256, L"%c", x) ;		return(dest) ;				}	//	%C and %c expect a Unicode char for swprintf
//inline const wchar_t * ToString(wchar_t * dest, unsigned char x)		{	swprintf(dest, 256, L"%hc", x) ;		return(dest) ;				}
inline const char		* ToString02(char * dest, int32_t x)					{	sprintf(dest, "%02i", x) ;			return(dest) ;				}
inline const wchar_t * ToString(wchar_t * dest, int16_t x)					{	swprintf(dest, 256, L"%hi", x) ;			return(dest) ;				}
inline const wchar_t * ToString(wchar_t * dest, uint16_t x)					{	swprintf(dest, 256, L"%hu", x) ;			return(dest) ;				}
inline const wchar_t * ToString(wchar_t * dest, int32_t x)					{	swprintf(dest, 256, L"%i", x) ;			return(dest) ;				}
inline const wchar_t * ToString(wchar_t * dest, uint32_t x)					{	swprintf(dest, 256, L"%u", x) ;			return(dest) ;				}
inline const wchar_t * ToString02(wchar_t * dest, int32_t x)				{	swprintf(dest, 256, L"%02i", x) ;			return(dest) ;				}
inline const wchar_t * ToString02(wchar_t * dest, uint32_t x)				{	swprintf(dest, 256, L"%02u", x) ;			return(dest) ;				}
inline const wchar_t * ToString03(wchar_t * dest, int32_t x)				{	swprintf(dest, 256, L"%03i", x) ;			return(dest) ;				}

//	return the hex value with prefiw 0x : 15 = "0x0F"
inline const wchar_t * ToStringX(char c)											{	return(HexChar[static_cast<unsigned char>(c)]) ;											}
inline const wchar_t * ToStringX(unsigned char c)								{	return(HexChar[c]) ;																}
inline const wchar_t * ToStringX(wchar_t * dest, uint32_t x)				{	swprintf(dest, 256, L"0x%06X", x) ;		return(dest) ;					}	//	for COLORREF
//	return the hex value between parentheses : 15 = "[0F]", used for PLC communication
inline const wchar_t * ToStringXP(char c)											{	return(HexCharXP[static_cast<unsigned char>(c)]) ;										}
inline const wchar_t * ToStringXP(unsigned char c)								{	return(HexCharXP[c]) ;															}
inline const wchar_t * ToStringX(wchar_t * dest, int32_t x)					{	swprintf(dest, 256, L"0x%X", static_cast<uint32_t>(x)) ;	return(dest) ;	}
//inline const wchar_t * ToStringX(wchar_t * dest, uint32_t x)				{	swprintf(dest, 256, L"0x%X", x) ;			return(dest) ;					}
//	From Rgb() to RRGGBB in hexadecimal
inline const wchar_t * ToStringRgb(wchar_t * dest, uint32_t x)				{	swprintf(dest, 256, L"%02X%02X%02X", x & 0xFF, (x >> 8) & 0xFF, (x >> 16) & 0xFF) ;	return(dest) ;	}
inline const wchar_t * ToStringP(wchar_t * dest, const void * x)			{	swprintf(dest, 256, L"0x%p", x) ;			return(dest) ;					}
//inline const wchar_t * ToString(wchar_t * dest, int32_t x)				{	swprintf(dest, 256, L"%li", x) ;			return(dest) ;					}
//inline const wchar_t * ToString(wchar_t * dest, uint32_t x)				{	swprintf(dest, 256, L"%lu", x) ;			return(dest) ;					}
inline const wchar_t * ToString(wchar_t * dest, int64_t x)					{	swprintf(dest, 256, L"%I64d", x) ;		return(dest) ;					}
inline const wchar_t * ToString(wchar_t * dest, uint64_t x)					{	swprintf(dest, 256, L"%I64u", x) ;		return(dest) ;					}
inline const wchar_t * ToString(wchar_t * dest, float x)						{	swprintf(dest, 256, L"%E", x) ;			return(dest) ;					}


const wchar_t	*	ToString(wchar_t * dest, double x, int32_t decimal = 3) ;
const wchar_t	*	ToCleanString(wchar_t * dest, double x, int32_t decimal = 3) ;

wchar_t			*	CleanServiceName(wchar_t * p0) ;
wchar_t			*	CleanZero(wchar_t * srce) ;

//	******************************	Time utilities	************************************

double				MakeDuration(int32_t hour, int32_t minute, int32_t sec, int32_t milli) ;
double				ReadDuration(const wchar_t * TimeString) ;
wchar_t			*	ShowDuration(wchar_t * dest, double laps, bool viewMilli = false) ;
wchar_t			*	ShowRoundedDuration(wchar_t * dest, double laps) ;
wchar_t			*	ShowDayDuration(wchar_t * dest, double laps, int32_t language) ;

