#ifndef YD_T_1363_3_2005_H
#define YD_T_1363_3_2005_H

#include <stdio.h>
#include <string.h>

//数据类型重命名
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

//是否开启协议调试
#define EN_DEBUG_YDT true
//SU使用还是SM使用
#define EN_SU true

/********************************设备类型********************************/
//蓄电池监测装置
#define YD_CID1_BATTERY 0x46
//直流电表
#define YD_CID1_DIRCURMETER 0xD2
/**********************************************************************/

/*****************************通信协议参数定义****************************/
//起始位标志
#define YD_SOI 0x7e
//结束码
#define YD_EOI 0x0d
//通信协议版本号
#define YD_VER 0x20
//协议未打包时的最大长度
#define YD_MAX_LEN_UNPACK 128
//协议打包后的最大长度
#define YD_MAX_LEN_PACK (2 * YD_MAX_LEN_UNPACK - 2)
//协议未打包时的最小长度
#define YD_MIN_LEN_UNPACK 10
//协议打包后的最小长度
#define YD_MIN_LEN_PACK (2 * YD_MIN_LEN_UNPACK - 2)
/**********************************************************************/

/*****************************CID2返回码RTN定义**************************/
//正常
#define YD_RTN_NORMAL 0x00
//VER错
#define YD_RTN_ERROR_VER 0x01
//CHKSUM错
#define YD_RTN_ERROR_CHKSUM 0x02
//LCHKSUM错
#define YD_RTN_ERROR_LCHKSUM 0x03
//CID2无效
#define YD_RTN_CID2_INVALID 0x04
//命令格式错
#define YD_RTN_ERROR_CMD 0x05
//无效数据
#define YD_RTN_DATA_INVALID 0x06
/**********************************************************************/

/****************************CID2命令信息编码定义*************************/
//获取模拟量量化后数据（浮点数）
#define YD_CID2_GET_FLOAT_DATA 0x41
//获取模拟量量化后数据（定点数）
#define YD_CID2_GET_INT_DATA 0x42
//获取开关输入状态
#define YD_CID2_GET_SWITCH_STATE 0x43
//获取告警状态
#define YD_CID2_GET_ALARM_STATE 0x44
//遥控
#define YD_CID2_REMOTE_CTRL 0x45
//获取系统参数（浮点数）
#define YD_CID2_GET_SYS_FLOAT_ARGS 0x46
//获取系统参数（定点数）
#define YD_CID2_GET_SYS_INT_ARGS 0x47
//设定系统参数（浮点数）
#define YD_CID2_SET_SYS_FLOAT_ARGS 0x48
//设定系统参数（定点数）
#define YD_CID2_SET_SYS_INT_ARGS 0x49
//*获取系统历史数据（浮点数）*
#define YD_CID2_GET_SYS_HISTORY_FLOAT 0x4a
//*获取系统历史数据（定点数）*
#define YD_CID2_GET_SYS_HISTORY_INT 0x4b
//*获取历史告警*
#define YD_CID2_GET_HISTORY_ALARM 0x4c
//*获取监测模块时间*
#define YD_CID2_GET_MODULE_TIME 0x4d
//*设定监测模块时间*
#define YD_CID2_SET_MODULE_TIME 0x4e
//获取通信协议版本号
#define YD_CID2_GET_VER 0x4f
//获取设备地址
#define YD_CID2_GET_ADR 0x50
//获取设备（监测模块）厂商信息
#define YD_CID2_GET_DEV_INFO 0x51
/**********************************************************************/

//协议结构体
typedef struct ydt136332005
{
    u8 SOI;
    u8 VER;
    u8 ADR;
    u8 CID1;
    u8 CID2;
    u8 LENGTH[2];
    u8 INFO[YD_MAX_LEN_UNPACK - YD_MIN_LEN_UNPACK];
    u8 CHKSUM[2];
    u8 EOI;
}st_ydt136332005, *pst_ydt136332005;

//协议结构体的长度
#define LEN_ST_YDT sizeof(st_ydt136332005)

class YD_T_1363_3_2005
{
public:
    YD_T_1363_3_2005();

    //协议结构体初始化
    static void stInit(pst_ydt136332005 pst_ydt, u8 ADR, u8 CID1, u8 CID2, u8 * INFO, u16 INFOlen);

    //打包协议结构体
    static void packProtocol(pst_ydt136332005 pst_ydt, u8 * protocol);

    //解包到协议结构体，成功返回0，失败返回相应的错误码
    static u8 unpackProtocol(pst_ydt136332005 pst_ydt, u8 * protocol);

    //获取协议结构体中的LENID
    //如果返回 < 0，则LCHKSUM错误
    static int getLENID(pst_ydt136332005 pst_ydt);

    //设置协议结构体中的LENGTH（包括LENID和LCHKSUM）
    static void setLENGTH(pst_ydt136332005 pst_ydt, u16 LENID);

    //计算并返回校验码
    static u16 calculateCHKSUM(u8 * data, u16 length);

    //获取协议结构体中的CHKSUM
    static u16 getCHKSUM(pst_ydt136332005 pst_ydt);

    //设置协议结构体中的CHKSUM
    static void setCHKSUM(pst_ydt136332005 pst_ydt);

    //将char[4]字节数组转float类型，char[0]存着float类型的低八位
    static float bytes2float(u8 * bytes);

    //将float类型转换为char[4]，char[0]存着float类型的低八位
    static void float2bytes(u8 * bytes, float f);

    //将u8[2]字节数组转short类型，u8[0]存着u16类型的高八位
    static u16 bytes2u16(u8 * bytes);

    //将u8[4]字节数组转u32类型，u8[0]存着u32类型的高八位
    static u32 bytes2u32(u8 * bytes);

    //将u16类型转换为u8[2]，u8[0]存着u16类型的高八位
    static void u162bytes(u8 * bytes, u16 s);

    //自定义的strncpy，解决strncpy遇0返回而没有拷贝到规定的长度
    static void my_strncpy(char * dest, char * src, size_t count);

    //将u8[7]时间数组，转为char*字符串"2017-05-20 10:51:30"
    static void time2str(u8 * time, char * time_str);

    #if EN_DEBUG_YDT
    static void testProtocol(void);
    #endif
};

#endif // YD_T_1363_3_2005_H
