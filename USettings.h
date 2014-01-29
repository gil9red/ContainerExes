#ifndef USETTINGS_H
#define USETTINGS_H

#include <QDialog>
#include <QCheckBox>
#include <QHeaderView>
#include <QListWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QSettings>

namespace Ui {
    class USettings;
}

#include "USupport.h"
#include "USettingsPassword.h"

class USettingsVisibleColumns: public QWidget
{
    Q_OBJECT

public:
    USettingsVisibleColumns(QWidget *parent = 0)
        : QWidget(parent),
          p_header(0)
    {
        visibleColumns.setText( "Показывать столбцы:" );

        QVBoxLayout *vBoxLayout = new QVBoxLayout();
        vBoxLayout->setSpacing( 0 );
        vBoxLayout->addWidget( &visibleColumns, 0, Qt::AlignLeft );
        vBoxLayout->addWidget( &lwColumns );
        setLayout( vBoxLayout );

        connect( &lwColumns, SIGNAL(itemChanged(QListWidgetItem*)),
                 SLOT(changeItem(QListWidgetItem*)) );

        connect( &visibleColumns, SIGNAL( clicked(bool) ),
                 SLOT( setVisibleColumns(bool) ) );

        _updateEnabled();
    }

    void setTableView(QTableView *view)
    {
        p_header = 0;
        lwColumns.clear();

        QAbstractItemModel *model = view->model();
        QHeaderView *header = view->horizontalHeader();

        p_header = header;

        for(int column = U::Name + 1; column < U::Size; column++)
        {
            bool checked = !header->isSectionHidden(column);
            QString text = model->headerData(column, Qt::Horizontal).toString();

            QListWidgetItem *item = new QListWidgetItem(text);
            item->setData( Qt::UserRole, column );
            item->setCheckState( checked ? Qt::Checked : Qt::Unchecked );
            lwColumns.addItem(item);
        }

        _updateEnabled();
    }

private:
    void _updateEnabled()
    {
        bool isChecked = false;
        bool isUnchecked = false;

        Qt::CheckState checkState;

        for ( int i = 0; i < lwColumns.count(); i++ )
        {
            checkState = lwColumns.item( i )->checkState();

            if ( checkState == Qt::Checked )
                isChecked = true;

            else if ( checkState == Qt::Unchecked )
                isUnchecked = true;

            if ( isChecked && isUnchecked )
                break;
        }

        // Если есть и выделенные и нет, тогда смешанный check
        if ( isChecked && isUnchecked )
            checkState = Qt::PartiallyChecked;

        else if ( isChecked && !isUnchecked )
            checkState = Qt::Checked;

        else if ( !isChecked && isUnchecked )
            checkState = Qt::Unchecked;

        visibleColumns.setCheckState( checkState );
    }

private:
    QCheckBox visibleColumns;
    QListWidget lwColumns;
    QHeaderView *p_header;

private slots:
    void changeItem( QListWidgetItem * item )
    {
        if( !p_header )
            return;

        int index = item->data( Qt::UserRole ).toInt();
        bool hidden = item->checkState() == Qt::Unchecked;

        p_header->setSectionHidden( index, hidden );

        _updateEnabled();
    }
    void setVisibleColumns( bool visible )
    {
        for ( int i = 0; i < lwColumns.count(); i++ )
            lwColumns.item( i )->setCheckState( visible ? Qt::Checked : Qt::Unchecked );
    }
};


class USettings: public QDialog
{
    Q_OBJECT
    
public:
    explicit USettings(QWidget *parent = 0);
    ~USettings();
    
private:
    Ui::USettings *ui;

public:
    USettingsVisibleColumns pageVisibleColumns;
    USettingsPassword pagePassword;
};

#endif // USETTINGS_H
