#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myhelper.h"
#include "iconhelper.h"
#include "frmmessagebox.h"
#include "global.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //关乎界面
    myHelper::FormInCenter(this);
    this->InitStyle();
    //系统初始化
    this->initSystem();
    //串口初始化
    this->initSerialPort();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitStyle()
{
    //设置窗体标题栏隐藏
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    location = this->geometry();
    max = false;
    mousePressed = false;

    //安装事件监听器,让标题栏识别鼠标双击
    ui->lab_Title->installEventFilter(this);

    IconHelper::Instance()->SetIcon(ui->btnMenu_Close, QChar(0xf00d), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Min, QChar(0xf068), 10);
    //修改菜单按钮图标
    //IconHelper::Instance()->SetIcon(ui->btnMenu, QChar(0xf0c9), 10);
    //修改软件左上角的图标
    //IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf015), 12);

    //初始化菜单
    this->initMenu();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick)
    {
        this->on_btnMenu_Max_clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton) && !max)
    {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void MainWindow::on_btnMenu_Close_clicked()
{
    qApp->exit();
}

void MainWindow::on_btnMenu_Max_clicked()
{
    if (max)
    {
        this->setGeometry(location);
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
        ui->btnMenu_Max->setToolTip(tr("最大化"));
    }
    else
    {
        location = this->geometry();
        this->setGeometry(qApp->desktop()->availableGeometry());
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf079), 10);
        ui->btnMenu_Max->setToolTip(tr("还原"));
    }
    max = !max;
}

void MainWindow::on_btnMenu_Min_clicked()
{
    this->showMinimized();
}

void MainWindow::on_btnMenu_clicked()
{

}

//初始化菜单
void MainWindow::initMenu()
{
    m_menu = new QMenu();

    m_selfInfo = new QAction(tr("个人信息"), this);

    m_menu->addAction(m_selfInfo);
    ui->btnMenu->setMenu(m_menu);

    #if 0
    //去除小箭头
    ui->btnMenu->setStyleSheet("QPushButton::menu-indicator{image:none}");
    #else
    //采用系统默认菜单按钮图标，并将其居中显示
    ui->btnMenu->setStyleSheet("QPushButton::menu-indicator{subcontrol-position:center;subcontrol-origin: padding;}");
    #endif
}

//左边导航栏切换
void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);

    if(0 == currentRow)
    {
        //加载本地的html文件
        QString dirStr = QDir::currentPath();
        qDebug() << "currentPath:" << dirStr;
        dirStr.replace("/", "\\");
        QUrl url("file:///" + dirStr + "\\web\\baidumap.html");
        ui->webView->setUrl(url);
    }
}

//系统初始化
void MainWindow::initSystem()
{
    //设置QT可以调用javascript函数
    ui->webView->page()->setForwardUnsupportedContent(true);
    ui->webView->page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    ui->webView->page()->settings()->setAttribute(QWebSettings::JavaEnabled, true);
    ui->webView->settings()->setAttribute(QWebSettings::AutoLoadImages, true);
    //跳转到显示地图的窗口
    ui->listWidget->setCurrentRow(0);
}

/***********************************串口 start******************************/

//串口初始化
void MainWindow::initSerialPort()
{    
    //刷新串口
    this->refreshSerialPortNameOfDCM();
    //创建串口定时器
    m_timerSerialPort = new QTimer(this);
    //串口定时器信号与槽
    connect(m_timerSerialPort, SIGNAL(timeout()), this, SLOT(on_timerSerialPort_timeout()));
    //开启串口定时器
    m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);

    //DCM变量初始化
    memset(&m_DCM_switch_state, 0, sizeof(st_DCM_switch_state));
    //DCM槽函数在主线程运行(因为要操作GUI)
    connect(&m_sp_DCM,SIGNAL(accepted(QByteArray)),this,SLOT(acceptedOfDCM(QByteArray)));
    connect(&m_sp_DCM,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(errorOfDCM(QSerialPort::SerialPortError)));    
}

//刷新串口名称
void MainWindow::on_pushButton_28_clicked()
{

}

//打开或关闭串口
void MainWindow::on_pushButton_29_clicked()
{

}

/***********************************DCM start******************************/
//刷新串口名称
void MainWindow::refreshSerialPortNameOfDCM()
{
    ui->comboBox_13->clear();
    for(const auto &now: QSerialPortInfo::availablePorts())
    {
        ui->comboBox_13->addItem(now.portName());
    }

    ui->pushButton_45->setEnabled(ui->comboBox_13->count());
}

//DCM刷新串口
void MainWindow::on_pushButton_44_clicked()
{
    this->refreshSerialPortNameOfDCM();
}

//DCM打开或关闭串口
void MainWindow::on_pushButton_45_clicked()
{
    if(m_sp_DCM.isSerialPortAvailable())
    {//串口已经打开
        if(DevDirCurMeter::DCM_startMonitorDev)
        {//监控已开启
            myHelper::ShowMessageBoxError("请先关闭监控再重试");
        }
        else
        {
            //关闭串口
            m_sp_DCM.close();
            ui->pushButton_45->setText(tr("打开串口"));
            ui->pushButton_45->setIcon(QIcon(":/image/ball_off.ico"));
            DevDirCurMeter::DCM_is_busy = false;//释放串口正忙标志
        }
    }
    else
    {//串口已经关闭打开串口
        //打开串口
        QString portName = ui->comboBox_13->currentText();
        int baudRate = ui->comboBox_14->currentText().toInt(NULL,10);
        if(m_sp_DCM.open(portName, baudRate))
        {
            ui->pushButton_45->setText(tr("关闭串口"));
            ui->pushButton_45->setIcon(QIcon(":/image/ball_on.ico"));            
        }
        else
        {
            ui->pushButton_45->setText(tr("打开串口"));
            ui->pushButton_45->setIcon(QIcon(":/image/ball_off.ico"));
        }
    }
}

