#include "batwidget.h"
#include <QPainter>

BatWidget::BatWidget(QWidget *parent) : QWidget(parent)
{

}


void BatWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter;
    painter.begin(this);

    //显示图片
    painter.drawPixmap(20, 20, QPixmap(":/image/alert_on.png"));
    //显示文字
    painter.setFont(QFont("Arial", 10));
    painter.setPen(Qt::green);

    QRect font_rect(20,20,60,60);
    painter.drawText(font_rect, Qt::AlignCenter, "helloworld");

    painter.end();
}
