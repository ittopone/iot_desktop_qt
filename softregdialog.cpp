#include "softregdialog.h"
#include "ui_softregdialog.h"
#include "iconhelper.h"
#include "myhelper.h"
#include "global.h"

SoftRegDialog::SoftRegDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SoftRegDialog)
{
    ui->setupUi(this);

    this->m_mousePressed = false;
    //设置窗体标题栏隐藏
    this->setWindowFlags(Qt::FramelessWindowHint);
    //设置窗体关闭时自动释放内存
    this->setAttribute(Qt::WA_DeleteOnClose);
    //设置窗口左上角图标
    //IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf015), 12);
    //设置关闭按钮图标
    IconHelper::Instance()->SetIcon(ui->btnMenu_Close, QChar(0xf00d), 10);
    //设置样式
    this->setStyleSheet(".QWidget{border:1px solid #1B89CA;border-radius:0px;}");
    //关联关闭按钮
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));
    //窗体居中显示
    myHelper::FormInCenter(this);
}

SoftRegDialog::~SoftRegDialog()
{
    delete ui;
}


void SoftRegDialog::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mousePressed && (e->buttons() && Qt::LeftButton))
    {
        this->move(e->globalPos() - m_mousePoint);
        e->accept();
    }
}

void SoftRegDialog::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_mousePressed = true;
        m_mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void SoftRegDialog::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

    m_mousePressed = false;
}

void SoftRegDialog::on_btnOk_clicked()
{
    QString cpuid;
    Global::getCPUID(cpuid);

    cpuid.append("yjw");
    QByteArray byte_array;
    byte_array.append(cpuid);
    qDebug() << tr("about to code to md5:") << byte_array;
    QByteArray hash_byte_array = QCryptographicHash::hash(byte_array, QCryptographicHash::Md5);
    QString md5 = hash_byte_array.toHex();

    qDebug() << tr("md5:") << md5;

    QString regCode = ui->lineEdit->text().trimmed();
    QStringList strList = regCode.split("-");
    QString inputMD5;
    for(int i=0; i < strList.size(); i++)
    {
        inputMD5.append(strList.at(i));
    }

    qDebug() << tr("input md5:") << inputMD5;

    if(inputMD5.isEmpty()||inputMD5.isNull())
    {
        myHelper::ShowMessageBoxError(tr("注册码不能为空！"));
        Global::m_isRegister = false;
        return;
    }

    if(inputMD5 != md5)
    {
        myHelper::ShowMessageBoxError(tr("注册码错误！"));
        Global::m_isRegister = false;
        return;
    }

    Global::m_isRegister = true;

    myHelper::ShowMessageBoxInfo(tr("注册成功！"));

    QFile file("registerCode.ini");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << ui->lineEdit->text().trimmed() << "\n";
        file.close();
    }

    done(1);
    this->close();
}
