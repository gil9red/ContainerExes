#include "UAboutProgram.h"
#include "ui_UAboutProgram.h"

/// PUBLIC
UAboutProgram::UAboutProgram(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::UAboutProgram)
{
    setWindowFlags(Qt::WindowCloseButtonHint);

    ui->setupUi(this);

    connect(ui->pButtonClose, SIGNAL(clicked()), SLOT(close()));

    QString name = "<span style=\"font-size:14pt; font-weight:600;\">%1</span>";
    ui->labelName->setText(name.arg(qApp->applicationName()));

    QString text = "<span style=\" font-size:7pt;\">%1</span>";
    text = text.arg( "Программа хранения путей и адресов Url" );
    ui->labelDescription->setText( text );

    ui->labelVersion->setText( qApp->applicationVersion() );
}

UAboutProgram::~UAboutProgram()
{
    delete ui;
}
