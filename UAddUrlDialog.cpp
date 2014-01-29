#include "UAddUrlDialog.h"
#include "ui_UAddUrlDialog.h"

#include <QUrl>
#include <QDebug>

/// PUBLIC
UAddUrlDialog::UAddUrlDialog( QWidget * parent ) :
    QDialog( parent ),
    ui( new Ui::UAddUrlDialog )
{
    ui->setupUi( this );

    setWindowFlags( Qt::Dialog
                    | Qt::WindowCloseButtonHint );

    defaultNameUrl = ui->lEditNameUrl->text();

    connect( ui->lEditUrl, SIGNAL( textChanged( QString ) ),
             SLOT( changeLineEditUrl( QString ) ) );
    connect( ui->lEditNameUrl, SIGNAL( textEdited( QString ) ),
             SLOT( changeLineEditNameUrl( QString ) ) );

    connect( ui->pButtonOk, SIGNAL( clicked() ), SLOT( ok() ) );
    connect( ui->pButtonCancel, SIGNAL( clicked() ), SLOT( cancel() ) );
}

UAddUrlDialog::~UAddUrlDialog()
{
    delete ui;
}
QString UAddUrlDialog::url()
{
    return d_url;
}
QString UAddUrlDialog::name()
{
    return d_name;
}

/// PUBLIC SLOTS
void UAddUrlDialog::ok()
{
    d_url = ui->lEditUrl->text();
    d_name = ui->lEditNameUrl->text();

    accept();
}
void UAddUrlDialog::cancel()
{
    reject();
}

/// PRIVATE SLOTS
void UAddUrlDialog::changeLineEditUrl( QString text )
{
    Q_UNUSED( text );

    if ( ui->lEditNameUrl->text().isEmpty()
         /*|| defaultNameUrl == ui->lEditNameUrl->text() */)
        ui->lEditNameUrl->setText( ui->lEditUrl->text() );

    updateEnabled();
}
void UAddUrlDialog::changeLineEditNameUrl( QString text )
{
    Q_UNUSED( text );

    updateEnabled();
}
void UAddUrlDialog::updateEnabled()
{
    ui->pButtonOk->setEnabled( !ui->lEditNameUrl->text().isEmpty()
                               && !ui->lEditUrl->text().isEmpty() );
}
