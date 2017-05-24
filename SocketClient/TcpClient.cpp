#include "TcpClient.h"
#include <QDebug>
#include "global.h"

#define SOCK_WRITE_TIMEOUT 1000
#define SOCK_READ_TIMEOUT 1000

TcpClient::TcpClient(QObject *parent) : QObject(parent)
{
    m_isConnected = false;

    m_tcpsocket = new QTcpSocket(this);
    m_tcpsocket->connectToHost("120.77.73.31", 8900);
    connect(m_tcpsocket, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(m_tcpsocket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
}

bool TcpClient::sendMessage(QByteArray bytes)
{
    if(!m_isConnected)
        return false;

    if(-1 != m_tcpsocket->write(bytes, bytes.length()))
    {
        if(!m_tcpsocket->waitForBytesWritten(SOCK_WRITE_TIMEOUT))
        {
            qDebug() << "client write timeout!";
            return false;
        }
        else
        {
            qDebug() << "send message:" << bytes;
            return true;
        }
    }

    qDebug() << "client write error!";
    return false;
}

bool TcpClient::readMessage(QByteArray &bytes)
{
    if(!m_isConnected)
        return false;

    if(!m_tcpsocket->waitForReadyRead(SOCK_READ_TIMEOUT))
    {
        qDebug() << "client read timeout!";

        bytes.clear();

        return false;
    }

    bytes = m_tcpsocket->readAll();

    qDebug() << "read message:" << bytes;

    return true;
}

void TcpClient::test()
{
    Global::sleep(2000);

    QByteArray bytes;
    this->sendMessage("hellow world!");
    this->readMessage(bytes);
}

void TcpClient::on_connected()
{
    qDebug() << "client connected!";

    m_isConnected = true;
}

void TcpClient::on_disconnected()
{
    qDebug() << "client disconnected!";

    m_isConnected = false;

    disconnect(m_tcpsocket, SIGNAL(connected()), this, SLOT(on_connected()));
    disconnect(m_tcpsocket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
    m_tcpsocket->abort();

    m_tcpsocket = new QTcpSocket(this);
    m_tcpsocket->connectToHost("120.77.73.31", 8900);
    connect(m_tcpsocket, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(m_tcpsocket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
}

