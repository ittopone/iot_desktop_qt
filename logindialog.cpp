#include "logindialog.h"
#include "ui_logindialog.h"
#include "iconhelper.h"
#include "myhelper.h"
#include "mainwindow.h"

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

void LoginDialog::on_btnOk_clicked()
{
    MainWindow * mainWindow = new MainWindow;
    this->hide();
    mainWindow->show();
}
