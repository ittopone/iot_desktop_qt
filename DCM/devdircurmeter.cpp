#include "devdircurmeter.h"

//开始/停止监控设备标志
bool DevDirCurMeter::DCM_startMonitorDev = false;
//串口闲忙标志
bool DevDirCurMeter::DCM_is_busy = false;
//当前发送的指令标志
u8 DevDirCurMeter::DCM_cur_cmd = 0;

DevDirCurMeter::DevDirCurMeter()
{

}

//获取模拟量定点数指令
u8 DevDirCurMeter::DCM_cmd_get_int_data(u8 ADR, u8 * protocol)
{
    st_ydt136332005 st_ydt;

    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, YD_CID2_GET_INT_DATA, NULL, 0);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return YD_CID2_GET_INT_DATA;
}
void DevDirCurMeter::DCM_get_int_data(pst_DCM_int_data pst_int_data, pst_ydt136332005 pst_ydt)
{
    int offset = 0;
    char temp[5] = {0};

    //电压
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_int_data->V = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 10.0;
    offset += 2;
    //分路1电流
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_int_data->I1 = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 100.00;
    offset += 2;
    //分路2电流
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_int_data->I2 = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 100.00;
    offset += 2;
    //分路3电流
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_int_data->I3 = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 100.00;
    offset += 2;
    //分路4电流
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_int_data->I4 = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 100.00;
    offset += 2;
    //分路5电流
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_int_data->I5 = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 100.00;
    offset += 2;
    //分路1功率
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_int_data->P1 = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
    //分路2功率
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_int_data->P2 = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
    //分路3功率
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_int_data->P3 = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
    //分路4功率
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_int_data->P4 = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
    //分路5功率
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_int_data->P5 = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
    //分路1电能
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 4);
    pst_int_data->E1 = YD_T_1363_3_2005::bytes2u32((u8*)temp) / 100.00;
    offset += 4;
    //分路2电能
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 4);
    pst_int_data->E2 = YD_T_1363_3_2005::bytes2u32((u8*)temp) / 100.00;
    offset += 4;
    //分路3电能
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 4);
    pst_int_data->E3 = YD_T_1363_3_2005::bytes2u32((u8*)temp) / 100.00;
    offset += 4;
    //分路4电能
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 4);
    pst_int_data->E4 = YD_T_1363_3_2005::bytes2u32((u8*)temp) / 100.00;
    offset += 4;
    //分路5电能
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 4);
    pst_int_data->E5 = YD_T_1363_3_2005::bytes2u32((u8*)temp) / 100.00;
    offset += 4;
}
//获取开关状态指令
u8 DevDirCurMeter::DCM_cmd_get_switch_state(u8 ADR, u8 * protocol)
{
    st_ydt136332005 st_ydt;

    memset(&st_ydt, 0, LEN_ST_YDT);    
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, YD_CID2_GET_SWITCH_STATE, NULL, 0);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return YD_CID2_GET_SWITCH_STATE;
}
void DevDirCurMeter::DCM_get_switch_state(pst_DCM_switch_state pst_switch_state, pst_ydt136332005 pst_ydt)
{
    u8 i = 0;

    for(i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            if((pst_ydt->INFO[0] >> i) & 0x01)
                pst_switch_state->DO0 = true;
            else
                pst_switch_state->DO0 = false;
            break;
        case 1:
            if((pst_ydt->INFO[0] >> i) & 0x01)
                pst_switch_state->DO1 = true;
            else
                pst_switch_state->DO1 = false;
            break;
        case 2:
            if((pst_ydt->INFO[0] >> i) & 0x01)
                pst_switch_state->DO2 = true;
            else
                pst_switch_state->DO2 = false;
            break;
        case 3:
            if((pst_ydt->INFO[0] >> i) & 0x01)
                pst_switch_state->DO3 = true;
            else
                pst_switch_state->DO3 = false;
            break;
        default:
            break;
        }
    }

    for(i = 0; i < 3; i++)
    {
        switch (i)
        {
        case 0:
            if((pst_ydt->INFO[1] >> i) & 0x01)
                pst_switch_state->DI0 = true;
            else
                pst_switch_state->DI0 = false;
            break;
        case 1:
            if((pst_ydt->INFO[1] >> i) & 0x01)
                pst_switch_state->DI1 = true;
            else
                pst_switch_state->DI1 = false;
            break;
        case 2:
            if((pst_ydt->INFO[1] >> i) & 0x01)
                pst_switch_state->DI2 = true;
            else
                pst_switch_state->DI2 = false;
            break;
        default:
            break;
        }
    }
}
//获取告警状态指令
u8 DevDirCurMeter::DCM_cmd_get_alarm_state(u8 ADR, u8 * protocol)
{
    st_ydt136332005 st_ydt;

    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, YD_CID2_GET_ALARM_STATE, NULL, 0);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return YD_CID2_GET_ALARM_STATE;
}
void DevDirCurMeter::DCM_get_alarm_state(pst_DCM_alarm_state pst_alarm_state, pst_ydt136332005 pst_ydt)
{
    u8 i = 0;

    for(i = 0; i < 3; i++)
    {
        switch (i)
        {
        case 0:
            if((pst_ydt->INFO[0] >> i) & 0x01)
                pst_alarm_state->overV = true;
            else
                pst_alarm_state->overV = false;
            break;
        case 1:
            if((pst_ydt->INFO[0] >> i) & 0x01)
                pst_alarm_state->lackV = true;
            else
                pst_alarm_state->lackV = false;
            break;
        case 2:
            if((pst_ydt->INFO[0] >> i) & 0x01)
                pst_alarm_state->overI = true;
            else
                pst_alarm_state->overI = false;
            break;
        default:
            break;
        }
    }
}

