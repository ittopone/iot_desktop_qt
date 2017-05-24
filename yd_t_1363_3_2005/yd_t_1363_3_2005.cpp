#include "yd_t_1363_3_2005.h"
#include <string.h>
#include <stdlib.h>
#include <QDebug>


YD_T_1363_3_2005::YD_T_1363_3_2005()
{

}

//协议结构体初始化
void YD_T_1363_3_2005::stInit(pst_ydt136332005 pst_ydt, u8 ADR, u8 CID1, u8 CID2, u8 * INFO, u16 INFOlen)
{
    pst_ydt->SOI = YD_SOI;
    pst_ydt->EOI = YD_EOI;
    pst_ydt->VER = YD_VER;
    pst_ydt->ADR = ADR;
    pst_ydt->CID1 = CID1;
    pst_ydt->CID2 = CID2;

    //填充INFO
    YD_T_1363_3_2005::my_strncpy((char*)pst_ydt->INFO, (char*)INFO, INFOlen);
    //填充LENGTH
    YD_T_1363_3_2005::setLENGTH(pst_ydt, INFOlen * 2);
    //填充CHKSUM
    YD_T_1363_3_2005::setCHKSUM(pst_ydt);
}

//打包协议
void YD_T_1363_3_2005::packProtocol(pst_ydt136332005 pst_ydt, u8 * protocol)
{
    u8 temp[3] = {0};
    u16 i = 0;
    u16 index = 0;

    //填充包头
    protocol[index++] = YD_SOI;

    for(i = 1; i <= (6 + YD_T_1363_3_2005::getLENID(pst_ydt) / 2); i++)
    {
        sprintf((char*)temp, "%02X", *((char*)pst_ydt + i) & 0xff);
        protocol[index++] = temp[0];
        protocol[index++] = temp[1];
    }

    //填充校验码
    for(i = (LEN_ST_YDT - 3); i < (LEN_ST_YDT - 1); i++)
    {
        sprintf((char*)temp, "%02X", *((char*)pst_ydt + i) & 0xff);
        protocol[index++] = temp[0];
        protocol[index++] = temp[1];
    }

    //填充包尾
    protocol[index++] = YD_EOI;

    //字符串结束符
    protocol[index] = '\0';

    #if EN_DEBUG_YDT
    printf("packProtocol:");
    for(i = 0; i < strlen((char*)protocol); i++)
    {
        printf(" %02X", protocol[i] & 0xff);
    }
    printf("\r\n");
    fflush(stdout);
    #endif
}

