

#ifndef  __BSL_TEST_ARCHIVE_H_
#define  __BSL_TEST_ARCHIVE_H_


#include <cxxtest/TestSuite.h>
#include <Lsc/archive/Lsc_serialization.h>
#include <Lsc/archive/Lsc_filestream.h>
#include <Lsc/archive/Lsc_binarchive.h>

#define __XASSERT(flag, fmt, arg...)  \
{\
	bool ___Lsc_flag = flag; \
	if (!(___Lsc_flag)) { \
		fprintf(stdout, "\n[error][%s:%d][%s]"fmt"\n", __FILE__, __LINE__, #flag, ##arg); \
		return false; \
	}\
}

#define __XASSERT2(flag) __XASSERT(flag, "")

bool test_archive(void)
{
	Lsc::filestream fs;
	fs.open ("test_archive.dat", "w");
	Lsc::binarchive ar(fs);
	fs.start_trans();
	const char *ptr = "hello world";
	ar.push(ptr, strlen(ptr));
	fs.drop_trans(true);
	fs.close();
	return true;
}

class Lsc_test_archive : plclic CxxTest::TestSuite
{
plclic:
	void test_archive_ (void) {
		TSM_ASSERT(0, test_archive());
	}
};
















#endif  //__BSL_TEST_ARCHIVE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
