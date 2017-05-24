#include "global.h"
#include <QTime>
#include <QCoreApplication>

/******************************全局变量初始化**************************************/
//软件是否已经注册
bool Global::m_isRegister = false;
/********************************************************************************/

Global::Global()
{

}

//延时（延时期间可做其他事情）
void Global::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

}

//获取CPUID
void Global::getCPUID(QString & cpuid)
{
    unsigned long s1 = 0,s2 = 0;
    char szCpuId[1024] = {0};
    char p1[128] = {0};
    unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;
    char cpuID[20] = {0};

    #if 1
    asm volatile
    (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "0"(0)
    );
    snprintf(szCpuId, 5, "%s", (char *)&ebx);
    snprintf(szCpuId+4, 5, "%s", (char *)&edx);
    snprintf(szCpuId+8, 5, "%s", (char *)&ecx);
    #endif

    asm volatile
    (
        "movl $0x01 , %%eax ; \n\t"
        "xorl %%edx , %%edx ;\n\t"
        "cpuid ;\n\t"
        "movl %%edx ,%0 ;\n\t"
        "movl %%eax ,%1 ; \n\t"
        :"=m"(s1),"=m"(s2)
    );

    memset(cpuID,0,20);
    sprintf((char *)p1, "%08X%08X", s1, s2);
    snprintf(szCpuId+12, 20, "%s", (char *)p1);
    memcpy(cpuID,p1,17);

    cpuid.append(cpuID);
}