//遥控
u8 DevDirCurMeter::DCM_cmd_remote_ctrl(u8 ADR, u8 * protocol, pst_DCM_switch_state pst_switch_state)
{
    st_ydt136332005 st_ydt;
    u8 info[2] = {0};
    u8 i = 0;

    for(i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            if(pst_switch_state->DO0)
                info[0] |= 0x01;
            else
                info[0] &= 0xfe;
            break;
        case 1:
            if(pst_switch_state->DO1)
                info[0] |= 0x02;
            else
                info[0] &= 0xfd;
            break;
        case 2:
            if(pst_switch_state->DO2)
                info[0] |= 0x04;
            else
                info[0] &= 0xfb;
            break;
        case 3:
            if(pst_switch_state->DO3)
                info[0] |= 0x08;
            else
                info[0] &= 0xf7;
            break;
        default:
            break;
        }
    }

    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, YD_CID2_REMOTE_CTRL, info, 1);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return YD_CID2_REMOTE_CTRL;
}
//DCM获取系统历史数据（定点数）
u8 DevDirCurMeter::DCM_cmd_get_sys_history_int(u8 ADR, u8 * protocol, u8 type)
{
    st_ydt136332005 st_ydt;
    u8 info[2] = {0};

    info[0] = type;
    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, YD_CID2_GET_SYS_HISTORY_INT, info, 1);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return YD_CID2_GET_SYS_HISTORY_INT;
}
void DevDirCurMeter::DCM_get_sys_history_int(pst_DCM_sys_history_int pst_sys_his_int, pst_ydt136332005 pst_ydt)
{
    char temp[5] = {0};
    u8 offset = 0;

    pst_sys_his_int->data_type = pst_ydt->INFO[0];
    offset += 1;
    YD_T_1363_3_2005::my_strncpy((char*)(pst_sys_his_int->time), (char*)(pst_ydt->INFO) + offset, 7);
    offset += 7;
    //电压
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_his_int->st_int_data.V = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 10.0;
    offset += 2;
    //分路1电流
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_his_int->st_int_data.I1 = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 100.00;
    offset += 2;
    //分路2电流
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_his_int->st_int_data.I2 = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 100.00;
    offset += 2;
    //分路3电流
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_his_int->st_int_data.I3 = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 100.00;
    offset += 2;
    //分路4电流
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_his_int->st_int_data.I4 = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 100.00;
    offset += 2;
    //分路5电流
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_his_int->st_int_data.I5 = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 100.00;
    offset += 2;
    //分路1功率
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_his_int->st_int_data.P1 = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
    //分路2功率
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_his_int->st_int_data.P2 = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
    //分路3功率
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_his_int->st_int_data.P3 = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
    //分路4功率
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_his_int->st_int_data.P4 = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
    //分路5功率
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_his_int->st_int_data.P5 = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
    //分路1电能
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 4);
    pst_sys_his_int->st_int_data.E1 = YD_T_1363_3_2005::bytes2u32((u8*)temp) / 100.00;
    offset += 4;
    //分路2电能
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 4);
    pst_sys_his_int->st_int_data.E2 = YD_T_1363_3_2005::bytes2u32((u8*)temp) / 100.00;
    offset += 4;
    //分路3电能
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 4);
    pst_sys_his_int->st_int_data.E3 = YD_T_1363_3_2005::bytes2u32((u8*)temp) / 100.00;
    offset += 4;
    //分路4电能
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 4);
    pst_sys_his_int->st_int_data.E4 = YD_T_1363_3_2005::bytes2u32((u8*)temp) / 100.00;
    offset += 4;
    //分路5电能
    YD_T_1363_3_2005::my_strncpy(temp,  (char*)(pst_ydt->INFO) + offset, 4);
    pst_sys_his_int->st_int_data.E5 = YD_T_1363_3_2005::bytes2u32((u8*)temp) / 100.00;
    offset += 4;
}