//串口出错
void MainWindow::errorOfDCM(const QSerialPort::SerialPortError &error)
{
    //QMessageBox::warning(this, "Serial error", QString("Serial error: %1").arg(error));
    myHelper::ShowMessageBoxError(QString("Serial error: %1").arg(error));
}

//DCM串口接收
void MainWindow::acceptedOfDCM(const QByteArray &data)
{
    u8 cmd = DevDirCurMeter::DCM_cur_cmd;
    DevDirCurMeter::DCM_is_busy = false;//释放串口正忙标志

    st_ydt136332005 st_ydt;
    memset(&st_ydt, 0, LEN_ST_YDT);
    u8 RTN = YD_T_1363_3_2005::unpackProtocol(&st_ydt, (u8*)(data.data()));
    if(RTN)
    {//解包失败
        ui->textEdit_2->append("R:" + data + tr("(解包失败:%1)").arg(RTN));
        switch(cmd)
        {
        case YD_CID2_REMOTE_CTRL://遥控
        {
            if(0 == m_DCM_switch_state.DOx)
                m_DCM_switch_state.DO0 = !(m_DCM_switch_state.DO0);
            else if(1 == m_DCM_switch_state.DOx)
                m_DCM_switch_state.DO1 = !(m_DCM_switch_state.DO1);
            else if(2 == m_DCM_switch_state.DOx)
                m_DCM_switch_state.DO2 = !(m_DCM_switch_state.DO2);
            else if(3 == m_DCM_switch_state.DOx)
                m_DCM_switch_state.DO3 = !(m_DCM_switch_state.DO3);
        }
            break;
        case YD_CID2_GET_SYS_HISTORY_INT://DCM获取系统历史数据（定点数）
        {
            if(DevDirCurMeter::DCM_startMonitorDev)
            {//监控已开启
                if(! DevDirCurMeter::DCM_is_busy)
                {//串口空闲
                    u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
                    u8 type = 0;
                    if(ui->radioButton->isChecked())
                        type = 0x02;
                    else if(ui->radioButton_2->isChecked())
                        type = 0x12;
                    else if(ui->radioButton_3->isChecked())
                        type = 0x22;
                    //发送收到历史数据错误，重新上传上条历史数据指令
                    u8 ADR = ui->spinBox_4->value();
                    DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
                    DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_sys_history_int(ADR, protocol,type);
                    m_sp_DCM.send(QByteArray((char*)protocol));
                    ui->textEdit_2->append("S:" + QString((char*)protocol) + "(收到历史数据错误，重新上传上条历史数据)");
                }
                else
                {//串口正忙
                    //开启串口定时器
                    m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
                    myHelper::ShowMessageBoxError("串口正忙，请重试");
                }
            }
            else
            {//监控已关闭
                //开启串口定时器
                m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
                myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
            }
        }
            break;
        case YD_CID2_GET_HISTORY_ALARM://DCM获取历史报警记录
        {
            if(DevDirCurMeter::DCM_startMonitorDev)
            {//监控已开启
                if(! DevDirCurMeter::DCM_is_busy)
                {//串口空闲
                    u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
                    u8 type = 0x02;
                    //发送收到历史数据错误，重新上传上条历史数据指令
                    u8 ADR = ui->spinBox_4->value();
                    DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
                    DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_history_alarm(ADR, protocol,type);
                    m_sp_DCM.send(QByteArray((char*)protocol));
                    ui->textEdit_2->append("S:" + QString((char*)protocol) + "(收到历史数据错误，重新上传上条历史数据)");
                }
                else
                {//串口正忙
                    //开启串口定时器
                    m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
                    myHelper::ShowMessageBoxError("串口正忙，请重试");
                }
            }
            else
            {//监控已关闭
                //开启串口定时器
                m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
                myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
            }
        }
            break;
        default:
            break;
        }

        return;
    }
    //解包成功
    ui->textEdit_2->append("R:" + data + tr("(解包成功:%1)").arg(st_ydt.CID2));
    switch (cmd)
    {
    case YD_CID2_GET_INT_DATA://获取模拟量定点数
    {
        //解析结构体st_ydt中的数据
        st_DCM_int_data st_int_data;
        memset(&st_int_data, 0, sizeof(st_DCM_int_data));
        DevDirCurMeter::DCM_get_int_data(&st_int_data, &st_ydt);
        //更新到UI
        ui->label_4->setText(tr("%1").arg(st_int_data.V));//电压
        ui->label_9->setText(tr("%1").arg(st_int_data.I1));//分路1电流
        ui->label_12->setText(tr("%1").arg(st_int_data.I2));//分路2电流
        ui->label_15->setText(tr("%1").arg(st_int_data.I3));//分路3电流
        ui->label_18->setText(tr("%1").arg(st_int_data.I4));//分路4电流
        ui->label_21->setText(tr("%1").arg(st_int_data.I5));//分路5电流
        ui->label_24->setText(tr("%1").arg(st_int_data.P1));//分路1功率
        ui->label_36->setText(tr("%1").arg(st_int_data.P2));//分路2功率
        ui->label_27->setText(tr("%1").arg(st_int_data.P3));//分路3功率
        ui->label_30->setText(tr("%1").arg(st_int_data.P4));//分路4功率
        ui->label_33->setText(tr("%1").arg(st_int_data.P5));//分路5功率
        ui->label_51->setText(tr("%1").arg(st_int_data.E1));//分路1电能
        ui->label_45->setText(tr("%1").arg(st_int_data.E2));//分路2电能
        ui->label_42->setText(tr("%1").arg(st_int_data.E3));//分路3电能
        ui->label_48->setText(tr("%1").arg(st_int_data.E4));//分路4电能
        ui->label_39->setText(tr("%1").arg(st_int_data.E5));//分路5电能
    }
        break;
    case YD_CID2_GET_SWITCH_STATE://获取开关输入状态
    {
        //解析结构体st_ydt中的数据
        DevDirCurMeter::DCM_get_switch_state(&m_DCM_switch_state, &st_ydt);
        //更新到UI
        if(m_DCM_switch_state.DI0)
            ui->label_54->setPixmap(QPixmap(":/image/switch_on.png"));
        else
            ui->label_54->setPixmap(QPixmap(":/image/switch_off.png"));
        if(m_DCM_switch_state.DI1)
            ui->label_56->setPixmap(QPixmap(":/image/switch_on.png"));
        else
            ui->label_56->setPixmap(QPixmap(":/image/switch_off.png"));
        if(m_DCM_switch_state.DI2)
            ui->label_58->setPixmap(QPixmap(":/image/switch_on.png"));
        else
            ui->label_58->setPixmap(QPixmap(":/image/switch_off.png"));
        if(m_DCM_switch_state.DO0)
            ui->pushButton_11->setIcon(QIcon(":/image/switch_on.png"));
        else
            ui->pushButton_11->setIcon(QIcon(":/image/switch_off.png"));
        if(m_DCM_switch_state.DO1)
            ui->pushButton_12->setIcon(QIcon(":/image/switch_on.png"));
        else
            ui->pushButton_12->setIcon(QIcon(":/image/switch_off.png"));
        if(m_DCM_switch_state.DO2)
            ui->pushButton_13->setIcon(QIcon(":/image/switch_on.png"));
        else
            ui->pushButton_13->setIcon(QIcon(":/image/switch_off.png"));
        if(m_DCM_switch_state.DO3)
            ui->pushButton_14->setIcon(QIcon(":/image/switch_on.png"));
        else
            ui->pushButton_14->setIcon(QIcon(":/image/switch_off.png"));
    }
        break;
    case YD_CID2_GET_ALARM_STATE://获取告警信息
    {
        //解析结构体st_ydt中的数据
        st_DCM_alarm_state st_alarm_state;
        DevDirCurMeter::DCM_get_alarm_state(&st_alarm_state, &st_ydt);
        //更新到UI
        if(st_alarm_state.overV)
            ui->label_280->setPixmap(QPixmap(":/image/alert_on.png"));
        else
            ui->label_280->setPixmap(QPixmap(":/image/alert_off.png"));
        if(st_alarm_state.lackV)
            ui->label_282->setPixmap(QPixmap(":/image/alert_on.png"));
        else
            ui->label_282->setPixmap(QPixmap(":/image/alert_off.png"));
        if(st_alarm_state.overI)
            ui->label_284->setPixmap(QPixmap(":/image/alert_on.png"));
        else
            ui->label_284->setPixmap(QPixmap(":/image/alert_off.png"));
    }
        break;
    case YD_CID2_REMOTE_CTRL://遥控
    {
        if(0 == m_DCM_switch_state.DOx)
        {
            if(m_DCM_switch_state.DO0)
                ui->pushButton_11->setIcon(QIcon(":/image/switch_on.png"));
            else
                ui->pushButton_11->setIcon(QIcon(":/image/switch_off.png"));
        }
        else if(1 == m_DCM_switch_state.DOx)
        {
            if(m_DCM_switch_state.DO1)
                ui->pushButton_12->setIcon(QIcon(":/image/switch_on.png"));
            else
                ui->pushButton_12->setIcon(QIcon(":/image/switch_off.png"));
        }
        else if(2 == m_DCM_switch_state.DOx)
        {
            if(m_DCM_switch_state.DO2)
                ui->pushButton_13->setIcon(QIcon(":/image/switch_on.png"));
            else
                ui->pushButton_13->setIcon(QIcon(":/image/switch_off.png"));
        }
        else if(3 == m_DCM_switch_state.DOx)
        {
            if(m_DCM_switch_state.DO3)
                ui->pushButton_14->setIcon(QIcon(":/image/switch_on.png"));
            else
                ui->pushButton_14->setIcon(QIcon(":/image/switch_off.png"));
        }
    }
        break;
    case YD_CID2_GET_SYS_HISTORY_INT://DCM获取系统历史数据（定点数）
    {
        //解析结构体st_ydt中的数据
        char timeStr[20] = {0};
        st_DCM_sys_history_int st_sys_his_int;
        DevDirCurMeter::DCM_get_sys_history_int(&st_sys_his_int, &st_ydt);
        YD_T_1363_3_2005::time2str(st_sys_his_int.time, timeStr);
        //更新到UI
        int row_count = ui->tableWidget_4->rowCount();
        QTableWidgetItem *itemCount = new QTableWidgetItem(tr("%1").arg(row_count + 1));
        QTableWidgetItem *itemV = new QTableWidgetItem(tr("%1V").arg(st_sys_his_int.st_int_data.V));
        QTableWidgetItem *itemI1 = new QTableWidgetItem(tr("%1A").arg(st_sys_his_int.st_int_data.I1));
        QTableWidgetItem *itemI2 = new QTableWidgetItem(tr("%1A").arg(st_sys_his_int.st_int_data.I2));
        QTableWidgetItem *itemI3 = new QTableWidgetItem(tr("%1A").arg(st_sys_his_int.st_int_data.I3));
        QTableWidgetItem *itemI4 = new QTableWidgetItem(tr("%1A").arg(st_sys_his_int.st_int_data.I4));
        QTableWidgetItem *itemI5 = new QTableWidgetItem(tr("%1A").arg(st_sys_his_int.st_int_data.I5));
        QTableWidgetItem *itemP1 = new QTableWidgetItem(tr("%1W").arg(st_sys_his_int.st_int_data.P1));
        QTableWidgetItem *itemP2 = new QTableWidgetItem(tr("%1W").arg(st_sys_his_int.st_int_data.P2));
        QTableWidgetItem *itemP3 = new QTableWidgetItem(tr("%1W").arg(st_sys_his_int.st_int_data.P3));
        QTableWidgetItem *itemP4 = new QTableWidgetItem(tr("%1W").arg(st_sys_his_int.st_int_data.P4));
        QTableWidgetItem *itemP5 = new QTableWidgetItem(tr("%1W").arg(st_sys_his_int.st_int_data.P5));
        QTableWidgetItem *itemE1 = new QTableWidgetItem(tr("%1KWH").arg(st_sys_his_int.st_int_data.E1));
        QTableWidgetItem *itemE2 = new QTableWidgetItem(tr("%1KWH").arg(st_sys_his_int.st_int_data.E2));
        QTableWidgetItem *itemE3 = new QTableWidgetItem(tr("%1KWH").arg(st_sys_his_int.st_int_data.E3));
        QTableWidgetItem *itemE4 = new QTableWidgetItem(tr("%1KWH").arg(st_sys_his_int.st_int_data.E4));
        QTableWidgetItem *itemE5 = new QTableWidgetItem(tr("%1KWH").arg(st_sys_his_int.st_int_data.E5));
        QTableWidgetItem *itemTime = new QTableWidgetItem(tr("%1").arg(timeStr));
        ui->tableWidget_4->insertRow(row_count + 1);
        ui->tableWidget_4->setItem(row_count + 1, 0, itemCount);
        ui->tableWidget_4->setItem(row_count + 1, 1, itemV);
        ui->tableWidget_4->setItem(row_count + 1, 2, itemI1);
        ui->tableWidget_4->setItem(row_count + 1, 3, itemI2);
        ui->tableWidget_4->setItem(row_count + 1, 4, itemI3);
        ui->tableWidget_4->setItem(row_count + 1, 5, itemI4);
        ui->tableWidget_4->setItem(row_count + 1, 6, itemI5);
        ui->tableWidget_4->setItem(row_count + 1, 7, itemP1);
        ui->tableWidget_4->setItem(row_count + 1, 8, itemP2);
        ui->tableWidget_4->setItem(row_count + 1, 9, itemP3);
        ui->tableWidget_4->setItem(row_count + 1, 10, itemP4);
        ui->tableWidget_4->setItem(row_count + 1, 11, itemP5);
        ui->tableWidget_4->setItem(row_count + 1, 12, itemE1);
        ui->tableWidget_4->setItem(row_count + 1, 13, itemE2);
        ui->tableWidget_4->setItem(row_count + 1, 14, itemE3);
        ui->tableWidget_4->setItem(row_count + 1, 15, itemE4);
        ui->tableWidget_4->setItem(row_count + 1, 16, itemE5);
        ui->tableWidget_4->setItem(row_count + 1, 17, itemTime);
        //检测是否为最后一条记录
        if(!st_sys_his_int.data_type)
        {//不是最后一条记录，获取下一条记录
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            u8 type = 0;
            if(ui->radioButton->isChecked())
                type = 0x01;
            else if(ui->radioButton_2->isChecked())
                type = 0x11;
            else if(ui->radioButton_3->isChecked())
                type = 0x21;
            if(DevDirCurMeter::DCM_startMonitorDev)
            {//监控已开启
                if(! DevDirCurMeter::DCM_is_busy)
                {//串口空闲
                    //发送获取系统历史数据（定点数）指令
                    u8 ADR = ui->spinBox_4->value();
                    DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
                    DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_sys_history_int(ADR, protocol,type);
                    m_sp_DCM.send(QByteArray((char*)protocol));
                    ui->textEdit_2->append("S:" + QString((char*)protocol) + "(收到历史数据正确，要求上送下一条历史数据)");
                }
                else
                {//串口正忙
                    //开启串口定时器
                    m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
                    myHelper::ShowMessageBoxError("串口正忙，请重试");
                }
            }
            else
            {//监控已关闭
                //开启串口定时器
                m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
                myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
            }
        }
        else
        {//已经为最后一条记录
            //开启串口定时器
            m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
        }
    }
        break;
    case YD_CID2_GET_HISTORY_ALARM://DCM获取历史告警记录
    {
        //解析结构体st_ydt中的数据
        char timeStr[20] = {0};
        st_DCM_history_alarm st_his_alarm;
        DevDirCurMeter::DCM_get_history_alarm(&st_his_alarm, &st_ydt);
        YD_T_1363_3_2005::time2str(st_his_alarm.time, timeStr);
        //更新到UI
        int row_count = ui->tableWidget_6->rowCount();
        QTableWidgetItem *itemCount = new QTableWidgetItem(tr("%1").arg(row_count + 1));
        QTableWidgetItem *itemOverV = new QTableWidgetItem();
        QTableWidgetItem *itemLackV = new QTableWidgetItem();
        QTableWidgetItem *itemOverI = new QTableWidgetItem();
        QTableWidgetItem *itemTime = new QTableWidgetItem(tr("%1").arg(timeStr));
        if(st_his_alarm.st_alarm_state.overV)//过压
            itemOverV->setIcon(QIcon(":/image/alert_off.png"));
        else
            itemOverV->setIcon(QIcon(":/image/alert_on.png"));
        if(st_his_alarm.st_alarm_state.lackV)//欠压
            itemLackV->setIcon(QIcon(":/image/alert_off.png"));
        else
            itemLackV->setIcon(QIcon(":/image/alert_on.png"));
        if(st_his_alarm.st_alarm_state.overI)//过流
            itemOverI->setIcon(QIcon(":/image/alert_off.png"));
        else
            itemOverI->setIcon(QIcon(":/image/alert_on.png"));
        ui->tableWidget_6->insertRow(row_count + 1);
        ui->tableWidget_6->setItem(row_count + 1, 0, itemCount);
        ui->tableWidget_6->setItem(row_count + 1, 1, itemOverV);
        ui->tableWidget_6->setItem(row_count + 1, 2, itemLackV);
        ui->tableWidget_6->setItem(row_count + 1, 3, itemOverI);
        ui->tableWidget_6->setItem(row_count + 1, 4, itemTime);
        //检测是否为最后一条记录
        if(!st_his_alarm.data_type)
        {//不是最后一条记录，获取下一条记录
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            u8 type = 0x01;
            if(DevDirCurMeter::DCM_startMonitorDev)
            {//监控已开启
                if(! DevDirCurMeter::DCM_is_busy)
                {//串口空闲
                    //发送获取系统历史数据（定点数）指令
                    u8 ADR = ui->spinBox_4->value();
                    DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
                    DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_history_alarm(ADR, protocol,type);
                    m_sp_DCM.send(QByteArray((char*)protocol));
                    ui->textEdit_2->append("S:" + QString((char*)protocol) + "(收到历史数据正确，要求上送下一条历史数据)");
                }
                else
                {//串口正忙
                    //开启串口定时器
                    m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
                    myHelper::ShowMessageBoxError("串口正忙，请重试");
                }
            }
            else
            {//监控已关闭
                //开启串口定时器
                m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
                myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
            }
        }
        else
        {//已经为最后一条记录
            //开启串口定时器
            m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
        }
    }
    case DCM_CID2_GET_MON_FREEZE_EE://DCM获取月冻结电能记录
    {
        int curIndex = ui->comboBox_3->currentIndex();
        if(0 != curIndex)
        {//获取单条记录
            //解析结构体st_ydt中的数据
            st_DCM_mon_freeze_ee st_mon_freeze_ee;
            DevDirCurMeter::DCM_get_mon_freeze_ee(&st_mon_freeze_ee, &st_ydt);
            //更新到UI
            if(0 != st_mon_freeze_ee.amount)
            {
                if(ui->comboBox_3->currentIndex() > st_mon_freeze_ee.amount)
                {//超出最大条数
                    myHelper::ShowMessageBoxError("没有该月份数据");
                    return;
                }
                else
                {//在最大条数內
                    QTableWidgetItem *itemMOn = new QTableWidgetItem(tr("%1").arg(curIndex));
                    QTableWidgetItem *itemE1 = new QTableWidgetItem(tr("%1KWH").arg(st_mon_freeze_ee.E1));
                    QTableWidgetItem *itemE2 = new QTableWidgetItem(tr("%1KWH").arg(st_mon_freeze_ee.E2));
                    QTableWidgetItem *itemE3 = new QTableWidgetItem(tr("%1KWH").arg(st_mon_freeze_ee.E3));
                    QTableWidgetItem *itemE4 = new QTableWidgetItem(tr("%1KWH").arg(st_mon_freeze_ee.E4));
                    QTableWidgetItem *itemE5 = new QTableWidgetItem(tr("%1KWH").arg(st_mon_freeze_ee.E5));
                    int row_count = ui->tableWidget_7->rowCount();
                    ui->tableWidget_7->insertRow(row_count + 1);
                    ui->tableWidget_7->setItem(row_count + 1, 0, itemMOn);
                    ui->tableWidget_7->setItem(row_count + 1, 1, itemE1);
                    ui->tableWidget_7->setItem(row_count + 1, 2, itemE2);
                    ui->tableWidget_7->setItem(row_count + 1, 3, itemE3);
                    ui->tableWidget_7->setItem(row_count + 1, 4, itemE4);
                    ui->tableWidget_7->setItem(row_count + 1, 5, itemE5);
                }
            }
        }
        else
        {//获取所有记录
            //解析结构体st_ydt中的数据
            st_DCM_mon_freeze_ee st_mon_freeze_ee;
            DevDirCurMeter::DCM_get_mon_freeze_ee(&st_mon_freeze_ee, &st_ydt);
            //更新到UI
            if(0 != st_mon_freeze_ee.amount)
            {
                if(m_DCMmonFreezeEEcurMon <= st_mon_freeze_ee.amount)
                {
                    QTableWidgetItem *itemMOn = new QTableWidgetItem(tr("%1").arg(curIndex));
                    QTableWidgetItem *itemE1 = new QTableWidgetItem(tr("%1KWH").arg(st_mon_freeze_ee.E1));
                    QTableWidgetItem *itemE2 = new QTableWidgetItem(tr("%1KWH").arg(st_mon_freeze_ee.E2));
                    QTableWidgetItem *itemE3 = new QTableWidgetItem(tr("%1KWH").arg(st_mon_freeze_ee.E3));
                    QTableWidgetItem *itemE4 = new QTableWidgetItem(tr("%1KWH").arg(st_mon_freeze_ee.E4));
                    QTableWidgetItem *itemE5 = new QTableWidgetItem(tr("%1KWH").arg(st_mon_freeze_ee.E5));
                    int row_count = ui->tableWidget_7->rowCount();
                    ui->tableWidget_7->insertRow(row_count + 1);
                    ui->tableWidget_7->setItem(row_count + 1, 0, itemMOn);
                    ui->tableWidget_7->setItem(row_count + 1, 1, itemE1);
                    ui->tableWidget_7->setItem(row_count + 1, 2, itemE2);
                    ui->tableWidget_7->setItem(row_count + 1, 3, itemE3);
                    ui->tableWidget_7->setItem(row_count + 1, 4, itemE4);
                    ui->tableWidget_7->setItem(row_count + 1, 5, itemE5);
                }

                if(m_DCMmonFreezeEEcurMon < st_mon_freeze_ee.amount)
                {
                    m_DCMmonFreezeEEcurMon++;
                    if(DevDirCurMeter::DCM_startMonitorDev)
                    {//监控已开启
                        if(! DevDirCurMeter::DCM_is_busy)
                        {//串口空闲
                            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
                            //发送获取月冻结电能记录指令
                            u8 ADR = ui->spinBox_4->value();
                            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
                            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_mon_freeze_ee(ADR, protocol,m_DCMmonFreezeEEcurMon);
                            m_sp_DCM.send(QByteArray((char*)protocol));
                            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(获取月冻结电能记录)");
                        }
                        else
                        {//串口正忙
                            //开启串口定时器
                            m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
                            myHelper::ShowMessageBoxError("串口正忙，请重试");
                        }
                    }
                    else
                    {//监控已关闭
                        //开启串口定时器
                        m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
                        myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
                    }
                }

                if(m_DCMmonFreezeEEcurMon == st_mon_freeze_ee.amount)
                {
                    //开启串口定时器
                    m_timerSerialPort->start(TIMER_SERIAL_PORT_MSEC);
                }
            }
        }
    }
        break;
    case DCM_CID2_GET_MON_EE://获取月能记录
    {
        //解析结构体st_ydt中的数据
        st_DCM_mon_ee st_mon_ee;
        DevDirCurMeter::DCM_get_mon_ee(&st_mon_ee, &st_ydt);
        //更新到UI
        for(u8 i = 0; i < 12; i++)
        {
            QTableWidgetItem *itemMon = new QTableWidgetItem(tr("%1").arg(12 * st_mon_ee.monGroup + 1 + i));
            QTableWidgetItem *itemEE = new QTableWidgetItem(tr("%1KWH").arg(st_mon_ee.eeArray[i]));
            int row_count = ui->tableWidget_8->rowCount();
            ui->tableWidget_8->insertRow(row_count + 1);
            ui->tableWidget_8->setItem(row_count + 1, 0, itemMon);
            ui->tableWidget_8->setItem(row_count + 1, 1, itemEE);
        }
    }
        break;
    case DCM_CID2_GET_DAY_EE://获取日能记录
    {
        //解析结构体st_ydt中的数据
        st_DCM_day_ee st_day_ee;
        DevDirCurMeter::DCM_get_day_ee(&st_day_ee, &st_ydt);
        //更新到UI
        for(u8 i = 0; i < 12; i++)
        {
            QTableWidgetItem *itemDay = new QTableWidgetItem(tr("%1").arg(12 * st_day_ee.dayGroup + 1 + i));
            QTableWidgetItem *itemEE = new QTableWidgetItem(tr("%1KWH").arg(st_day_ee.eeArray[i]));
            int row_count = ui->tableWidget_9->rowCount();
            ui->tableWidget_9->insertRow(row_count + 1);
            ui->tableWidget_9->setItem(row_count + 1, 0, itemDay);
            ui->tableWidget_9->setItem(row_count + 1, 1, itemEE);
        }
    }
    case DCM_CID2_GET_SENSOR_ARGS://获取传感器参数
    {
        //解析结构体st_ydt中的数据
        st_DCM_sensor_args st_sensor_args;
        DevDirCurMeter::DCM_get_sensor_args(&st_sensor_args, &st_ydt);
        //更新到UI
        if(st_sensor_args.sensorType)
        {//分流器
            ui->radioButton_20->setChecked(false);
            ui->radioButton_21->setChecked(true);
        }
        else
        {//霍尔电流传感器
            ui->radioButton_21->setChecked(false);
            ui->radioButton_20->setChecked(true);
        }
        ui->lineEdit->setText(tr("%1").arg(st_sensor_args.sensorPrimRange));
        ui->lineEdit_2->setText(tr("%1").arg(st_sensor_args.sensorPrimRangeMin));
        ui->lineEdit_3->setText(tr("%1").arg(st_sensor_args.sensorPrimRangeMax));
    }
        break;
    default:
        break;
    }
}

