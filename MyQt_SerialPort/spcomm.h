#ifndef SPCOMM_H
#define SPCOMM_H

#include <QObject>
#include <QVector>
#include <QtSerialPort/QSerialPort>

//通信异常时是否开启窗口提示
#define EN_ERROR_WIN_NOTIFY false

//读取串口超时返回
#define COM_READ_TIMEOUT 500

class SPComm : public QObject
{
    Q_OBJECT
public:
    explicit SPComm(QObject *parent = 0);
    ~SPComm();

    //设置串口名称
    static void setPortName(const QString &name);

    //获取有效的串口名称
    static void getAvailablePortName(QVector<QString> & comVector);

    //用来获取串口的名字
    QString portName() const;

    //设置串口波特率
    static void setBaudRate(int baudRate);

    //用来打开串口，调用前，先设置串口名字和波特率
    static bool open();

    //用来关闭串口
    static void close();

    //重启串口,清除数据
    static bool clear();

    //count必须小于或等于串口返回的数据的长度，不然读取不到数据，返回超时信息
    static int readData(char *buffer, int count, int timeout = 1000);

    //延时（延时期间可做其他事情）
    static void sleep(unsigned int msec);

    //读取串口返回的所有数据，不指定数据长度
    static bool readAllData(char *buffer, int timeout);

    //发送数据到串口  比如发送协议
    static int writeData(char *data, int size);

    //串口发送和接收数据
    static int writeAndReadData(char * data, size_t * dataLen, size_t dataDefLen, unsigned int timeout);

public:
    //串口名称
    static QString m_portName;
    //波特率
    static int m_baudRate;
    //串口对象
    static QSerialPort * m_serialPort;
};

#endif // SPCOMM_H
