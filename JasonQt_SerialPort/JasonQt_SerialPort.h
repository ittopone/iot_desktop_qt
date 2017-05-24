#ifndef __JasonQt_Serialport_h__
#define __JasonQt_Serialport_h__

// Qt lib import
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QEventLoop>

class JasonQt_SerialPort;

class JasonQt_SerialPort_run: public QObject
{
    Q_OBJECT

private:
    JasonQt_SerialPort *m_parent = NULL;
    QSerialPort *m_serialPort = NULL;

public:
    JasonQt_SerialPort_run(JasonQt_SerialPort *parent);
    bool isSerialPortAvailable();//检测串口是否有效

public slots:
    void open(const QString &portName, const int &baudRate, const QSerialPort::DataBits &dataBits, const QSerialPort::Parity &parity, const QSerialPort::StopBits &stopBits);

    void close(void);

    void send(const QByteArray &data);

private slots:
    void accepted(void);

    void error(const QSerialPort::SerialPortError &error);

signals:
    void controlOpened(const bool isOpen);
};

class JasonQt_SerialPort: public QThread
{
    Q_OBJECT

private:
    JasonQt_SerialPort_run *m_serialPort = NULL;

public:
    JasonQt_SerialPort(void);

    ~JasonQt_SerialPort(void);

    bool isSerialPortAvailable();//检测串口是否有效

public slots:
    bool open(const QString &portName, const int &baudRate,
              const QSerialPort::DataBits &dataBits = QSerialPort::Data8,
              const QSerialPort::Parity &parity = QSerialPort::NoParity,
              const QSerialPort::StopBits &stopBits = QSerialPort::OneStop);

    void close(void);

    void send(const QByteArray &data);

private:
    void run(void);

signals:
    void accepted(const QByteArray data);

    void error(const QSerialPort::SerialPortError error);

    void controlOpen(const QString portName, const int baudRate, const QSerialPort::DataBits dataBits, const QSerialPort::Parity parity, QSerialPort::StopBits stopBits);

    void controlClose(void);

    void controlSend(const QByteArray data);
};

#endif//__JasonQt_Serialport_h__