//DCM监控开关
void MainWindow::on_pushButton_56_clicked()
{
    if(m_sp_DCM.isSerialPortAvailable())
    {//串口已打开
        DevDirCurMeter::DCM_startMonitorDev = !(DevDirCurMeter::DCM_startMonitorDev);

        if(DevDirCurMeter::DCM_startMonitorDev)
        {//关闭监控
            ui->pushButton_56->setText(tr("停止监控"));
            ui->pushButton_56->setIcon(QIcon(":/image/ball_on.ico"));
            DevDirCurMeter::DCM_is_busy = false;//释放串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = 0;
            ui->label_280->setPixmap(QPixmap(":/image/alert_unknow.png"));
            ui->label_282->setPixmap(QPixmap(":/image/alert_unknow.png"));
            ui->label_284->setPixmap(QPixmap(":/image/alert_unknow.png"));
        }
        else
        {//开启监控
            ui->pushButton_56->setText(tr("开始监控"));
            ui->pushButton_56->setIcon(QIcon(":/image/ball_off.ico"));
        }
    }
    else
    {//串口已关闭
        myHelper::ShowMessageBoxError("请先打开串口再重试");
    }
}

//DCM获取开关状态
void MainWindow::on_pushButton_54_clicked()
{
    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            //发送获取开关输入状态指令
            u8 ADR = ui->spinBox_4->value();
            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_switch_state(ADR, protocol);
            m_sp_DCM.send(QByteArray((char*)protocol));
            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(获取开关输入状态)");
        }
        else
        {//串口正忙
            myHelper::ShowMessageBoxError("串口正忙，请重试");
        }
    }
    else
    {//监控已关闭
        myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
    }
}
//DCM遥控DO0
void MainWindow::on_pushButton_11_clicked()
{
    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            m_DCM_switch_state.DO0 = !(m_DCM_switch_state.DO0);
            m_DCM_switch_state.DOx = 0;
            //发送获取开关输入状态指令
            u8 ADR = ui->spinBox_4->value();
            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_remote_ctrl(ADR, protocol,&m_DCM_switch_state);
            m_sp_DCM.send(QByteArray((char*)protocol));
            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(遥控DO0)");
        }
        else
        {//串口正忙
            myHelper::ShowMessageBoxError("串口正忙，请重试");
        }
    }
    else
    {//监控已关闭
        myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
    }
}
//DCM遥控DO1
void MainWindow::on_pushButton_12_clicked()
{
    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            m_DCM_switch_state.DO1 = !(m_DCM_switch_state.DO1);
            m_DCM_switch_state.DOx = 1;
            //发送获取开关输入状态指令
            u8 ADR = ui->spinBox_4->value();
            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_remote_ctrl(ADR, protocol,&m_DCM_switch_state);
            m_sp_DCM.send(QByteArray((char*)protocol));
            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(遥控DO1)");
        }
        else
        {//串口正忙
            myHelper::ShowMessageBoxError("串口正忙，请重试");
        }
    }
    else
    {//监控已关闭
        myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
    }
}
//DCM遥控DO2
void MainWindow::on_pushButton_13_clicked()
{
    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            m_DCM_switch_state.DO2 = !(m_DCM_switch_state.DO2);
            m_DCM_switch_state.DOx = 2;
            //发送获取开关输入状态指令
            u8 ADR = ui->spinBox_4->value();
            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_remote_ctrl(ADR, protocol,&m_DCM_switch_state);
            m_sp_DCM.send(QByteArray((char*)protocol));
            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(遥控DO2)");
        }
        else
        {//串口正忙
            myHelper::ShowMessageBoxError("串口正忙，请重试");
        }
    }
    else
    {//监控已关闭
        myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
    }
}
//DCM遥控DO3
void MainWindow::on_pushButton_14_clicked()
{
    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            m_DCM_switch_state.DO3 = !(m_DCM_switch_state.DO3);
            m_DCM_switch_state.DOx = 3;
            //发送获取开关输入状态指令
            u8 ADR = ui->spinBox_4->value();
            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_remote_ctrl(ADR, protocol,&m_DCM_switch_state);
            m_sp_DCM.send(QByteArray((char*)protocol));
            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(遥控DO3)");
        }
        else
        {//串口正忙
            myHelper::ShowMessageBoxError("串口正忙，请重试");
        }
    }
    else
    {//监控已关闭
        myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
    }
}

