#ifndef UCONTEINEREXES_H
#define UCONTEINEREXES_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QSystemTrayIcon>
#include <QToolBar>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QListWidgetItem>
#include <QLayout>
#include <QIcon>
#include <QMenu>
#include <QPushButton>
#include <QClipboard>
#include <QString>
#include <QDebug>
#include <QInputDialog>
#include <QLabel>
#include <QMap>
#include <QTimer>
#include <QtSql>
#include <QSettings>

#include "ui_UContainerExes.h"

namespace Ui
{
    class UConteinerExes;
}

#include "UDatabaseManagement.h"
#include "USupport.h"
#include "UAboutProgram.h"
#include "UAddUrlDialog.h"
#include "sbglass.h"
#include "UPasswordWidget.h"
#include "USettings.h"

class UConteinerExes: public QMainWindow
{
    Q_OBJECT

public:
    explicit UConteinerExes(QWidget *parent = 0);
    ~UConteinerExes();

    void add(QString path);
    void addUrl( QUrl url, QString name );

private:
    void stateSave();
    void stateLoad();
    void saveSettingsVisibleColumn();
    void loadSettingsVisibleColumn();

    void createGUI();
    void createToolBars();
    void createTrayIcon();    
    void setHeaderLabels();
    void setConnect();    
    void updateListTags();
    void setSizeColumnCheck();

private:
    Ui::UConteinerExes *ui;
    QStandardItemModel model;
    QSystemTrayIcon systemTrayIcon;
    ULineEditWithClearButton * lEditQuickSearch;
    QMap < QString, uint > tags;
    SBGlass glassDrop;

public slots:
    void load();
    void start();
    void startAll();
    void addFile();
    void addFolder();
    void addUrl();
    void remove();
    void remove( int row );
    void removeAll();
    void duplicate();
    void about();
    void search( QString text);
    void search();
    void setAllChecked( bool checked );
    void showFileInFolder();
    void settings();
    void copyPathToFileToClipboard();
    void copyFileToClipboard();
    void saveDBAs();
    void loadDB();

private slots:
    void _updateEnabled();
    void clickCell( QModelIndex index );
    void itemListTagsChanged( QListWidgetItem * item );
    void changeItem( QStandardItem * item );
    void trayActivate( QSystemTrayIcon::ActivationReason reason );
    void selectionChange( QItemSelection item1, QItemSelection item2 );
    void changeSelectionTags();

protected:
    void dragEnterEvent( QDragEnterEvent * event );
    void dropEvent( QDropEvent * event );
    void closeEvent( QCloseEvent * event );
};

#endif // UCONTEINEREXES_H