//获取历史告警记录
u8 DevDirCurMeter::DCM_cmd_get_history_alarm(u8 ADR, u8 * protocol, u8 type)
{
    st_ydt136332005 st_ydt;
    u8 info[2] = {0};

    info[0] = type;
    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, YD_CID2_GET_HISTORY_ALARM, info, 1);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return YD_CID2_GET_HISTORY_ALARM;
}
void DevDirCurMeter::DCM_get_history_alarm(pst_DCM_history_alarm pst_his_alarm, pst_ydt136332005 pst_ydt)
{
    u8 i = 0;
    u8 offset = 0;

    pst_his_alarm->data_type = pst_ydt->INFO[0];
    offset += 1;
    YD_T_1363_3_2005::my_strncpy((char*)(pst_his_alarm->time), (char*)(pst_ydt->INFO) + offset, 7);
    offset += 7;

    for(i = 0; i < 3; i++)
    {
        switch (i)
        {
        case 0:
            if((pst_ydt->INFO[offset] >> i) & 0x01)
                pst_his_alarm->st_alarm_state.overV = true;
            else
                pst_his_alarm->st_alarm_state.overV = false;
            break;
        case 1:
            if((pst_ydt->INFO[offset] >> i) & 0x01)
                pst_his_alarm->st_alarm_state.lackV = true;
            else
                pst_his_alarm->st_alarm_state.lackV = false;
            break;
        case 2:
            if((pst_ydt->INFO[offset] >> i) & 0x01)
                pst_his_alarm->st_alarm_state.overI = true;
            else
                pst_his_alarm->st_alarm_state.overI = false;
            break;
        default:
            break;
        }
    }
}

//获取月冻结电能记录
u8 DevDirCurMeter::DCM_cmd_get_mon_freeze_ee(u8 ADR, u8 * protocol, u8 mon)
{
    st_ydt136332005 st_ydt;
    u8 info[2] = {0};

    info[0] = mon;
    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, DCM_CID2_GET_MON_FREEZE_EE, info, 1);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return DCM_CID2_GET_MON_FREEZE_EE;
}
void DevDirCurMeter::DCM_get_mon_freeze_ee(pst_DCM_mon_freeze_ee pst_mon_freeze_ee, pst_ydt136332005 pst_ydt)
{
    char temp[5] = {0};
    u8 offset = 0;

    /*总条数*/
    pst_mon_freeze_ee->amount = pst_ydt->INFO[0];
    offset += 1;

    /*分路1电能*/
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_freeze_ee->E1 = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;

    /*分路2电能*/
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_freeze_ee->E2 = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;

    /*分路3电能*/
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_freeze_ee->E3 = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;

    /*分路4电能*/
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_freeze_ee->E4 = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;

    /*分路5电能*/
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_freeze_ee->E5 = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
}

//获取月电能记录
u8 DevDirCurMeter::DCM_cmd_get_mon_ee(u8 ADR, u8 * protocol, u8 Ex, u8 monGroup)
{
    st_ydt136332005 st_ydt;
    u8 info[3] = {0};

    info[0] = Ex;
    info[1] = monGroup;
    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, DCM_CID2_GET_MON_EE, info, 2);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return DCM_CID2_GET_MON_EE;
}
void DevDirCurMeter::DCM_get_mon_ee(pst_DCM_mon_ee pst_mon_ee, pst_ydt136332005 pst_ydt)
{
    char temp[5] = {0};
    u8 offset = 0;

    pst_mon_ee->Ex = pst_ydt->INFO[0];
    offset += 1;
    pst_mon_ee->monGroup = pst_ydt->INFO[1];
    offset += 1;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_ee->eeArray[0] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_ee->eeArray[1] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_ee->eeArray[2] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_ee->eeArray[3] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_ee->eeArray[4] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_ee->eeArray[5] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_ee->eeArray[6] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_ee->eeArray[7] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_ee->eeArray[8] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_ee->eeArray[9] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_ee->eeArray[10] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_mon_ee->eeArray[11] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
}