//DCM获取系统历史数据（定点数）
void MainWindow::on_pushButton_15_clicked()
{
    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            //关闭串口定时器
            m_timerSerialPort->stop();
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            u8 type = 0;
            if(ui->radioButton->isChecked())
                type = 0x00;
            else if(ui->radioButton_2->isChecked())
                type = 0x10;
            else if(ui->radioButton_3->isChecked())
                type = 0x20;
            //清空UI
            int rowCount = ui->tableWidget_4->rowCount();
            for(int i = 0; i < rowCount; i++)
            {
                ui->tableWidget_4->removeRow(0);
            }
            //发送获取系统历史数据（定点数）指令
            u8 ADR = ui->spinBox_4->value();
            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_sys_history_int(ADR, protocol,type);
            m_sp_DCM.send(QByteArray((char*)protocol));
            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(获取系统历史数据（定点数）)");
        }
        else
        {//串口正忙
            myHelper::ShowMessageBoxError("串口正忙，请重试");
        }
    }
    else
    {//监控已关闭
        myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
    }
}

/*DCM获取历史告警记录*/
void MainWindow::on_pushButton_17_clicked()
{
    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            //关闭串口定时器
            m_timerSerialPort->stop();
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            u8 type = 0x00;
            //清空UI
            int rowCount = ui->tableWidget_6->rowCount();
            for(int i = 0; i < rowCount; i++)
            {
                ui->tableWidget_6->removeRow(0);
            }
            //发送获取历史告警记录指令
            u8 ADR = ui->spinBox_4->value();
            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_history_alarm(ADR, protocol,type);
            m_sp_DCM.send(QByteArray((char*)protocol));
            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(获取历史告警记录)");
        }
        else
        {//串口正忙
            myHelper::ShowMessageBoxError("串口正忙，请重试");
        }
    }
    else
    {//监控已关闭
        myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
    }
}

