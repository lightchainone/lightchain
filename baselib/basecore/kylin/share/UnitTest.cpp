
#include "stdafx.h"
#include <iostream>
#include <utility>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <ctype.h>
#include "UnitTest.h"
#include "SyncObj.h"

using namespace std;

#ifndef FILENAME_MAX
#define FILENAME_MAX    8192
#endif

namespace unittest {

bool UNIT_TEST_ABORTED = false;
const int MAX_WAIT_TIME = 120;
string TEST_PROGRAM;
bool g_bTestInProcess = true;
bool g_bPrintOnAdd = true;
char g_ColorRed[16];
char g_ColorGreen[16];
char g_ColorEnd[16];

string GetEach(char* s, int& start) {
    string name;
    int len = 0;

    while (isdigit(s[start])) {
        len = len * 10 + (s[start] - '0');
        start++;
    }
    char bak = s[start+len];
    s[start+len] = 0;
    name = string(s+start);
    s[start+len] = bak;
    start += len;
    return name;
}

string GetNested(char* s, int& start) {
	string name = "";
	int count = 0;
	if (s[start] == 'N') {
		start++;
		while (isdigit(s[start])) {
			if (name.size() != 0) {
				name += "::";
			}
			name += GetEach(s, start);
			count++;
		}
		while (count--) {
			start++;
		}
	}
	else {
		name = GetEach(s, start);
		start++;
	}
	return name;
}

string GetFuncName(char* s) {
    string name = "";
    int len = strlen(s);
    int start = -1;
    for (int i=len-1; i>=0; --i) {
        if (s[i] == '+') {
            s[i] = 0;
        }
        else if (s[i] == '(') {
            start = i;
            break;
        }
    }
    if (start != -1) {
        if (s[start+1] == '_' && s[start+2] == 'Z') {
            start += 3;
            if (s[start] == 'N') {
                ++start;
                name += GetEach(s, start);
                while (isdigit(s[start]) || s[start] == 'I') {
                    if (isdigit(s[start])) {
                        name += "::";
                        name += GetEach(s, start);
                    }
                    else if (s[start] == 'I') {
                        start++;
                        name += "<";
                        name += GetNested(s, start);
                        name += ">";
                    }
                }
            }
            else {
                name += GetEach(s, start);
            }
        }
        else {
            name += string(s+start+1);
        }
    }
    return name;
}

string GetAddress(char* s) {
    string addr = "";
    int len = strlen(s);
    if (len > 4 && s[0] == '[' && s[len-1] == ']') {
        s[len-1] = 0;
        addr = string(s+3);
    }
    len = 16 - addr.size();
    for(int i=0; i<len; i++) {
        addr = "0" + addr;
    }
    return "0x" + addr;
}

string GetPosition(const char* fileName) {
	FILE* fp = fopen(fileName, "r");
	char s[1024];
	int len;

	if (fp != NULL) {
		if (fscanf(fp, "%s", s) != 0) {
			fclose(fp);
			len = strlen(s)-1;
			while (len >= 0 && s[len] != '/') {
				--len;
			}
			return string(s+len+1);
		}
		fclose(fp);
	}
	return "??:0";
}

void Exception::BackTrace(ostream& os) {
	void *bt[20];
	char **strings;
	size_t sz;
	char* p[2];
	int n;
	int idx = 0;
	string addr;
	char tmpFileName[32];
	snprintf(tmpFileName, 30, "%d.tmp", thread_getid());

	sz = backtrace(bt, 30);
	strings = backtrace_symbols(bt, sz);
	os<<"   StackTrace:"<<endl;
	for(uint32 i = 0; i < sz; ++i) {
		n = SplitString(strings[i], " ", p, 2, true);
		if (n == 2) {
			string name = GetFuncName(p[0]);
			if (name.size() != 0) {
				if (idx > 1) {
					addr = GetAddress(p[1]);
					system(("echo " + addr + " | addr2line -e "
						+ TEST_PROGRAM + " > " + string(tmpFileName)).c_str());
					os<<"   #"<<idx - 2<<" "<<addr<<" in "<<name<<" () at "
						<<GetPosition(tmpFileName)<<endl;
				}
				idx++;
			}
			if (name == "main") {
				break;
			}
		}
	}
	system(("rm -f " + string(tmpFileName)).c_str());
	free(strings);
}

Runner::Runner(const string& file, int len, const string& name) {
	m_sName = name;
    m_sFile = file;
    m_nLen = len;
    RunnerRegistry::Instance()->AddRunner(this);
}

Runner::~Runner() {
}




IMPLEMENT_SINGLETON_WITHOUT_CTOR(Environment)
Environment::Environment() {
    m_nCase = 0;
    m_bInChild = false;
}

void Environment::AddException(Exception* e) {
	spin_lock(&sm_lock);
	m_failures.push_back(make_pair(m_nCase, e));
	if (g_bPrintOnAdd) {
		e->Print(cout);
	}
	spin_unlock(&sm_lock);
}

void Environment::RemoveException(Exception* e) {
	spin_lock(&sm_lock);
	for (int i = 0; i < (int)m_failures.size(); ++i) {
		if (m_failures[i].second == e) {
			m_failures.erase(m_failures.begin() + i);
			break;
		}
	}
	spin_unlock(&sm_lock);
}

Exception* Environment::GetException() {
	Exception* e = NULL;

	spin_lock(&sm_lock);
	if (m_failures.size() > 0 && m_failures[m_failures.size()-1].first == m_nCase) {
		e = m_failures[m_failures.size()-1].second;
	}
	spin_unlock(&sm_lock);

	return e;
}

int Environment::PrintResult(vector<Runner*>& runners) {
	spin_lock(&sm_lock);
	if (m_failures.size()) {
		cout<<g_ColorRed<<m_failures.size()<<" FAILED"<<g_ColorEnd<<endl;
	} else {
		cout<<g_ColorGreen<<"ALL PASSED"<<g_ColorEnd<<endl;
	}
	int ret = (int)m_failures.size();
	for (int i = 0; i < (int)m_failures.size(); ++i) {
		int index = m_failures[i].first;
			<<":"<<runners[index]->len();
		m_failures[i].second->Print(cout);
		cout<<g_ColorEnd<<endl;
		delete m_failures[i].second;
	}
	spin_unlock(&sm_lock);
	return ret;
}

void Environment::PrintCase(int nCase) {
	spin_lock(&sm_lock);
	if (m_failures.size() > 0 && m_failures[m_failures.size()-1].first == nCase) {
		Exception* e = m_failures[m_failures.size()-1].second;
		e->Print(cout);
		cout<<nCase + 1<<") "<<g_ColorRed<<"Failed"<<g_ColorEnd<<endl<<endl;
	}
	else {
		cout<<nCase + 1<<") "<<g_ColorGreen<<"Pass"<<g_ColorEnd<<endl<<endl;
	}
	spin_unlock(&sm_lock);
}

int g_nChildPipe;

void SendException(Exception* e) {
	int8_t ret = 1;
	if (e == NULL) {
		write(g_nChildPipe, (char*)&ret, sizeof(int8_t));
	}
	else {
		ret = 2;
		write(g_nChildPipe, (char*)&ret, sizeof(int8_t));
		ostringstream stream;
		e->Print(stream);
		write(g_nChildPipe, (char*)stream.str().c_str(), stream.str().size());
		write(g_nChildPipe, "\0", 1);
		delete e;
	}
	close(g_nChildPipe);
}

void OnSignal(int signum)
{
	try {
		switch (signum) {
		case SIGSEGV:
			FAIL("Segmentation fault");
			break;
		case SIGABRT:
			Exception* e = Environment::Instance()->GetException();
			if (e == NULL) {
				FAIL("Aborted");
			}
			else {
				throw e;
			}
			break;
		}
	}
	catch(Exception* e) {
		SendException(e);
		exit(0);
	}
}




IMPLEMENT_SINGLETON_WITHOUT_CTOR(RunnerRegistry)
RunnerRegistry::RunnerRegistry() {
    m_pSetUp = NULL;
    m_pTearDown = NULL;
}

void RunnerRegistry::AddRunner(Runner* runner) {
    runners_.push_back(runner);
}

void RunnerRegistry::AddSetUp(TestFixture* pSetUp) {
	m_pSetUp = pSetUp;
}

void RunnerRegistry::AddTearDown(TestFixture* pTearDown) {
	m_pTearDown = pTearDown;
}

int RunnerRegistry::RunAll(char* pSingleCaseName) {
	int nCaseRunned = 0;

	if(m_pSetUp != NULL) {
		m_pSetUp->SetUp();
	}
	for (int i = 0; i < (int)runners_.size(); ++i) {
		if (NULL==pSingleCaseName
		    ||(NULL!=pSingleCaseName && runners_[i]->name()==string(pSingleCaseName))
        ) {
			cout<<"=============== "<<runners_[i]->name()<<" ==============="<<endl;
			try {																	
				++nCaseRunned;
				Environment::Instance()->SetCase(i);
				if (g_bTestInProcess || runners_[i]->testInProcess()) {
					pid_t pid;
					int pipe_fd[2];

					if (pipe(pipe_fd) < 0) {
						FAIL("Create pipe fail");
					}

					if ((pid = fork()) == 0) { 
						close(pipe_fd[0]);
						signal(SIGSEGV, OnSignal);
						signal(SIGABRT, OnSignal);
						g_nChildPipe = pipe_fd[1];
						Environment::Instance()->SetChild();

						int8_t ret = 1;
						try {
							runners_[i]->Run();
							Exception* e = Environment::Instance()->GetException();
							if (e == NULL) {
								write(pipe_fd[1], (char*)&ret, sizeof(int8_t));
							}
							else {
								throw e;
							}
						}
						catch(Exception* e) {
							ret = 2;
							write(pipe_fd[1], (char*)&ret, sizeof(int8_t));
							ostringstream stream;
							e->Print(stream);
							write(pipe_fd[1], (char*)stream.str().c_str(), stream.str().size());
							write(pipe_fd[1], "\0", 1);
						}
						close(pipe_fd[1]);
						exit(0);
					}
					else { 
						close(pipe_fd[1]);
						fcntl(pipe_fd[0], F_SETFL, O_NONBLOCK);
						int wait_time = 0;
						char rbuf[264];
						int8_t test_res = 0;
						ostringstream stream;
						int rnum = 0;
						while (wait_time < MAX_WAIT_TIME) {
							rnum = read(pipe_fd[0], rbuf, 256);
							rbuf[rnum] = 0;
							if (rnum > 0) {
								if (test_res == 0) {
									test_res = rbuf[0];
									if (test_res == 1) {
										break;
									}
									stream << rbuf + 1;
								}
								else {
									stream << rbuf;
								}
								if (rbuf[rnum - 1] == 0) {
									break;
								}
							}
							sleep(1);
							wait_time++;
						}
						close(pipe_fd[0]);
						if (wait_time == MAX_WAIT_TIME) {
							kill(pid, SIGTERM);
							FAIL("Run test case time out");
						}
						else if (test_res != 1) {
							Environment::Instance()->AddException(new Exception(stream.str()));
						}
					}
				}
				else {
					runners_[i]->Run();
				}										
			} catch (Exception* e) {
				
			}
			Environment::Instance()->PrintCase(i);
			cout<<endl;
		}
	}

    cout<<nCaseRunned<<" testcases. ";
	int ret = Environment::Instance()->PrintResult(runners_);
	if(m_pTearDown != NULL) {
		m_pTearDown->TearDown();
	}
	return ret;
}




IMPLEMENT_SINGLETON_WITHOUT_CTOR(ThreadTestUtil)
static CCond s_waitCond;

void ThreadTestUtil::Start(int count) {
    s_waitCond.Init(count);
}

void ThreadTestUtil::Wait() {
    s_waitCond.Wait();
}

void ThreadTestUtil::Complete(bool condition) {
    if (condition) {
        s_waitCond.Signal();
    }
}

void ThreadTestUtil::Abort() {
	if (Environment::Instance()->IsInChild()) {
		SendException(Environment::Instance()->GetException());
		_exit(0);
	}
	else {
		Complete(true);
		CCond stopCond;
		stopCond.Init(1);
		stopCond.Wait();
	}
}


static string GetWorkingDir() {
    char path[FILENAME_MAX + 1];
    if (getcwd(path, sizeof(path)) == NULL) {
        exit(1);
    }
    return path;
}

string CURRENT_WORKING_DIR = GetWorkingDir();
string TESTDIR = CURRENT_WORKING_DIR + "/testdata";

void PrintHelp() {
	printf("Usage: %s [Options...] TestCase\n", unittest::TEST_PROGRAM.c_str());
    printf("Options:\n");
    printf("  -p \tRun all test cases in new process.\n");
    printf("  -e \tPrint exception message when thrown.\n");
    printf("  -C \tPrint non-colored message.\n");
	printf("  -h \tPrint this help message.\n");
}

int RunTests(int argc, char* argv[]) {
	int ret = 0;
	TEST_PROGRAM = argv[0];
	char* pCase = NULL;

	g_bTestInProcess = false;
	g_bPrintOnAdd = false;
	strcpy(g_ColorRed, "\x1b[31m");
	strcpy(g_ColorGreen, "\x1b[32m");
	strcpy(g_ColorEnd, "\x1b[0m");
	for (int i=1; i<argc; ++i) {
		if (strcmp(argv[i], "-p") == 0) {
			g_bTestInProcess = true;
		}
		else if (strcmp(argv[i], "-e") == 0) {
			g_bPrintOnAdd = true;
		}
		else if (strcmp(argv[i], "-C") == 0) {
			g_ColorRed[0] = 0;
			g_ColorGreen[0] = 0;
			g_ColorEnd[0] = 0;
		}
		else if (strcmp(argv[i], "-h") == 0) {
			PrintHelp();
			return 0;
		}
		else if (pCase == NULL) {
			pCase = argv[i];
		}
		else {
			PrintHelp();
			return 0;
		}
	}

	ret = RunnerRegistry::Instance()->RunAll(pCase);
    delete RunnerRegistry::Instance();
    return ret;
}

}


