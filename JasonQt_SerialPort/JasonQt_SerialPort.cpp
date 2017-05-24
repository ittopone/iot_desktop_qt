#include "JasonQt_SerialPort.h"

// JasonQt_SerialPort_run
JasonQt_SerialPort_run::JasonQt_SerialPort_run(JasonQt_SerialPort *parent)
{
    m_parent = parent;
    m_serialPort = NULL;
}

void JasonQt_SerialPort_run::open(const QString &portName, const int &baudRate, const QSerialPort::DataBits &dataBits, const QSerialPort::Parity &parity, const QSerialPort::StopBits &stopBits)
{
    if(m_serialPort)
    {
        this->close();
    }

    m_serialPort = new QSerialPort(portName);

#ifndef Q_OS_WIN
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(dataBits);
    m_serialPort->setParity(parity);
    m_serialPort->setStopBits(stopBits);
#endif

    connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(accepted()), Qt::DirectConnection);
    connect(m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(error(QSerialPort::SerialPortError)), Qt::DirectConnection);

    if(!m_serialPort->open(QIODevice::ReadWrite))
    {
        emit controlOpened(false);
        return;
    }

#ifdef Q_OS_WIN
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(dataBits);
    m_serialPort->setParity(parity);
    m_serialPort->setStopBits(stopBits);
#endif

    emit controlOpened(true);
}

void JasonQt_SerialPort_run::close(void)
{
    if(m_serialPort)
    {
        m_serialPort->close();
        m_serialPort->deleteLater();
        m_serialPort = NULL;
    }
}

void JasonQt_SerialPort_run::send(const QByteArray &data)
{
    if(m_serialPort)
    {
        m_serialPort->write(data);
    }
}

void JasonQt_SerialPort_run::accepted(void)
{
    if(m_serialPort)
    {
        emit m_parent->accepted(m_serialPort->readAll());
    }
}

void JasonQt_SerialPort_run::error(const QSerialPort::SerialPortError &error)
{
    if(error)
    {
        emit m_parent->error(error);
    }
}

//检测串口是否有效
bool JasonQt_SerialPort_run::isSerialPortAvailable()
{
    if(m_serialPort)
    {
        return m_serialPort->isOpen();
    }

    return false;
}



// JasonQt_SerialPort
JasonQt_SerialPort::JasonQt_SerialPort(void)
{
    static bool flag = true;
    if(flag)
    {
        flag = false;
        qRegisterMetaType<QSerialPort::DataBits>("QSerialPort::DataBits");
        qRegisterMetaType<QSerialPort::Parity>("QSerialPort::Parity");
        qRegisterMetaType<QSerialPort::StopBits>("QSerialPort::StopBits");
        qRegisterMetaType<QSerialPort::SerialPortError>("QSerialPort::SerialPortError");
    }

    this->start();
}

JasonQt_SerialPort::~JasonQt_SerialPort(void)
{
    this->quit();
    this->wait();
}

bool JasonQt_SerialPort::open(const QString &portName, const int &baudRate,
                              const QSerialPort::DataBits &dataBits,
                              const QSerialPort::Parity &parity,
                              const QSerialPort::StopBits &stopBits)
{
    QEventLoop eventLoop;
    bool flag = false;

    auto a = connect(m_serialPort, SIGNAL(controlOpened(bool)), &eventLoop, SLOT(quit()), Qt::DirectConnection);
    auto b = connect(m_serialPort, &JasonQt_SerialPort_run::controlOpened, [&](const bool &isOpen){ flag = isOpen; });

    emit controlOpen(portName, baudRate, dataBits, parity, stopBits);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    disconnect(a);
    disconnect(b);

    return flag;
}

void JasonQt_SerialPort::close(void)
{
    emit controlClose();
}

void JasonQt_SerialPort::send(const QByteArray &data)
{
    emit controlSend(data);
}

void JasonQt_SerialPort::run(void)
{
    m_serialPort = new JasonQt_SerialPort_run(this);

    connect(this, SIGNAL(controlOpen(QString,int,QSerialPort::DataBits,QSerialPort::Parity,QSerialPort::StopBits)),
            m_serialPort, SLOT(open(QString,int,QSerialPort::DataBits,QSerialPort::Parity,QSerialPort::StopBits)));
    connect(this, SIGNAL(controlClose()), m_serialPort, SLOT(close()));
    connect(this, SIGNAL(controlSend(QByteArray)), m_serialPort, SLOT(send(QByteArray)));

    this->exec();

    delete m_serialPort;
    m_serialPort = NULL;
}

//检测串口是否有效
bool JasonQt_SerialPort::isSerialPortAvailable()
{
    return m_serialPort->isSerialPortAvailable();
}
