#include "logindialog.h"
#include "ui_logindialog.h"
#include "iconhelper.h"
#include "myhelper.h"
#include "mainwindow.h"
#include "global.h"
#include "softregdialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    //关乎界面
    myHelper::FormInCenter(this);

    //安装事件监听器,让标题栏识别鼠标双击
    ui->lab_Title->installEventFilter(this);

    //设置窗体标题栏隐藏
    this->setWindowFlags(Qt::FramelessWindowHint);
    //设置窗体关闭时自动释放内存
    this->setAttribute(Qt::WA_DeleteOnClose);
    //设置窗口左上角图标
    //IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf015), 12);
    //设置菜单按钮图标
    //IconHelper::Instance()->SetIcon(ui->btnMenu, QChar(0xf0c9), 10);
    //设置关闭按钮图标
    IconHelper::Instance()->SetIcon(ui->btnMenu_Close, QChar(0xf00d), 10);
    //设置样式
    this->setStyleSheet(".QWidget{border:1px solid #1B89CA;border-radius:0px;}");
    //关联关闭按钮
    this->m_mousePressed = false;
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));
    //窗体居中显示
    myHelper::FormInCenter(this);
    //菜单初始化
    this->initMenu();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mousePressed && (e->buttons() && Qt::LeftButton))
    {
        this->move(e->globalPos() - m_mousePoint);
        e->accept();
    }
}

void LoginDialog::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_mousePressed = true;
        m_mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void LoginDialog::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

    m_mousePressed = false;
}

//初始化菜单
void LoginDialog::initMenu()
{
    //检测软件是否已经注册
    this->detectSoftRegister();

    m_menu = new QMenu();

    if(Global::m_isRegister)
        m_softReg = new QAction(tr("已注册"), this);
    else
        m_softReg = new QAction(tr("软件注册"), this);

    m_menu->addAction(m_softReg);
    ui->btnMenu->setMenu(m_menu);

    #if 0
    //去除小箭头
    ui->btnMenu->setStyleSheet("QPushButton::menu-indicator{image:none}");
    #else
    //采用系统默认菜单按钮图标，并将其居中显示
    ui->btnMenu->setStyleSheet("QPushButton::menu-indicator{subcontrol-position:center;subcontrol-origin: padding;}");
    #endif

    //软件注册菜单动作触发
    connect(this->m_softReg, SIGNAL(triggered()), this, SLOT(on_action_softReg()));
}
//检测软件是否已经注册
void LoginDialog::detectSoftRegister()
{
    //读注册码并进行校验
    QFile fileSoftReg("registerCode.ini");
    if (fileSoftReg.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&fileSoftReg);
        for(int lineNum = 0; !in.atEnd(); lineNum++)
        {
            QString registerCode = in.readLine();
            qDebug() << "registerCode:" << registerCode;
            if(0 == lineNum)
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

                QStringList strList = registerCode.split("-");
                QString inputMD5;
                for(int i=0; i < strList.size(); i++)
                {
                    inputMD5.append(strList.at(i));
                }

                qDebug() << tr("input md5:") << inputMD5;

                if(inputMD5.isEmpty()||inputMD5.isNull())
                {
                    Global::m_isRegister = false;
                }

                if(inputMD5 != md5)
                {
                    Global::m_isRegister = false;
                }
                else
                {
                    Global::m_isRegister = true;
                }
            }
        }

        fileSoftReg.close();
    }
}
//软件注册（槽）
void LoginDialog::on_action_softReg()
{
    if(Global::m_isRegister)
        return;//软件已注册

    //打开软件注册窗口
    SoftRegDialog * softRegDlg = new SoftRegDialog();
    softRegDlg->exec();//显示窗口，并将该窗口设置为活动窗口

    if(Global::m_isRegister)
    {
        m_softReg->setText(tr("已注册"));
    }
}
void LoginDialog::on_btnOk_clicked()
{
    if(! Global::m_isRegister)
    {
        myHelper::ShowMessageBoxError("请先注册软件！");
        return;
    }
    if("admin" != ui->lineEdit_2->text())
    {
        myHelper::ShowMessageBoxError("昵称错误！");
        return;
    }
    if("12345678" != ui->lineEdit_3->text())
    {
        myHelper::ShowMessageBoxError("密码错误！");
        return;
    }

    MainWindow * mainWindow = new MainWindow;
    this->hide();
    mainWindow->show();
}