//解包协议，成功返回0，失败返回相应的错误码
u8 YD_T_1363_3_2005::unpackProtocol(pst_ydt136332005 pst_ydt, u8 * protocol)
{
    u8 temp[3] = {0};
    int i = 0, LENID = 0;
    u16 CHKSUM = 0;
    u16 length = (u16)strlen((char*)protocol);

    //检测长度是否符合标准
    if((length < YD_MIN_LEN_PACK)||(length > YD_MAX_LEN_PACK))
    {
        #if EN_DEBUG_YDT
        printf("unpackProtocol error:length, errno:5, errinfo:命令格式错\r\n");
        fflush(stdout);
        #endif
        return 5;//命令格式错
    }

    //检测头尾标识是否正确
    if((YD_SOI != protocol[0])||(YD_EOI != protocol[length - 1]))
    {
        #if EN_DEBUG_YDT
        printf("unpackProtocol error:SOI or EOI, errno:5, errinfo:命令格式错\r\n");
        fflush(stdout);
        #endif
        return 5;//命令格式错
    }
    pst_ydt->SOI = YD_SOI;
    pst_ydt->EOI = YD_EOI;

    //验证CHKSUM
    CHKSUM = YD_T_1363_3_2005::calculateCHKSUM(protocol + 1, length - 1 - 5);
    YD_T_1363_3_2005::my_strncpy((char*)temp, (char*)protocol + length - 5, 2);
    pst_ydt->CHKSUM[0] = (u8)strtol((char*)temp, NULL, 16);
    YD_T_1363_3_2005::my_strncpy((char*)temp, (char*)protocol + length - 3, 2);
    pst_ydt->CHKSUM[1] = (u8)strtol((char*)temp, NULL, 16);
    if(CHKSUM != YD_T_1363_3_2005::getCHKSUM(pst_ydt))
    {
        #if EN_DEBUG_YDT
        printf("unpackProtocol error:CHKSUM, errno:2, errinfo:CHKSUM错\r\n");
        fflush(stdout);
        #endif
        return 2;//CHKSUM错
    }

    //获取长度并验证校验码LCHKSUM
    YD_T_1363_3_2005::my_strncpy((char*)temp, (char*)protocol + 9, 2);
    pst_ydt->LENGTH[0] = (u8)strtol((char*)temp, NULL, 16);
    YD_T_1363_3_2005::my_strncpy((char*)temp, (char*)protocol + 11, 2);
    pst_ydt->LENGTH[1] = (u8)strtol((char*)temp, NULL, 16);
    LENID = YD_T_1363_3_2005::getLENID(pst_ydt);
    if(LENID < 0)
    {
        #if EN_DEBUG_YDT
        printf("unpackProtocol error:LCHKSUM, errno:3, errinfo:LCHKSUM错\r\n");
        fflush(stdout);
        #endif
        return 3;//LCHKSUM错误
    }

    //检测协议版本号是否正确
    YD_T_1363_3_2005::my_strncpy((char*)temp, (char*)protocol + 1, 2);
    if(YD_VER != (u8)strtol((char*)temp, NULL, 16))
    {
        #if EN_DEBUG_YDT
        printf("unpackProtocol error:VER, errno:1, errinfo:VER错\r\n");
        fflush(stdout);
        #endif
        return 1;//VER错
    }
    pst_ydt->VER = YD_VER;

    //获取设备地址
    YD_T_1363_3_2005::my_strncpy((char*)temp, (char*)protocol + 3, 2);
    pst_ydt->ADR = (u8)strtol((char*)temp, NULL, 16);

    //获取设备类型
    YD_T_1363_3_2005::my_strncpy((char*)temp, (char*)protocol + 5, 2);
    pst_ydt->CID1 = (u8)strtol((char*)temp, NULL, 16);

    //获取返回码RTN
    YD_T_1363_3_2005::my_strncpy((char*)temp, (char*)protocol + 7, 2);
    pst_ydt->CID2 = (u8)strtol((char*)temp, NULL, 16);

    #if EN_SU   
    if(pst_ydt->CID2)//检测返回码
    {
        #if EN_DEBUG_YDT
        printf("unpackProtocol:ok, SM says:the data SU sended is wrong, errno:%d\r\n", pst_ydt->CID2);
        fflush(stdout);
        #endif

        switch (pst_ydt->CID2)
        {
        case YD_RTN_ERROR_VER: //VER错
        case YD_RTN_ERROR_CHKSUM: //CHKSUM错
        case YD_RTN_ERROR_LCHKSUM: //LCHKSUM错
        case YD_RTN_CID2_INVALID: //CID2无效
        case YD_RTN_ERROR_CMD: //命令格式错
        case YD_RTN_DATA_INVALID://无效数据
            return pst_ydt->CID2;
        default:
            break;
        }
    }
    #endif

    //获取INFO
    for(i = 0; i < LENID; i++)
    {
        YD_T_1363_3_2005::my_strncpy((char*)temp, (char*)protocol + 13 + 2 * i, 2);
        pst_ydt->INFO[i] = (u8)strtol((char*)temp, NULL, 16);
    }

    #if EN_DEBUG_YDT
    printf("unpackProtocol:ok, ");
    printf("VER:%02X, ", pst_ydt->VER & 0xff);
    printf("ADR:%02X, ", pst_ydt->ADR & 0xff);
    printf("CID1:%02X, ", pst_ydt->CID1 & 0xff);
    printf("CID2:%02X, ", pst_ydt->CID2 & 0xff);
    printf("LENID:%04X, ", LENID & 0xffff);
    printf("INFO:");
    for(i = 0; i < (LENID / 2); i++)
        printf("%02X ", pst_ydt->INFO[i] & 0xff);
    printf("\r\n");
    fflush(stdout);
    #endif

    return 0;//正常
}