//获取日电能记录
u8 DevDirCurMeter::DCM_cmd_get_day_ee(u8 ADR, u8 * protocol, u8 Ex, u8 dayGroup)
{
    st_ydt136332005 st_ydt;
    u8 info[3] = {0};

    info[0] = Ex;
    info[1] = dayGroup;
    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, DCM_CID2_GET_DAY_EE, info, 2);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return DCM_CID2_GET_DAY_EE;
}
void DevDirCurMeter::DCM_get_day_ee(pst_DCM_day_ee pst_day_ee, pst_ydt136332005 pst_ydt)
{
    char temp[5] = {0};
    u8 offset = 0;

    pst_day_ee->Ex = pst_ydt->INFO[0];
    offset += 1;
    pst_day_ee->dayGroup = pst_ydt->INFO[1];
    offset += 1;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_day_ee->eeArray[0] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_day_ee->eeArray[1] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_day_ee->eeArray[2] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_day_ee->eeArray[3] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_day_ee->eeArray[4] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_day_ee->eeArray[5] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_day_ee->eeArray[6] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_day_ee->eeArray[7] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_day_ee->eeArray[8] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_day_ee->eeArray[9] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_day_ee->eeArray[10] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 4);
    pst_day_ee->eeArray[11] = YD_T_1363_3_2005::bytes2float((u8*)temp) / 100.00;
    offset += 4;
}

//获取传感器参数
u8 DevDirCurMeter::DCM_cmd_get_sensor_args(u8 ADR, u8 * protocol)
{
    st_ydt136332005 st_ydt;

    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, DCM_CID2_GET_SENSOR_ARGS, NULL, 0);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return DCM_CID2_GET_SENSOR_ARGS;
}
void DevDirCurMeter::DCM_get_sensor_args(pst_DCM_sensor_args pst_sensor_args, pst_ydt136332005 pst_ydt)
{
    char temp[3] = {0};
    u8 offset = 0;

    pst_sensor_args->sensorType = pst_ydt->INFO[0];
    offset += 1;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 2);
    pst_sensor_args->sensorPrimRange = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 2);
    pst_sensor_args->sensorPrimRangeMin = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 2);
    pst_sensor_args->sensorPrimRangeMax = YD_T_1363_3_2005::bytes2u16((u8*)temp);
    offset += 2;
}

//设置传感器参数
u8 DevDirCurMeter::DCM_cmd_set_sensor_args(u8 ADR, u8 * protocol, pst_DCM_sensor_args pst_sensor_args)
{
    u8 info[8] = {0};
    u8 offset = 0;
    st_ydt136332005 st_ydt;

    info[0] = pst_sensor_args->sensorType;
    offset += 1;
    YD_T_1363_3_2005::u162bytes(info + offset, pst_sensor_args->sensorPrimRange);
    offset += 2;
    YD_T_1363_3_2005::u162bytes(info + offset, pst_sensor_args->sensorPrimRangeMin);
    offset += 2;
    YD_T_1363_3_2005::u162bytes(info + offset, pst_sensor_args->sensorPrimRangeMax);
    offset += 2;
    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, DCM_CID2_SET_SENSOR_ARGS, info, 7);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return DCM_CID2_SET_SENSOR_ARGS;
}

//获取抄表日
u8 DevDirCurMeter::DCM_cmd_get_read_meter_day(u8 ADR, u8 * protocol)
{
    st_ydt136332005 st_ydt;

    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, DCM_CID2_GET_READ_METER_DAY, NULL, 0);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return DCM_CID2_GET_READ_METER_DAY;
}
void DevDirCurMeter::DCM_get_read_meter_day(pst_DCM_read_meter_day pst_read_meter_day, pst_ydt136332005 pst_ydt)
{
    pst_read_meter_day->day = pst_ydt->INFO[0];
    pst_read_meter_day->hour = pst_ydt->INFO[1];
}

//设置抄表日
u8 DevDirCurMeter::DCM_cmd_set_read_meter_day(u8 ADR, u8 * protocol, pst_DCM_read_meter_day pst_read_meter_day)
{
    u8 info[3] = {0};
    st_ydt136332005 st_ydt;

    info[0] = pst_read_meter_day->day;
    info[1] = pst_read_meter_day->hour;
    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, DCM_CID2_SET_READ_METER_DAY, info, 2);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return DCM_CID2_SET_READ_METER_DAY;
}