/*DCM获取月冻结电能记录*/
void MainWindow::on_pushButton_18_clicked()
{
    int curIndex = ui->comboBox_3->currentIndex();

    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            if(0 != curIndex)
            {//获取单条记录
                u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
                //清空UI
                int rowCount = ui->tableWidget_7->rowCount();
                for(int i = 0; i < rowCount; i++)
                {
                    ui->tableWidget_7->removeRow(0);
                }
                //发送获取月冻结电能记录指令
                u8 ADR = ui->spinBox_4->value();
                DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
                DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_mon_freeze_ee(ADR, protocol,curIndex);
                m_sp_DCM.send(QByteArray((char*)protocol));
                ui->textEdit_2->append("S:" + QString((char*)protocol) + "(获取月冻结电能记录)");
            }
            else
            {//获取所有记录
                //关闭串口定时器
                m_timerSerialPort->stop();
                m_DCMmonFreezeEEcurMon = 1;
                u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
                //清空UI
                int rowCount = ui->tableWidget_7->rowCount();
                for(int i = 0; i < rowCount; i++)
                {
                    ui->tableWidget_7->removeRow(0);
                }
                //发送获取月冻结电能记录指令
                u8 ADR = ui->spinBox_4->value();
                DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
                DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_mon_freeze_ee(ADR, protocol,0x01);
                m_sp_DCM.send(QByteArray((char*)protocol));
                ui->textEdit_2->append("S:" + QString((char*)protocol) + "(获取月冻结电能记录)");
            }
        }
        else
        {//串口正忙
            myHelper::ShowMessageBoxError("串口正忙，请重试");
        }
    }
    else
    {//监控已关闭
        myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
    }
}

