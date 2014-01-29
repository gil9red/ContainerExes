#include "USettings.h"
#include "ui_USettings.h"

/// PUBLIC
USettings::USettings( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::USettings )
{
    setWindowFlags( Qt::Dialog
                    | Qt::WindowCloseButtonHint );

    ui->setupUi( this );
    ui->tabWidgetPages->addTab( &pageVisibleColumns, "Таблица" );
    ui->tabWidgetPages->addTab( &pagePassword, "Пароль" );

    connect( ui->pButtonOk, SIGNAL( clicked() ), SLOT( accept() ) );
}

USettings::~USettings()
{
    delete ui;
}
