#include "spcomm.h"
#include <QDebug>
#include <QTime>
#include <QCoreApplication>
#include <QSerialPortInfo>

#if EN_ERROR_WIN_NOTIFY
#include "myhelper.h"
#endif

/*****************************************全局变量初始化********************************************/
//串口对象
QSerialPort * SPComm::m_serialPort = new QSerialPort();
//串口名称
QString SPComm::m_portName = "";
//波特率
int SPComm::m_baudRate = 9600;
/*************************************************************************************************/

SPComm::SPComm(QObject *parent) : QObject(parent)
{

}

//析构的时候  删除 数据
SPComm::~SPComm()
{
    delete m_serialPort;
}

//获取有效的串口名称
void SPComm::getAvailablePortName(QVector<QString>& comVector)
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        comVector.append(info.portName());
        qDebug() << tr("Name:") << info.portName();
        qDebug() << tr("Description:") << info.description();
        qDebug() << tr("Manufacturer:") << info.manufacturer();
        qDebug() << tr("Serial Number:") << info.serialNumber();
        qDebug() << tr("System Location:") << info.systemLocation();
    }
}

//获取波特率
static QSerialPort::BaudRate getBaudRate(int baudRate)
{
    switch (baudRate)
    {
    case 1200:
        return QSerialPort::Baud1200;
    case 2400:
        return QSerialPort::Baud2400;
    case 4800:
        return QSerialPort::Baud4800;
    case 9600:
        return QSerialPort::Baud9600;
    case 19200:
        return QSerialPort::Baud19200;
    case 38400:
        return QSerialPort::Baud38400;
    case 57600:
        return QSerialPort::Baud57600;
    case 115200:
        return QSerialPort::Baud115200;
    default:
        return QSerialPort::UnknownBaud;
    }
}

//这个函数用来设置串口的名字
//window下用"com1"
//linux下用"/dev/ttyS0"
void SPComm::setPortName(const QString &name)
{
    m_portName = name;
}

//用来获取串口的名字
QString SPComm::portName() const
{
    return m_portName;
}

//设置波特率 9600  19200  38400
void SPComm::setBaudRate(int baudRate)
{
    m_baudRate = baudRate;
}

//用来打开串口，调用前，先设置串口名字和波特率
bool SPComm::open()
{
    if (m_serialPort->isOpen())
    {
        return true;
    }

    m_serialPort->setPortName(m_portName);
    m_serialPort->setBaudRate(getBaudRate(m_baudRate));
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort->setReadBufferSize(1024);

    return m_serialPort->open(QIODevice::ReadWrite);
}

//用来关闭串口
void SPComm::close()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
    }
}

//重启串口,清除数据
bool SPComm::clear()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->clear();
        SPComm::close();
        return SPComm::open();
    }

    return false;
}

//用来接收串口发来的数据，需要指定接收的数据长度
int SPComm::readData(char *buffer, int count, int timeout)
{
    int len = 0;
    forever
    {
        int n = m_serialPort->read(&buffer[len], count - len);
        if (n == -1)
        {
            #if EN_ERROR_WIN_NOTIFY
            myHelper::ShowMessageBoxError(QObject::tr("读取串口数据失败"));
            #endif

            return -1;
        }
        else if (n == 0 && !m_serialPort->waitForReadyRead(timeout))
        {
            #if EN_ERROR_WIN_NOTIFY
            myHelper::ShowMessageBoxError(QObject::tr("读取串口数据超时"));
            #endif
            return -2;
        }
        else
        {
            len += n;
            if (count == len)
            {
                break;
            }
        }
    }

    return len;

}

//延时（延时期间可做其他事情）
void SPComm::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

}

//读取串口返回的所有数据，不指定数据长度
bool SPComm::readAllData(char *buffer, int timeout)
{
    QByteArray byteArray;
    forever
    {
        if(m_serialPort->waitForReadyRead(timeout))
        {
            byteArray.append(m_serialPort->readAll());
            if(byteArray.contains("\r"))
            {
                qDebug() << "SPComm get : " << byteArray;
                strcpy(buffer, byteArray.data());
                return true;
            }
        }
        else
        {
            #if EN_ERROR_WIN_NOTIFY
            myHelper::ShowMessageBoxError(QObject::tr("读取串口数据超时"));
            #endif

            return false;
        }
    }
}

//发送数据到串口  比如发送协议
int SPComm::writeData(char *data, int size)
{
    int len = 0;
    forever
    {
        long long n = m_serialPort->write(&data[len], size - len);
        if (n == -1)
        {
            #if EN_ERROR_WIN_NOTIFY
            myHelper::ShowMessageBoxError(QObject::tr("写入串口数据失败"));
            #endif

            return -1;
        }
        else
        {
            len += n;
            if (size == len)
            {
                break;
            }
        }
    }

    return len;
}

//串口发送和接收数据
int SPComm::writeAndReadData(char * data, size_t * dataLen, size_t dataDefLen, unsigned int timeout)
{
    //串口发送
    if(SPComm::writeData(data, *dataLen) < 0)
        return -1;
    //串口接收
    memset(data, 0, dataDefLen);
    if(!SPComm::readAllData(data, timeout))
        return -2;
    return 0;
}