/*DCM获取月电能记录*/
void MainWindow::on_pushButton_20_clicked()
{
    u8 Ex = ui->comboBox_4->currentIndex() + 1;
    u8 monGroup = 0;

    if(ui->radioButton_13->isChecked())
        monGroup = 0;
    else
        monGroup = 1;

    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            //清空UI
            int rowCount = ui->tableWidget_8->rowCount();
            for(int i = 0; i < rowCount; i++)
            {
                ui->tableWidget_8->removeRow(0);
            }
            //发送获取月电能记录指令
            u8 ADR = ui->spinBox_4->value();
            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_mon_ee(ADR, protocol,Ex,monGroup);
            m_sp_DCM.send(QByteArray((char*)protocol));
            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(获取月电能记录)");
        }
        else
        {//串口正忙
            myHelper::ShowMessageBoxError("串口正忙，请重试");
        }
    }
    else
    {//监控已关闭
        myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
    }
}
/*DCM获取日电能记录*/
void MainWindow::on_pushButton_19_clicked()
{
    u8 Ex = ui->comboBox_5->currentIndex() + 1;
    u8 dayGroup = 0;

    if(ui->radioButton_15->isChecked())
        dayGroup = 0;
    else if(ui->radioButton_16->isChecked())
        dayGroup = 1;
    else if(ui->radioButton_17->isChecked())
        dayGroup = 2;
    else if(ui->radioButton_18->isChecked())
        dayGroup = 3;
    else if(ui->radioButton_19->isChecked())
        dayGroup = 4;
    else
        dayGroup = 0;

    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            //清空UI
            int rowCount = ui->tableWidget_9->rowCount();
            for(int i = 0; i < rowCount; i++)
            {
                ui->tableWidget_9->removeRow(0);
            }
            //发送获取日电能记录指令
            u8 ADR = ui->spinBox_4->value();
            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_day_ee(ADR, protocol,Ex,dayGroup);
            m_sp_DCM.send(QByteArray((char*)protocol));
            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(获取日电能记录)");
        }
        else
        {//串口正忙
            myHelper::ShowMessageBoxError("串口正忙，请重试");
        }
    }
    else
    {//监控已关闭
        myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
    }
}

