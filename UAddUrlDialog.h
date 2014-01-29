#ifndef UADDURLDIALOG_H
#define UADDURLDIALOG_H

#include <QDialog>

namespace Ui {
    class UAddUrlDialog;
}

class UAddUrlDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit UAddUrlDialog( QWidget *parent = 0 );
    ~UAddUrlDialog();
    
    QString url();
    QString name();

private:
    Ui::UAddUrlDialog *ui;
    QString defaultNameUrl;
    QString d_url;
    QString d_name;

public slots:
    void ok();
    void cancel();

private slots:
    void changeLineEditUrl( QString text );
    void changeLineEditNameUrl( QString text );
    void updateEnabled();
};

#endif // UADDURLDIALOG_H