//获取协议中的LENID
//如果返回 < 0，则LCHKSUM错误
int YD_T_1363_3_2005::getLENID(pst_ydt136332005 pst_ydt)
{
    u8 D11_8 = 0;
    u8 D7_4 = 0;
    u8 D3_0 = 0;
    u8 LCHKSUM = 0;
    u16 LENID = 0;

    //取得LENID
    LENID = ((((u8)(pst_ydt->LENGTH[0])) << 8) + (u8)(pst_ydt->LENGTH[1])) & 0x0fff;
    //取得LCHKSUM
    LCHKSUM = (pst_ydt->LENGTH[0] >> 4) & 0x0f;

    D11_8 = (LENID >> 8) & 0x0f;
    D7_4 = (LENID >> 4) & 0x0f;
    D3_0 = LENID & 0x0f;

    if(LCHKSUM != (((~((D11_8 + D7_4 + D3_0) % 16)) + 1) & 0x0f))
        return -1;

    return LENID;
}

//设置协议结构体中的LENGTH（包括LENID和LCHKSUM）
void YD_T_1363_3_2005::setLENGTH(pst_ydt136332005 pst_ydt, u16 LENID)
{
    u8 D11_8 = 0;
    u8 D7_4 = 0;
    u8 D3_0 = 0;
    u8 LCHKSUM = 0;
    u16 LENGTH = 0;

    D11_8 = (LENID >> 8) & 0x0f;
    D7_4 = (LENID >> 4) & 0x0f;
    D3_0 = LENID & 0x0f;

    LCHKSUM = ((~((D11_8 + D7_4 + D3_0) % 16)) + 1) & 0x0f;

    LENGTH = ((LENID & 0x0fff) + ((LCHKSUM << 12) & 0xf000)) & 0xffff;

    pst_ydt->LENGTH[0] = (LENGTH >> 8) & 0xff;
    pst_ydt->LENGTH[1] = LENGTH & 0xff;
}

//计算并返回校验码
u16 YD_T_1363_3_2005::calculateCHKSUM(u8 * data, u16 length)
{
    int sum = 0;
    u16 i = 0;

    #if EN_DEBUG_YDT
    printf("calculateCHKSUM:");
    #endif
    for(i = 0; i < length; i++)
    {
        sum += data[i];
        #if EN_DEBUG_YDT
        if(!(i % 2))
            printf(" %c", data[i]);
        else
            printf("%c", data[i]);
        fflush(stdout);
        #endif
    }    
    #if EN_DEBUG_YDT
    printf(", CHKSUM: %04X\r\n", ((~(sum % 65536)) + 1) & 0xffff);
    fflush(stdout);
    #endif
    return  ((~(sum % 65536)) + 1) & 0xffff;
}

//获取协议结构体中的CHKSUM
u16 YD_T_1363_3_2005::getCHKSUM(pst_ydt136332005 pst_ydt)
{
    #if EN_DEBUG_YDT
    printf("getCHKSUM: %04X\r\n",
           ((((u8)(pst_ydt->CHKSUM[0])) << 8) + (u8)(pst_ydt->CHKSUM[1])) & 0xffff);
    fflush(stdout);
    #endif
    return ((((u8)(pst_ydt->CHKSUM[0])) << 8) + (u8)(pst_ydt->CHKSUM[1])) & 0xffff;
}

//设置协议结构体中的CHKSUM
void YD_T_1363_3_2005::setCHKSUM(pst_ydt136332005 pst_ydt)
{
    u8 temp[3] = {0};
    u16 i = 0;
    int sum = 0;

    #if EN_DEBUG_YDT
    printf("setCHKSUM:");
    #endif
    for(i = 1; i <= (6 + (YD_T_1363_3_2005::getLENID(pst_ydt) / 2)); i++)
    {
        sprintf((char*)temp, "%02X", *((char*)pst_ydt + i) & 0xff);
        sum += temp[0] + temp[1];
        #if EN_DEBUG_YDT
        printf(" %c%c", temp[0], temp[1]);
        #endif
    }
    sum = (~(sum % 65536)) + 1;

    pst_ydt->CHKSUM[0] = (sum >> 8) & 0xff;
    pst_ydt->CHKSUM[1] = sum & 0xff;

    #if EN_DEBUG_YDT
    printf(", CHKSUM: %02X%02X\r\n", pst_ydt->CHKSUM[0] & 0xff, pst_ydt->CHKSUM[1] & 0xff);
    fflush(stdout);
    #endif
}

