#ifndef DEVDIRCURMETER_H
#define DEVDIRCURMETER_H

#include "./yd_t_1363_3_2005/yd_t_1363_3_2005.h"

//获取月冻结电能记录指令
#define DCM_CID2_GET_MON_FREEZE_EE 0x81
//获取月电能记录指令
#define DCM_CID2_GET_MON_EE 0x87
//获取日电能记录指令
#define DCM_CID2_GET_DAY_EE 0x89
//获取电表传感器参数
#define DCM_CID2_GET_SENSOR_ARGS 0x83
//设置电表传感器参数
#define DCM_CID2_SET_SENSOR_ARGS 0x82

//获取模拟量定点数结构体定义
typedef struct
{
    float V;//电压

    float I1;//分路1电流
    float I2;//分路2电流
    float I3;//分路3电流
    float I4;//分路4电流
    float I5;//分路5电流

    float E1;//分路1电能
    float E2;//分路2电能
    float E3;//分路3电能
    float E4;//分路4电能
    float E5;//分路5电能

    int P1;//分路1功率
    int P2;//分路2功率
    int P3;//分路3功率
    int P4;//分路4功率
    int P5;//分路5功率
}st_DCM_int_data,*pst_DCM_int_data;

//获取开关状态结构体定义
typedef struct
{
    bool DO0;
    bool DO1;
    bool DO2;
    bool DO3;
    bool DI0;
    bool DI1;
    bool DI2;
    u8 DOx;
}st_DCM_switch_state, *pst_DCM_switch_state;

//获取告警状态结构体定义
typedef struct
{
    bool overV;//过压报警
    bool lackV;//欠压报警
    bool overI;//过流报警
}st_DCM_alarm_state,*pst_DCM_alarm_state;

//获取系统历史数据（定点数）结构体定义
typedef struct
{
    u8 data_type;
    u8 time[7];
    st_DCM_int_data st_int_data;
}st_DCM_sys_history_int,*pst_DCM_sys_history_int;

//获取历史告警记录结构体定义
typedef struct
{
    u8 data_type;
    u8 time[7];
    st_DCM_alarm_state st_alarm_state;
}st_DCM_history_alarm,*pst_DCM_history_alarm;

//获取月冻结电能记录结构体定义
typedef struct
{
    u8 amount;//总条数
    float E1;//分路1电能
    float E2;//分路2电能
    float E3;//分路3电能
    float E4;//分路4电能
    float E5;//分路5电能
}st_DCM_mon_freeze_ee,*pst_DCM_mon_freeze_ee;

//获取月电能记录结构体定义
typedef struct
{
    u8 Ex;
    u8 monGroup;
    float eeArray[12];
}st_DCM_mon_ee,*pst_DCM_mon_ee;

//获取日电能记录结构体定义
typedef struct
{
    u8 Ex;
    u8 dayGroup;
    float eeArray[12];
}st_DCM_day_ee,*pst_DCM_day_ee;

//传感器参数结构体定义
typedef struct
{
    u8 sensorType;//传感器类型
    u16 sensorPrimRange;//传感器初级量程
    u16 sensorPrimRangeMin;//传感器次级量程最小值
    u16 sensorPrimRangeMax;//传感器初级量程最大值
}st_DCM_sensor_args,*pst_DCM_sensor_args;

class DevDirCurMeter
{
public:
    DevDirCurMeter();

    //开始/停止监控设备标志
    static bool DCM_startMonitorDev;
    //串口闲忙标志
    static bool DCM_is_busy;
    //当前发送的指令标志
    static u8 DCM_cur_cmd;

    //获取模拟量定点数指令
    static u8 DCM_cmd_get_int_data(u8 ADR, u8 * protocol);
    static void DCM_get_int_data(pst_DCM_int_data pst_int_data, pst_ydt136332005 pst_ydt);
    //获取开关状态指令
    static u8 DCM_cmd_get_switch_state(u8 ADR, u8 * protocol);
    static void DCM_get_switch_state(pst_DCM_switch_state pst_switch_state, pst_ydt136332005 pst_ydt);
    //获取告警状态指令
    static u8 DCM_cmd_get_alarm_state(u8 ADR, u8 * protocol);
    static void DCM_get_alarm_state(pst_DCM_alarm_state pst_alarm_state, pst_ydt136332005 pst_ydt);
    //遥控
    static u8 DCM_cmd_remote_ctrl(u8 ADR, u8 * protocol, pst_DCM_switch_state pst_switch_state);
    //获取系统历史数据（定点数）
    static u8 DCM_cmd_get_sys_history_int(u8 ADR, u8 * protocol, u8 type);
    static void DCM_get_sys_history_int(pst_DCM_sys_history_int pst_sys_his_int, pst_ydt136332005 pst_ydt);
    //获取历史告警记录
    static u8 DCM_cmd_get_history_alarm(u8 ADR, u8 * protocol, u8 type);
    static void DCM_get_history_alarm(pst_DCM_history_alarm pst_his_alarm, pst_ydt136332005 pst_ydt);
    //获取月冻结电能记录
    static u8 DCM_cmd_get_mon_freeze_ee(u8 ADR, u8 * protocol, u8 mon);
    static void DCM_get_mon_freeze_ee(pst_DCM_mon_freeze_ee pst_mon_freeze_ee, pst_ydt136332005 pst_ydt);
    //获取月电能记录
    static u8 DCM_cmd_get_mon_ee(u8 ADR, u8 * protocol, u8 Ex, u8 monGroup);
    static void DCM_get_mon_ee(pst_DCM_mon_ee pst_mon_ee, pst_ydt136332005 pst_ydt);
    //获取日电能记录
    static u8 DCM_cmd_get_day_ee(u8 ADR, u8 * protocol, u8 Ex, u8 dayGroup);
    static void DCM_get_day_ee(pst_DCM_day_ee pst_day_ee, pst_ydt136332005 pst_ydt);
    //获取传感器参数
    static u8 DCM_cmd_get_sensor_args(u8 ADR, u8 * protocol);
    static void DCM_get_sensor_args(pst_DCM_sensor_args pst_sensor_args, pst_ydt136332005 pst_ydt);
    //设置传感器参数
    static u8 DCM_cmd_set_sensor_args(u8 ADR, u8 * protocol, pst_DCM_sensor_args pst_sensor_args);
};

#endif // DEVDIRCURMETER_H