//表校准
u8 DevDirCurMeter::DCM_cmd_meter_calibration(u8 ADR, u8 * protocol, u8 opt)
{
    u8 info[2] = {0};
    st_ydt136332005 st_ydt;

    info[0] = opt;
    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, DCM_CID2_METER_CALIBRATION, info, 1);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return DCM_CID2_METER_CALIBRATION;
}
//获取系统参数
u8 DevDirCurMeter::DCM_cmd_get_sys_args(u8 ADR, u8 * protocol)
{
    st_ydt136332005 st_ydt;

    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, YD_CID2_GET_SYS_INT_ARGS, NULL, 0);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return YD_CID2_GET_SYS_INT_ARGS;
}
void DevDirCurMeter::DCM_get_sys_args(pst_DCM_sys_args pst_sys_args, pst_ydt136332005 pst_ydt)
{
    char temp[3] = {0};
    u8 offset = 0;

    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_args->overV = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 10.0;
    offset += 2;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_args->lackV = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 10.0;
    offset += 2;
    YD_T_1363_3_2005::my_strncpy(temp, (char*)(pst_ydt->INFO) + offset, 2);
    pst_sys_args->overI = YD_T_1363_3_2005::bytes2u16((u8*)temp) / 100.00;
    offset += 2;
}

//设置系统参数
u8 DevDirCurMeter::DCM_cmd_set_sys_args(u8 ADR, u8 * protocol, pst_DCM_sys_args pst_sys_args)
{
    u8 info[7] = {0};
    u8 offset = 0;
    st_ydt136332005 st_ydt;

    YD_T_1363_3_2005::u162bytes(info + offset, (u16)(pst_sys_args->overV * 10));
    offset += 2;
    YD_T_1363_3_2005::u162bytes(info + offset, (u16)(pst_sys_args->lackV * 10));
    offset += 2;
    YD_T_1363_3_2005::u162bytes(info + offset, (u16)(pst_sys_args->overI * 100));
    offset += 2;

    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, YD_CID2_SET_SYS_INT_ARGS, info, 6);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return YD_CID2_SET_SYS_INT_ARGS;
}

//获取电能脉冲输出通道开关
u8 DevDirCurMeter::DCM_cmd_get_ee_pulse_switch(u8 ADR, u8 * protocol)
{
    st_ydt136332005 st_ydt;

    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, DCM_CID2_GET_EE_PULSE_SWITCH, NULL, 0);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return DCM_CID2_GET_EE_PULSE_SWITCH;
}
void DevDirCurMeter::DCM_get_ee_pulse_switch(pst_DCM_ee_pulse_switch pst_ee_pulse_switch, pst_ydt136332005 pst_ydt)
{
    u8 i = 0;

    for(i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            if((pst_ydt->INFO[0] >> i) & 0x01)
                pst_ee_pulse_switch->eeps1= true;
            else
                pst_ee_pulse_switch->eeps1 = false;
            break;
        case 1:
            if((pst_ydt->INFO[0] >> i) & 0x01)
                pst_ee_pulse_switch->eeps2 = true;
            else
                pst_ee_pulse_switch->eeps2 = false;
            break;
        case 2:
            if((pst_ydt->INFO[0] >> i) & 0x01)
                pst_ee_pulse_switch->eeps3 = true;
            else
                pst_ee_pulse_switch->eeps3 = false;
            break;
        case 3:
            if((pst_ydt->INFO[0] >> i) & 0x01)
                pst_ee_pulse_switch->eeps4 = true;
            else
                pst_ee_pulse_switch->eeps4 = false;
            break;
        default:
            break;
        }
    }
}
//设置电能脉冲输出通道开关
u8 DevDirCurMeter::DCM_cmd_set_ee_pulse_switch(u8 ADR, u8 * protocol, pst_DCM_ee_pulse_switch pst_ee_pulse_switch)
{
    st_ydt136332005 st_ydt;
    u8 info[2] = {0};
    u8 i = 0;

    for(i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            if(pst_ee_pulse_switch->eeps1)
                info[0] |= 0x01;
            else
                info[0] &= 0xfe;
            break;
        case 1:
            if(pst_ee_pulse_switch->eeps2)
                info[0] |= 0x02;
            else
                info[0] &= 0xfd;
            break;
        case 2:
            if(pst_ee_pulse_switch->eeps3)
                info[0] |= 0x04;
            else
                info[0] &= 0xfb;
            break;
        case 3:
            if(pst_ee_pulse_switch->eeps4)
                info[0] |= 0x08;
            else
                info[0] &= 0xf7;
            break;
        default:
            break;
        }
    }

    memset(&st_ydt, 0, LEN_ST_YDT);
    YD_T_1363_3_2005::stInit(&st_ydt, ADR, YD_CID1_DIRCURMETER, DCM_CID2_SET_EE_PULSE_SWITCH, info, 1);
    YD_T_1363_3_2005::packProtocol(&st_ydt, protocol);

    return DCM_CID2_SET_EE_PULSE_SWITCH;
}