/*DCM获取电表传感器参数*/
void MainWindow::on_pushButton_21_clicked()
{
    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            //发送获取电表传感器参数指令
            u8 ADR = ui->spinBox_4->value();
            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_sensor_args(ADR, protocol);
            m_sp_DCM.send(QByteArray((char*)protocol));
            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(获取电表传感器参数)");
        }
        else
        {//串口正忙
            myHelper::ShowMessageBoxError("串口正忙，请重试");
        }
    }
    else
    {//监控已关闭
        myHelper::ShowMessageBoxError("监控已关闭，请先开启再重试");
    }
}

/*DCM设置电表传感器参数*/
void MainWindow::on_pushButton_22_clicked()
{
    st_DCM_sensor_args st_sensor_args;

    if(ui->radioButton_20->isChecked())
        st_sensor_args.sensorType = 0;
    else
        st_sensor_args.sensorType = 1;

    bool ok = false;
    st_sensor_args.sensorPrimRange = ui->lineEdit->text().trimmed().toInt(&ok, 10);
    if(!ok)
    {
        myHelper::ShowMessageBoxError("设置失败，参数错误");
        return;
    }
   st_sensor_args.sensorPrimRangeMin = ui->lineEdit_2->text().trimmed().toInt(&ok, 10);
   if(!ok)
   {
       myHelper::ShowMessageBoxError("设置失败，参数错误");
       return;
   }
   st_sensor_args.sensorPrimRangeMax = ui->lineEdit_3->text().trimmed().toInt(&ok, 10);
   if(!ok)
   {
       myHelper::ShowMessageBoxError("设置失败，参数错误");
       return;
   }
}
/***********************************DCM end******************************/

