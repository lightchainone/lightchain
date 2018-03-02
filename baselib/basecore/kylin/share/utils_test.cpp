#include "stdafx.h"
#include "utils.h"
#include "UnitTest.h"

#define DECLARE_TEST_CLASS(testclassname) \
class testclassname { \
  plclic: \
    void SetUp() {} \
    void TearDown() {} \
}; 



DECLARE_TEST_CLASS(CNocaseCmpTest)

TEST_F(CNocaseCmpTest, Case1)
{
	CNocaseCmp m_CNocaseCmp;
	char *s1 = "abc";
	char *s2 = "ABCD";
	char *s3 = "";
	char *s4 = "ABC";
	char *s5 = "abbb";
	ASSERT(m_CNocaseCmp(s1, s2));
	ASSERT(m_CNocaseCmp(s3, s1));
	ASSERT(!m_CNocaseCmp(s1, s4));
	ASSERT(!m_CNocaseCmp(s1, s5));
}



DECLARE_TEST_CLASS(CCaseCmpTest)

TEST_F(CCaseCmpTest, Case1)
{
	CCaseCmp m_CCaseCmp;
	char *s1 = "ABC";
	char *s2 = "ABc";
	char *s3 = "";
	char *s4 = "ABCD";
	ASSERT(m_CCaseCmp(s1, s2));
	ASSERT(m_CCaseCmp(s3, s1));
	ASSERT(m_CCaseCmp(s1, s4));
}


DECLARE_TEST_CLASS(StrDirect2IntTest)

TEST_F(StrDirect2IntTest, Case1)
{
	char *s1 = "BIG5";
	char *s2 = "ADD";
	char *s3 = "BDDB_TEST";
	char *s4 = "";
	char *s5 = "AB";
	int actual_1, actual_2;
	int actual_3, actual_4;
	int actual_5;
	int expected_1 = 'BIG5';
	int expected_2 = 'ADD\0';
	int expected_3 = 'BDDB';
	int expected_4 = '\0\0\0\0';
	int expected_5 = 'AB\0\0';

	actual_1 = StrDirect2Int(s1, sizeof(int));
	actual_2 = StrDirect2Int(s2, sizeof(int));
	actual_3 = StrDirect2Int(s3, 6); 
	actual_4 = StrDirect2Int(s4, sizeof(int)); 
	actual_5 = StrDirect2Int(s5, sizeof(int)); 

	ASSERT_EQUAL(expected_1, actual_1);
    ASSERT_EQUAL(expected_2, actual_2);
	ASSERT_EQUAL(expected_3, actual_3);
	ASSERT_EQUAL(expected_4, actual_4);
	ASSERT_EQUAL(expected_5, actual_5);
}



DECLARE_TEST_CLASS(xfgetsTest)

TEST_F(xfgetsTest, Case1)
{
	int m_Fd[2];
	FILE *m_fp;
	char buf[256];
	int buf_size = 256;
	char *p_rtn = NULL;
	char *m_Con = "work\nlook\nixintui\ncheer\ngina";
	char *m_Set1 = "agcwl";
	if( pipe(m_Fd) < 0 )
		FAIL("pipe(fd) failed.\n");

	write(m_Fd[1], m_Con, strlen(m_Con));
	m_fp = fdopen(m_Fd[0],"r");

	if( (p_rtn = xfgets(buf, buf_size, m_fp, m_Set1)) == NULL)
	{
		close(m_Fd[0]);
		close(m_Fd[1]);
		FAIL("xfgets() failed.\n");
	}
	close(m_Fd[0]);
	close(m_Fd[1]);
	char *expected = "ixintui\n";
	ASSERT_EQUAL(strcmp(expected, p_rtn), 0);
}




class SplitBufTest{ 
plclic: 
    void SetUp() {
		rtn = 0;
	} 
    void TearDown() {} 
protected:
	uint8 m_Buf[256];
	const static int pElements_size = 5;
	uint8 *pElements[pElements_size];
	int rtn;
}; 


