#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMenu>

#include "./JasonQt_SerialPort/JasonQt_SerialPort.h"
#include "./DCM/devdircurmeter.h"

//串口定时器超时时间
#define TIMER_SERIAL_PORT_MSEC 3000

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    /*********************************界面优化相关*******************************/
private:
    QPoint mousePoint;
    bool mousePressed;
    bool max;
    QRect location;
    void InitStyle();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
private slots:
    void on_btnMenu_Close_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Min_clicked();
    void on_btnMenu_clicked();
    /***************************************************************************/

    /*********************************操作界面相关***********************************/
private:
    //菜单
    QMenu * m_menu;
    //菜单选项-个人信息修改
    QAction * m_selfInfo;
    //系统初始化
    void initSystem();
    void initMenu();//初始化菜单
private slots:
    void on_listWidget_currentRowChanged(int currentRow);//左边导航栏切换
    /***************************************************************************/

    /*********************************串口 start***********************************/
private:
    QTimer * m_timerSerialPort;//串口定时器
    void initSerialPort();//串口初始化
private slots:
    void on_timerSerialPort_timeout();//串口定时器槽
    /*********************************DCM start***********************************/
private:
    JasonQt_SerialPort m_sp_DCM;//直流电表连接的串口
    st_DCM_switch_state m_DCM_switch_state;//DCM开关状态
    u8 m_DCMmonFreezeEEcurMon;
    void refreshSerialPortNameOfDCM();//刷新DCM串口名称
private slots:
    void acceptedOfDCM(const QByteArray &data);
    void errorOfDCM(const QSerialPort::SerialPortError &error);
    void on_pushButton_44_clicked();//DCM刷新串口
    void on_pushButton_45_clicked();//DCM打开/关闭串口
    void on_pushButton_56_clicked();//DCM监控开关
    void on_pushButton_54_clicked();//DCM获取开关状态
    void on_pushButton_11_clicked();//DCM遥控DO0
    void on_pushButton_12_clicked();//DCM遥控DO1
    void on_pushButton_13_clicked();//DCM遥控DO2
    void on_pushButton_14_clicked();//DCM遥控DO3
    void on_pushButton_15_clicked();//DCM获取系统历史数据（定点数）
    void on_pushButton_17_clicked();//DCM获取历史告警记录
    void on_pushButton_18_clicked();//DCM获取月冻结电能记录
    void on_pushButton_20_clicked();//DCM获取月电能记录
    void on_pushButton_19_clicked();//DCM获取日电能记录
    void on_pushButton_21_clicked();//DCM获取电表传感器参数
    void on_pushButton_22_clicked();//DCM设置电表传感器参数
    /*********************************DCM end***********************************/

    /*********************************BMS start***********************************/
private slots:
    void on_pushButton_28_clicked();//BMS刷新串口
    void on_pushButton_29_clicked();//BMS打开/关闭串口
    /*********************************BMS end***********************************/

    /*********************************串口 end***********************************/

    /*********************************信号 start***************************************/



signals:
    /*********************************信号 end******************************************/
};

#endif // MAINWINDOW_H
