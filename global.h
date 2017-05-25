#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

class Global
{
public:
    Global();
    //软件是否已经注册
    static bool m_isRegister;
    //获取CPUID
    static void getCPUID(QString & cpuid);
    //延时（延时期间可做其他事情）
    static void sleep(unsigned int msec);
};

#endif // GLOBAL_H
