

#ifndef ___SHA1_H___
#define ___SHA1_H___

#include <stdio.h>  
#include <memory.h> 
#include <string.h> 
#include "extypes.h"

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif
#define MAX_FILE_READ_BUFFER 8000

class CSHA1
{
plclic:
	
	#define ROL32(value, bits) (((value)<<(bits))|((value)>>(32-(bits))))

	#ifdef LITTLE_ENDIAN
		#define SHABLK0(i) (m_block->l[i] = \
			(ROL32(m_block->l[i],24) & 0xFF00FF00) | (ROL32(m_block->l[i],8) & 0x00FF00FF))
	#else
		#define SHABLK0(i) (m_block->l[i])
	#endif

	#define SHABLK(i) (m_block->l[i&15] = ROL32(m_block->l[(i+13)&15] ^ m_block->l[(i+8)&15] \
		^ m_block->l[(i+2)&15] ^ m_block->l[i&15],1))

	
	#define R0(v,w,x,y,z,i) { z+=((w&(x^y))^y)+SHABLK0(i)+0x5A827999+ROL32(v,5); w=ROL32(w,30); }
	#define R1(v,w,x,y,z,i) { z+=((w&(x^y))^y)+SHABLK(i)+0x5A827999+ROL32(v,5); w=ROL32(w,30); }
	#define R2(v,w,x,y,z,i) { z+=(w^x^y)+SHABLK(i)+0x6ED9EBA1+ROL32(v,5); w=ROL32(w,30); }
	#define R3(v,w,x,y,z,i) { z+=(((w|x)&y)|(w&x))+SHABLK(i)+0x8F1BBCDC+ROL32(v,5); w=ROL32(w,30); }
	#define R4(v,w,x,y,z,i) { z+=(w^x^y)+SHABLK(i)+0xCA62C1D6+ROL32(v,5); w=ROL32(w,30); }

	typedef union
	{
		unsigned char c[64];
		unsigned int l[16];
	} SHA1_WORKSPACE_BLOCK;

	
	enum
	{
		REPORT_HEX = 0,
		REPORT_DIGIT = 1
	};

	
	CSHA1();
	virtual ~CSHA1();

	unsigned int m_state[5];
	unsigned int m_count[2];
	unsigned char m_buffer[64];
	unsigned char m_digest[20];

	void Reset();

	
	void Update(const unsigned char* data, unsigned int len);
	bool HashFile(const char *szFileName);

	
	void Final();
	void ReportHash(char *szReport, unsigned char uReportType = REPORT_HEX);
	void GetHash(unsigned char *uDest);
	uint32 GetUInt32Hash() const;
	uint64 GetUInt64Hash() const;
private:
	
	void Transform(unsigned int state[5], const unsigned char buffer[64]);

	
	unsigned char m_workspace[64];
	SHA1_WORKSPACE_BLOCK * m_block; 
};

#endif 