//将u8[4]字节数组转float类型，u8[0]存着float类型的低八位
float YD_T_1363_3_2005::bytes2float(u8 * bytes)
{
    u8 i = 0;
    float f = 0.0;
    void * pf  = &f;

    for(i = 0; i < 4; i++)
    {
        *((u8*)pf + i) = *(bytes + i) & 0xff;
    }

    return f;
}

//将float类型转换为u8[4]，u8[0]存着float类型的低八位
void YD_T_1363_3_2005::float2bytes(u8 * bytes, float f)
{
    float f_temp = f;
    u8 i = 0;
    u8 * p = (u8*)(&f_temp);

    for(i = 0; i < 4; i++)
    {
        bytes[i] = *(p + i) & 0xff;
    }
}

//将u8[2]字节数组转short类型，u8[0]存着u16类型的高八位
u16 YD_T_1363_3_2005::bytes2u16(u8 * bytes)
{
    u16 s = 0;
    s += (u16)((bytes[0] << 8) + bytes[1]);

    return s;
}

//将u8[4]字节数组转u32类型，u8[0]存着u32类型的高八位
u32 YD_T_1363_3_2005::bytes2u32(u8 * bytes)
{
    u32 s = 0;
    s += (u32)((bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3]);

    return s;
}

//将u16类型转换为u8[2]，u8[0]存着u16类型的高八位
void YD_T_1363_3_2005::u162bytes(u8 * bytes, u16 s)
{
    bytes[0] = (s >> 8) & 0xff;
    bytes[1] = s & 0xff;
}

//自定义的my_strncpy，解决my_strncpy遇0返回而没有拷贝到规定的长度
void YD_T_1363_3_2005::my_strncpy(char * dest, char * src, size_t count)
{
    size_t i = 0;

    if((NULL == dest) || (NULL == src))
        return;

    for(i = 0; i < count; i++)
        *(dest + i) = *(src + i);
}

//将u8[7]时间数组，转为char*字符串"2017-05-20 10:51:30"
void YD_T_1363_3_2005::time2str(u8 * time, char * time_str)
{
    char temp[3] = {0};
    u16 year = 0;
    u8 month = 0;
    u8 day = 0;
    u8 hour = 0;
    u8 min = 0;
    u8 sec = 0;

    my_strncpy(temp, (char*)time, 2);
    year = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    month = time[2];
    day = time[3];
    hour = time[4];
    min = time[5];
    sec = time[6];

    sprintf(time_str,"%04u-%02u-%02u %02u:%02u:%02u",year,month,day,hour,min,sec);
}


#if EN_DEBUG_YDT
void YD_T_1363_3_2005::testProtocol(void)
{
    #if 0
    u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
    st_ydt136332005 st_ydt;
    memset(&st_ydt, 0, LEN_ST_YDT);
    u8 temp[8] = {0x11,0x22,0x32,0x42,0x52,0x62,0x72};

    //初始化结构体
    YD_T_1363_3_2005::stInit(&st_ydt, 0x00,YD_CID1_BATTERY, YD_CID2_GET_FLOAT_DATA, temp, 7);

    //打包得到要发送的protocol
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    //解包得到包含信息的st_ydt
    YD_T_1363_3_2005::unpackProtocol(&st_ydt, protocol);

    #endif

    #if 0
    u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
    st_ydt136332005 st_ydt;
    memset(&st_ydt, 0, LEN_ST_YDT);
    u8 temp[2] = {0x01};

    //初始化结构体
    YD_T_1363_3_2005::stInit(&st_ydt, 0x01, 0xd0, 0x45, temp, 1);

    //打包得到要发送的protocol
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    //解包得到包含信息的st_ydt
    YD_T_1363_3_2005::unpackProtocol(&st_ydt, protocol);
    #endif

    #if 0
    float f = 9.98991;
    u8 bytes[5] = {0};

    while(f < 20)
    {
        printf("before:%f", f);
        memset(bytes, 0, 5);
        YD_T_1363_3_2005::float2bytes(bytes, f);
        YD_T_1363_3_2005::bytes2float(bytes);
        printf(", after:%f\r\n", YD_T_1363_3_2005::bytes2float(bytes));
        f += 0.88888;
    }
    #endif
}
#endif
