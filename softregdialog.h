#ifndef SOFTREGDIALOG_H
#define SOFTREGDIALOG_H

#include <QDialog>

namespace Ui {
class SoftRegDialog;
}

class SoftRegDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SoftRegDialog(QWidget *parent = 0);
    ~SoftRegDialog();

private:
    Ui::SoftRegDialog *ui;

protected:
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private slots:
    void on_btnOk_clicked();

private:
    QPoint m_mousePoint;//鼠标拖动自定义标题栏时的坐标
    bool m_mousePressed;//鼠标是否按下
};

#endif // SOFTREGDIALOG_H