TEST_F(SplitBufTest, Case1)
{
	char *m_str1 = "aaa*bbb*ccc";
	char c1 = '*';
	memcpy(m_Buf, m_str1, strlen(m_str1));

	rtn = SplitBuf(m_Buf, strlen(m_str1), c1, pElements, pElements_size);

	ASSERT(rtn == 3);
	ASSERT(pElements[0] == &m_Buf[0]);
	ASSERT(pElements[1] == &m_Buf[4]);
	ASSERT(pElements[2] == &m_Buf[8]);
}



TEST_F(SplitBufTest, Case2)
{
	char *m_str2 = "******";	
	char c2 = '*';
	memcpy(m_Buf, m_str2, strlen(m_str2));

	rtn = SplitBuf(m_Buf, strlen(m_str2), c2, pElements, pElements_size);

	ASSERT(rtn == 5);
	ASSERT(pElements[0] == &m_Buf[0]);
	ASSERT(pElements[1] == &m_Buf[1]);
	ASSERT(pElements[2] == &m_Buf[2]);
	ASSERT(pElements[3] == &m_Buf[3]);
	ASSERT(pElements[4] == &m_Buf[4]);
}


TEST_F(SplitBufTest, Case3)
{
	char c3 = 'k';
	memcpy(m_Buf, m_str3, strlen(m_str3));
	rtn = SplitBuf(m_Buf, strlen(m_str3), c3, pElements, pElements_size);
	ASSERT(rtn == 1);
}


TEST_F(SplitBufTest, Case4)
{
	char *m_str4 = "aa*bb*";
	char c4 = '*';
	memcpy(m_Buf, m_str4, strlen(m_str4));
	rtn = SplitBuf(m_Buf, strlen(m_str4), c4, pElements, pElements_size);
	ASSERT(rtn = 2);
	ASSERT(pElements[0] == &m_Buf[0]);
	ASSERT(pElements[1] == &m_Buf[3]);
}


class SplitStringTest{ 
plclic: 
    void SetUp() {
		rtn = 0;
	} 
    void TearDown() {} 
protected:
	char m_Buf[256];
	const static int pElements_size = 5;
	char *pElements[pElements_size];
	int rtn;
}; 



TEST_F(SplitStringTest, Case1)
{
	char *m_str1 = "aaa*bbb*ccc";
	char c1 = '*';
	memcpy(m_Buf, m_str1, strlen(m_str1)+1);

	rtn = SplitString(m_Buf, c1, pElements, pElements_size);

	ASSERT(rtn == 3);
	ASSERT(pElements[0] == &m_Buf[0]);
	ASSERT(pElements[1] == &m_Buf[4]);
	ASSERT(pElements[2] == &m_Buf[8]);
}



TEST_F(SplitStringTest, Case2)
{
	char *m_str2 = "******";	
	char c2 = '*';
	memcpy(m_Buf, m_str2, strlen(m_str2)+1);

	rtn = SplitString(m_Buf, c2, pElements, pElements_size);

	ASSERT(rtn == 5);
	ASSERT(pElements[0] == &m_Buf[0]);
	ASSERT(pElements[1] == &m_Buf[1]);
	ASSERT(pElements[2] == &m_Buf[2]);
	ASSERT(pElements[3] == &m_Buf[3]);
	ASSERT(pElements[4] == &m_Buf[4]);
}


TEST_F(SplitStringTest, Case3)
{
	char c3 = 'k';
	memcpy(m_Buf, m_str3, strlen(m_str3)+1);
	rtn = SplitString(m_Buf, c3, pElements, pElements_size);
	ASSERT(rtn == 1);
}


TEST_F(SplitStringTest, Case4)
{
	char *m_str4 = "aa*bb*";
	char c4 = '*';
	memcpy(m_Buf, m_str4, strlen(m_str4)+1);
	rtn = SplitString(m_Buf, c4, pElements, pElements_size);
	ASSERT(rtn = 2);
	ASSERT(pElements[0] == &m_Buf[0]);
	ASSERT(pElements[1] == &m_Buf[3]);
}



