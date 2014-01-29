#ifndef USETTINGSPASSWORD_H
#define USETTINGSPASSWORD_H

#include <QWidget>

namespace Ui {
    class USettingsPassword;
}

class USettingsPassword : public QWidget
{
    Q_OBJECT
    
public:
    explicit USettingsPassword(QWidget *parent = 0);
    ~USettingsPassword();
    
private:
    Ui::USettingsPassword *ui;
};

#endif // USETTINGSPASSWORD_H
