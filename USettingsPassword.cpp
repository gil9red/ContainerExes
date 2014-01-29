#include "USettingsPassword.h"
#include "ui_USettingsPassword.h"

USettingsPassword::USettingsPassword(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::USettingsPassword)
{
    ui->setupUi(this);
}

USettingsPassword::~USettingsPassword()
{
    delete ui;
}
