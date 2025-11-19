#include "sub.h"
#include "ui_sub.h"

sub::sub(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sub)
{
    ui->setupUi(this);
}

sub::~sub()
{
    delete ui;
}
