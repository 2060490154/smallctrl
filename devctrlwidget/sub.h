#ifndef SUB_H
#define SUB_H

#include <QWidget>

namespace Ui {
class sub;
}

class sub : public QWidget
{
    Q_OBJECT

public:
    explicit sub(QWidget *parent = nullptr);
    ~sub();

private:
    Ui::sub *ui;
};

#endif // SUB_H
