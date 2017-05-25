#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QMenu>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private:
    Ui::LoginDialog *ui;

protected:
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    /*********************************操作界面相关***********************************/
private:
    //菜单
    QMenu * m_menu;
    //菜单选项-软件注册
    QAction * m_softReg;
    void initMenu();//初始化菜单
    void detectSoftRegister();//检测软件是否已经注册
private slots:
     void on_action_softReg();//软件注册（槽）
    /***************************************************************************/

private slots:
    void on_btnOk_clicked();

private:
    QPoint m_mousePoint;//鼠标拖动自定义标题栏时的坐标
    bool m_mousePressed;//鼠标是否按下
};

#endif // LOGINDIALOG_H
