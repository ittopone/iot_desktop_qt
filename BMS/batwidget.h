#ifndef BATWIDGET_H
#define BATWIDGET_H

#include <QWidget>

class BatWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BatWidget(QWidget *parent = 0);

signals:

public slots:

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // BATWIDGET_H
