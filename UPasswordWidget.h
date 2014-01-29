#ifndef UPASSWORDWIDGET_H
#define UPASSWORDWIDGET_H

#include <QDialog>

namespace Ui
{
    class UPasswordWidget;
}

class UPasswordWidget: public QDialog
{
    Q_OBJECT
    
public:
    explicit UPasswordWidget( QWidget *parent = 0 );
    ~UPasswordWidget();
    
    void setVisibleCancelButton( const bool visible );
    void setVisiblePageSecretQuestion( const bool visible );

    void installConnect();

private:
    Ui::UPasswordWidget *ui;
    QString d_correctPassword;
    QString d_answer;

    QString d_error;

public slots:
    void setCorrectPassword( const QString & text );
    QString correctPassword();

    void setDisplayPassword( const bool display );

    void setSecretQuestion( const QString & text );
    void setAnswer( const QString & text );

    void setError( const QString & text );

    void ok();
    void cancel();

private slots:
    void textLineEditChanged( const QString & text );
    void _updateEnabled();
    void doSwitch();

signals:
    void correct( bool );
    void successfully();
    void fail();

protected:
    void closeEvent( QCloseEvent * );
};

#endif // UPASSWORDWIDGET_H