/*串口定时器槽*/
void MainWindow::on_timerSerialPort_timeout()
{
    /**************************DCM设备 start***********************************/
    /*发送获取定点数指令*/
    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            u8 ADR = ui->spinBox_4->value();
            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_int_data(ADR, protocol);
            m_sp_DCM.send(QByteArray((char*)protocol));
            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(获取模拟量定点数)");
        }
        else
        {//串口正忙
            qDebug() << "DCM 获取模拟量定点数 失败 串口正忙";
            return;
        }
    }
    else
    {//监控已关闭
        qDebug() << "DCM 获取模拟量定点数 失败 监控已关闭";
        return;
    }

    //延时等待接收完
    Global::sleep(500);

    /*发送获取告警状态指令*/
    if(DevDirCurMeter::DCM_startMonitorDev)
    {//监控已开启
        if(! DevDirCurMeter::DCM_is_busy)
        {//串口空闲
            u8 protocol[YD_MAX_LEN_PACK + 1] = {0};
            u8 ADR = ui->spinBox_4->value();
            DevDirCurMeter::DCM_is_busy = true;//设置串口正忙标志
            DevDirCurMeter::DCM_cur_cmd = DevDirCurMeter::DCM_cmd_get_alarm_state(ADR, protocol);
            m_sp_DCM.send(QByteArray((char*)protocol));
            ui->textEdit_2->append("S:" + QString((char*)protocol) + "(获取告警状态)");
        }
        else
        {//串口正忙
            qDebug() << "DCM 获取告警状态 失败 串口正忙";
            return;
        }
    }
    else
    {//监控已关闭
        qDebug() << "DCM 获取告警状态 失败 监控已关闭";
        return;
    }
    /**************************DCM设备 end***********************************/
}
/***********************************串口 end******************************/



