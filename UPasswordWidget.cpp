#include "UPasswordWidget.h"
#include "ui_UPasswordWidget.h"

#include <QMessageBox>

/// PUBLIC
UPasswordWidget::UPasswordWidget( QWidget * parent ) :
    QDialog( parent ),
    ui( new Ui::UPasswordWidget )
{
    ui->setupUi( this );

    d_error = "Error! Incorrect password!";

    installConnect();

    _updateEnabled();
}

UPasswordWidget::~UPasswordWidget()
{
    delete ui;
}

void UPasswordWidget::setVisibleCancelButton( const bool visible )
{
    ui->pButtonCancel->setVisible( visible );
}
void UPasswordWidget::setVisiblePageSecretQuestion( const bool visible )
{
    // если нужно скрыть страницу секретного вопроса,
    // и она является в данный момент текущей
    if ( !visible
         && ui->stackedWidget->currentWidget() == ui->pageSecretQuestion )
        ui->stackedWidget->setCurrentWidget( ui->pagePassword );

    ui->tButtonSwither->setVisible( visible );
}
void UPasswordWidget::installConnect()
{
    connect( ui->pButtonAccept, SIGNAL( clicked() ), SLOT( ok() ) );
    connect( ui->pButtonCancel, SIGNAL( clicked() ), SLOT( cancel() ) );

    connect( ui->cBoxDisplayPassword, SIGNAL( toggled(bool) ),
             SLOT( setDisplayPassword(bool) ) );

    connect( ui->lEditPassword, SIGNAL( textChanged(QString) ),
             SLOT( textLineEditChanged(QString) ) );
    connect( ui->lEditAnswer, SIGNAL( textChanged(QString) ),
             SLOT( textLineEditChanged(QString) ) );

    connect( ui->tButtonSwither, SIGNAL( clicked() ), SLOT( doSwitch() ) );
}

/// PUBLIC SLOTS
void UPasswordWidget::setCorrectPassword( const QString & text )
{ d_correctPassword = text; }

QString UPasswordWidget::correctPassword()
{ return d_correctPassword; }

void UPasswordWidget::setDisplayPassword( const bool display )
{
    ui->lEditPassword->setEchoMode( display ? QLineEdit::Normal :
                                              QLineEdit::Password );
}
void UPasswordWidget::setSecretQuestion( const QString & text )
{
    ui->lEditSecretQuestion->setText( text );
}
void UPasswordWidget::setAnswer( const QString & text )
{
    d_answer = text;
    _updateEnabled();
}

void UPasswordWidget::setError( const QString & text )
{
    d_error = text;
}

void UPasswordWidget::ok()
{
    bool successful = false;

    const QWidget * currentPage = ui->stackedWidget->currentWidget();

    if ( currentPage == ui->pagePassword )
        successful = d_correctPassword == ui->lEditPassword->text();

    else if ( currentPage == ui->pageSecretQuestion )
        successful = d_answer == ui->lEditAnswer->text();

    emit correct( successful );

    if ( successful )
    {
//        accept();
        setResult( QDialog::Accepted );
        hide();

        emit successfully();

    }else
    {
        QMessageBox::information( this,
                                  "Информация",
                                  d_error );

        emit fail();
    }
}
void UPasswordWidget::cancel()
{
    emit correct( false );
    emit fail();

    setResult( QDialog::Rejected );
    hide();
//    reject();
}

/// PRIVATE SLOTS
void UPasswordWidget::textLineEditChanged( const QString & text )
{
    Q_UNUSED ( text );

    _updateEnabled();
}
void UPasswordWidget::_updateEnabled()
{
    bool isEmpty = true;

    const QWidget * currentPage = ui->stackedWidget->currentWidget();

    if ( currentPage == ui->pagePassword )
    {
        isEmpty = ui->lEditPassword->text().isEmpty();

        ui->tButtonSwither->setText( "->" );
        ui->tButtonSwither->setToolTip( "Переключиться на ввод ответа на секретный вопрос" );
        ui->tButtonSwither->setWhatsThis( ui->tButtonSwither->toolTip() );

    }else if ( currentPage == ui->pageSecretQuestion )
    {
        isEmpty = ui->lEditAnswer->text().isEmpty();

        ui->tButtonSwither->setText( "<-" );
        ui->tButtonSwither->setToolTip( "Переключиться на ввод пароля" );
        ui->tButtonSwither->setWhatsThis( ui->tButtonSwither->toolTip() );
    }

    ui->pButtonAccept->setEnabled( !isEmpty );

    ui->tButtonSwither->setVisible( !d_answer.isEmpty() );
}
void UPasswordWidget::doSwitch()
{
    const QWidget * currentPage = ui->stackedWidget->currentWidget();

    if ( currentPage == ui->pagePassword )
        ui->stackedWidget->setCurrentWidget( ui->pageSecretQuestion );

    else if ( currentPage == ui->pageSecretQuestion )
        ui->stackedWidget->setCurrentWidget( ui->pagePassword );

    _updateEnabled();
}

/// PROTECTED
void UPasswordWidget::closeEvent( QCloseEvent * )
{
    cancel();
}
