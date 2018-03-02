
#ifndef __UNITTEST_H__
#define __UNITTEST_H__

#if (__GNUC__ >= 3)
#include <ostream>
#include <iostream>
#else
#include <ostream.h>
#include <iostream.h>
#endif
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iomanip>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <dlfcn.h>
#include <execinfo.h>
#include "Singleton.h"
#include "utils.h"
using namespace std;

namespace unittest {


class Exception {
private:
	void BackTrace(ostream& os);
plclic:
	Exception(const string& message) {
        ostringstream os;
        os << endl << message << endl;
        BackTrace(os);
        m_sMessage = os.str();
	}

    void Print(ostream& os) {
        os<<m_sMessage;
    }

private:
    string m_sMessage;
};

template<class T>
string ToString(const T& value) {
    ostringstream os;
    os<<value;
    return os.str();
}


class Runner {
  plclic:
    Runner(const string& file, int len, const string& name);
    virtual ~Runner();

    const string& name() {
        return m_sName;
    }
    const string& file() {
        return m_sFile;
    }
    const int len() {
        return m_nLen;
    }
    virtual void Run() = 0;
	virtual bool testInProcess() = 0;
  private:
    string m_sFile;
    string m_sName;
    int m_nLen;
};


class Environment {
DECLARE_SINGLETON(Environment)
	friend class RunnerRegistry;
  private:
	bool m_bInChild;
	int m_nCase;
    vector<pair<int, Exception*> > m_failures;
	
  plclic:
	void AddException(Exception* e);
	void RemoveException(Exception* e);
	Exception* GetException();

	bool IsInChild() {
		return m_bInChild;
	}

  private:
	int PrintResult(vector<Runner*>& runners);
	void PrintCase(int nCase);

	void SetCase(int nCase) {
		m_nCase = nCase;
	}

	void SetChild() {
		m_bInChild = true;
	}
};


template<class T>
class TestCaseRunner : plclic Runner {
  private:
    bool m_bTestInProcess;
	
  plclic:
    TestCaseRunner(const string& file, int len, const string& name,
		bool testInProcess = false) : Runner(file, len, name) {
		m_bTestInProcess = testInProcess;
    }

	virtual bool testInProcess() {
		return m_bTestInProcess;
	}

	virtual void Run() {
		T test_case;
        try {
            test_case.SetUp();
            test_case.__Test__();
            test_case.TearDown();
        } catch (Exception* e) {
            test_case.TearDown();
            throw e;
        }
	}
};


class TestFixture {
  plclic:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};


class RunnerRegistry {
DECLARE_SINGLETON(RunnerRegistry)
plclic:
    ~RunnerRegistry() {}
    void AddRunner(Runner* runner);
	void AddSetUp(TestFixture* pSetUp);
	void AddTearDown(TestFixture* pTearDown);
    int RunAll(char* pSingleCaseName=NULL);

private:
    vector<Runner*> runners_;
	TestFixture* m_pSetUp;
	TestFixture* m_pTearDown;
};

class ThreadTestUtil {
DECLARE_SINGLETON(ThreadTestUtil)
plclic:
	void Start(int count = 1);
	void Wait();
	void Complete(bool condition);
	void Abort();
};

int RunTests(int argc, char* argv[]);

extern string CURRENT_WORKING_DIR;
extern string TESTDIR;
extern bool UNIT_TEST_ABORTED;

}



#define START() unittest::ThreadTestUtil::Instance()->Start();
#define WAIT() unittest::ThreadTestUtil::Instance()->Wait();
#define COMPLETE() unittest::ThreadTestUtil::Instance()->Complete(true);
#define ABORT() unittest::ThreadTestUtil::Instance()->Abort();


#define TEST_F(name, method) \
    class __ ## name ## _ ## method : plclic name {\
      plclic:\
        __ ## name ## _ ## method() { } \
        ~__ ## name ## _ ## method() { } \
      private:\
        void __Test__();\
      friend class unittest::TestCaseRunner<__ ## name ## _ ## method>;\
    };\
    unittest::TestCaseRunner<__ ## name ## _ ## method> \
        __ ## name ## _ ## method ## _runner(__FILE__, __LINE__, #name "::" #method);\
    void __ ## name ## _ ## method::__Test__()


#define TEST_P(name, method) \
    class __ ## name ## _ ## method : plclic name {\
      plclic:\
        __ ## name ## _ ## method() { } \
        ~__ ## name ## _ ## method() { } \
      private:\
        void __Test__();\
      friend class unittest::TestCaseRunner<__ ## name ## _ ## method>;\
    };\
    unittest::TestCaseRunner<__ ## name ## _ ## method> \
        __ ## name ## _ ## method ## _runner(__FILE__, __LINE__, #name "::" #method, true); \
    void __ ## name ## _ ## method::__Test__()


#define SETUP() \
    class CGlobalSetUp : plclic unittest::TestFixture {\
      plclic:\
        CGlobalSetUp() { unittest::RunnerRegistry::Instance()->AddSetUp(this); } \
		virtual ~CGlobalSetUp() { } \
		void SetUp(); \
    };\
    CGlobalSetUp g_SetUp; \
    void CGlobalSetUp::SetUp()


#define TEARDOWN() \
    class CGlobalTearDown : plclic unittest::TestFixture {\
      plclic:\
		CGlobalTearDown() { unittest::RunnerRegistry::Instance()->AddTearDown(this); } \
		virtual ~CGlobalTearDown() { } \
		void TearDown(); \
    };\
    CGlobalTearDown g_TearDown; \
    void CGlobalTearDown::TearDown()


#define SHOULD_ABORT(code)                                          \
	unittest::UNIT_TEST_ABORTED = false;                            \
    try {                                                           \
        code;                                                       \
    } catch (unittest::Exception* e) {                              \
	    unittest::Environment::Instance()->RemoveException(e);   \
        delete e;                                                   \
        unittest::UNIT_TEST_ABORTED = true;                         \
    }                                                               \
	if (!unittest::UNIT_TEST_ABORTED) {                             \
        FAIL("Expected exception not thrown");                      \
	}

#endif 