TEST_F(SplitStringTest, Case5)
{
	char *m_str5 = "ibbc";
	char *m_s5 = "ab";

	strncpy(m_Buf, m_str5, strlen(m_str5)+1);

	rtn = SplitString(m_Buf, m_s5, pElements, pElements_size, true);

	ASSERT(rtn == 2);
	ASSERT(pElements[0] == &m_Buf[0]);
	ASSERT(pElements[1] == &m_Buf[3]);
}



TEST_F(SplitStringTest, Case6)
{
	char *m_str6 = "ibbc";
	char *m_s6 = "ab";
	strncpy(m_Buf, m_str6, strlen(m_str6)+1);
	
	rtn = SplitString(m_Buf, m_s6, pElements, pElements_size, false);

	ASSERT(rtn == 3);
	ASSERT(pElements[0] == &m_Buf[0]);
	ASSERT(pElements[1] == &m_Buf[2]);
	ASSERT(pElements[2] == &m_Buf[3]);
}



TEST_F(SplitStringTest, Case7)
{
	char *m_str7 = "***$$$";
	char *m_s7 = "*$";
	strncpy(m_Buf, m_str7, strlen(m_str7)+1);
	
	rtn = SplitString(m_Buf, m_s7, pElements, pElements_size, false);

	ASSERT(rtn == 5);
	ASSERT(pElements[0] == &m_Buf[0]);
	ASSERT(pElements[1] == &m_Buf[1]);
	ASSERT(pElements[2] == &m_Buf[2]);
	ASSERT(pElements[3] == &m_Buf[3]);
	ASSERT(pElements[4] == &m_Buf[4]);
}



TEST_F(SplitStringTest, Case8)
{
	char *m_str8 = "";
	char *m_s8 = "";
	strncpy(m_Buf, m_str8, strlen(m_str8)+1);
	rtn = SplitString(m_Buf, m_s8, pElements, pElements_size, false);
	ASSERT(rtn == 1);
}



class SplitStringByTokenTest{ 
plclic: 
    void SetUp() {
		rtn = 0;
	} 
    void TearDown() {} 
protected:
	char m_Buf[256];
	const static int pElements_size = 5;
	char *pElements[pElements_size];
	int rtn;
}; 



TEST_F(SplitStringByTokenTest, Case1)
{
	char *m_str1 = "cccabdddabeee";
	char *token1 = "ab";
	strncpy(m_Buf, m_str1, strlen(m_str1)+1);

	rtn = SplitStringByToken(m_Buf, token1, pElements, pElements_size);
	
	ASSERT(rtn == 3);
	ASSERT(pElements[0] == &m_Buf[0]);
	ASSERT(pElements[1] == &m_Buf[5]);
	ASSERT(pElements[2] == &m_Buf[10]);
}



TEST_F(SplitStringByTokenTest, Case2)
{
	char *m_str2 = "cccababddd";
	char *token2 = "ab";
	strncpy(m_Buf, m_str2, strlen(m_str2)+1);
	
	rtn = SplitStringByToken(m_Buf, token2, pElements, pElements_size);

	ASSERT(rtn == 3);
	ASSERT(pElements[0] == &m_Buf[0]);
	ASSERT(pElements[1] == &m_Buf[5]);
	ASSERT(pElements[2] == &m_Buf[7]);
}



TEST_F(SplitStringByTokenTest, Case3)
{
	char *m_str3 = "abccab";
	char *token3 = "ab";
	strncpy(m_Buf, m_str3, strlen(m_str3)+1);

	rtn = SplitStringByToken(m_Buf, token3, pElements, pElements_size);

	ASSERT(rtn == 3);
	ASSERT(pElements[0] == &m_Buf[0]);
	ASSERT(pElements[1] == &m_Buf[2]);
	ASSERT(pElements[2] == &m_Buf[6]);
}



