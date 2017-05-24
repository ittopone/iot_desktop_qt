#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

//否
#define IT_IS_FALSE 0x00
//是
#define IT_IS_TRUE 0x01
//未知
#define IT_IS_UNKNOW 0x02

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