TEST_F(SplitStringByTokenTest, Case4)
{
	char *m_str4 = "";
	char *token4 = "";
	strncpy(m_Buf, m_str4, strlen(m_str4)+1);

	rtn = SplitStringByToken(m_Buf, token4, pElements, pElements_size);

	ASSERT(rtn == 1);
	ASSERT(pElements[0] == &m_Buf[0]);
}


DECLARE_TEST_CLASS(SetBitsTest)

 
TEST_F(SetBitsTest, Case1)
{
	uint64 pOld_1 = 0;
	uint64 pMod_1 = 0x00000000000000CD;
	uint64 expected_1 = 0x3340000000000000;
	SetBits((char *)&pOld_1, (char *)&pMod_1, 54, 8);
	ASSERT(pOld_1 == expected_1);
}



TEST_F(SetBitsTest, Case2)
{
	uint8 pOld_2 = 0x0C;
	uint8 pMod_2 = 0x0C;
	uint8 expected_2 = 0x30;
	SetBits((char *)&pOld_2, (char *)&pMod_2, 2, 4);
	ASSERT(pOld_2 == expected_2);
}



DECLARE_TEST_CLASS(GetBitsTest)

TEST_F(GetBitsTest, Case1)
{
	char pData[10];
	pData[8] = 192;
	pData[9] = 2;
	ASSERT(GetBits(pData, 70, 4) == 11);
}



DECLARE_TEST_CLASS(SetBytesTest)



TEST_F(SetBytesTest, Case1)
{

	char pOld[10];
	char *pMod = "abcde1234";

	memset(pOld, 0, 10);
	SetBytes(pOld, pMod, 0, 1);
	ASSERT(pOld[0] == *pMod);
}



TEST_F(SetBytesTest, Case2)
{
	char pOld[10];
	char *pMod = "abcde1234";

	memset(pOld, 0, 10);
	SetBytes(pOld, pMod, 2, 5);
	ASSERT(pOld[2] == *pMod);
	ASSERT(pOld[3] == *(pMod+1));
	ASSERT(pOld[4] == *(pMod+2));
	ASSERT(pOld[5] == *(pMod+3));
	ASSERT(pOld[6] == *(pMod+4));
}




DECLARE_TEST_CLASS(ParseNumTest)



TEST_F(ParseNumTest, Case1)
{
	
	char *dec = "10486";
	const uint64 dec_expected = 10486;
	uint64 dec_actual = 0;
	ASSERT(ParseNum(dec, &dec_actual));
	ASSERT_EQUAL(dec_expected, dec_actual);

	
	const int nHexs = 7;
	char hex[][16] = {"10486F"  , "10486f",
	                 "X10486F" , "x10486f",
	                 "0x10486f", "0X10486F",
	                 "x10486F" };
	const uint64 hex_expected = 1067119;
	uint64 hex_actual = 0;
	for(int i = 0; i < nHexs; ++i)
	{
		hex_actual = 0;
		ASSERT(ParseNum(hex[i], &hex_actual));
		ASSERT_EQUAL(hex_expected, hex_actual);
	}

	
	char *octal = "O104";
	const uint64 oct_expected = 0;
	uint64 oct_actual = 1;
	ASSERT(!ParseNum(octal, &oct_actual));
	ASSERT_EQUAL(oct_expected, oct_actual);

}


DECLARE_TEST_CLASS(StrToLowerTest)


TEST_F(StrToLowerTest, Case1)
{
	const char *pFrom = "0X10486 Hello World!";
	const char *expected = "0x10486 hello world!";
	char pTo[64] = "\0";
	int nLen;
	const int expected_len = strlen(pFrom);
	ASSERT(StrToLower(pTo, pFrom, &nLen) != NULL);




	ASSERT((!strcmp(pTo, expected)));
	ASSERT_EQUAL(nLen, expected_len);

}
