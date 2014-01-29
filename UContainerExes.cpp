#include "UContainerExes.h"

#include <QProgressDialog>
#include <QToolButton>

/// PUBLIC
UConteinerExes::UConteinerExes( QWidget * parent )
    : QMainWindow( parent ),
      ui( new Ui::UConteinerExes )
{
    ui->setupUi( this );

    glassDrop.setParent( this );
    glassDrop.remove();
    glassDrop.enableAnimationBlock( new QMovie( ":/IconDrop" ) );
    glassDrop.enableColor( Qt::gray );
    glassDrop.enableOpacity( 0.9 );
    glassDrop.enableInfoTextBlock( QString( "Просто перетащите на окно файлы и папки" ),
                                   QFont( "Times", 14, QFont::Bold ) );

    QToolButton * tButtonAddFile = U::cloneFrom( ui->tButtonAddFile );
    QToolButton * tButtonAddFolder = U::cloneFrom( ui->tButtonAddFolder );
    QToolButton * tButtonAddUrl = U::cloneFrom( ui->tButtonAddUrl );

    tButtonAddFile->setAutoRaise( true );
    tButtonAddFolder->setAutoRaise( true );
    tButtonAddUrl->setAutoRaise( true );

    QSize iconSize = QSize( 40, 40 );
    tButtonAddFile->setIconSize( iconSize );
    tButtonAddFolder->setIconSize( iconSize );
    tButtonAddUrl->setIconSize( iconSize );

    QHBoxLayout * layout = new QHBoxLayout();
    layout->addWidget( tButtonAddFile );
    layout->addWidget( tButtonAddFolder );
    layout->addWidget( tButtonAddUrl );

    // Добавим на стекло виджеты
    glassDrop.enableWidgetBlock( layout );


    setContextMenuPolicy( Qt::NoContextMenu );

    createGUI();    

    ui->tView->setAlternatingRowColors(true);

    ui->lWidgetTags->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->lWidgetTags->setSelectionMode(QAbstractItemView::NoSelection);

    ui->tView->setItemDelegateForColumn( U::Check, new CheckTableItemDelegate() );

    CheckedHeader *header = new CheckedHeader(Qt::Horizontal);
    header->setChecked(checkedItemTableInit == Qt::Checked);
    header->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(header, SIGNAL(toggled(bool)), SLOT(setAllChecked(bool)));

    ui->tView->setHorizontalHeader(header);

    setSizeColumnCheck();

    // Сейчас сделаем небольшую хитрость :)
    // Получим список всех "прикрепляемых" виджетов
    QList < QDockWidget *> allDockWidgets = findChildren < QDockWidget * > ();

    // если список не пуст
    if ( allDockWidgets.size() )
    {
        QMenu * menuView = new QMenu( "Просмотр" );

        // сделаем перебор списка
        foreach ( QDockWidget * dock, allDockWidgets )
        {
            // если dock может быть закрыт (спрятан)
            if ( dock->features().testFlag( QDockWidget::DockWidgetClosable ) )
            {
                QAction * action = menuView->addAction( dock->windowTitle() );
                action->setCheckable( true );
                action->setChecked( dock->isVisible() );

                connect( action, SIGNAL( triggered(bool) ), dock, SLOT( setVisible(bool) ) );
                connect( dock, SIGNAL( visibilityChanged(bool) ), action, SLOT( setChecked(bool) ) );
            }
        }

        menuBar()->addMenu( menuView );
        menuBar()->addMenu( ui->menuCommand );
        menuBar()->addMenu( ui->menuHelp );
    }

    _updateEnabled();

    stateLoad();
    loadSettingsVisibleColumn();
}

UConteinerExes::~UConteinerExes()
{
    delete ui;
}
void UConteinerExes::add(QString path)
{    
    // check-состояние новой строки будет такой же как
    // check у заголовка
    bool checked = static_cast < CheckedHeader * > ( ui->tView->horizontalHeader() )->isChecked();

    QFileInfo fileInfo(path);

    QMap < QString, QVariant > values;
    values["name"]         = fileInfo.isRoot() ? QDir::toNativeSeparators( fileInfo.filePath() ) : fileInfo.completeBaseName();
    values["format"]       = fileInfo.isDir() ? "Папка" : fileInfo.suffix();
    values["path"]         = QDir::toNativeSeparators( path );
    values["created"]      = U::fromDateTime( fileInfo.created() );
    values["lastModified"] = U::fromDateTime( fileInfo.lastModified() );
    values["lastRead"]     = U::fromDateTime( fileInfo.lastRead() );
    values["arguments"]    = "";
    values["comment"]      = "";
    values["tags"]         = values["format"].toString() + tagsSplit;
    values["checked"]      = checked;

    QList < QStandardItem * > list = U::addRowToModel( &model, values, Phase::AddFile );
    UDBManagement::insertToDB( &list );
}
void UConteinerExes::addUrl( QUrl url, QString name )
{
    // check-состояние новой строки будет такой же как
    // check у заголовка
    bool checked = static_cast < CheckedHeader * > ( ui->tView->horizontalHeader() )->isChecked();

    QMap < QString, QVariant > values;
    values["name"]         = name;
    values["format"]       = "URL адрес";
    values["path"]         = url.toString();
    values["created"]      = U::currentDateTime();
    values["lastModified"] = values["created"].toString();
    values["lastRead"]     = "";
    values["arguments"]    = "";
    values["comment"]      = "";
    values["tags"]         = "url" + tagsSplit;
    values["checked"]      = checked;

    QList < QStandardItem * > list = U::addRowToModel( &model, values, Phase::AddUrl );
    UDBManagement::insertToDB( &list );
}

/// PRIVATE
void UConteinerExes::stateSave()
{
    QSettings ini( QString( qApp->applicationDirPath()
                            + "\\" + "settings.ini" ),
                   QSettings::IniFormat );

    ini.setValue( "Geometry", saveGeometry() );
    ini.setValue( "WindowState", saveState() );
}
void UConteinerExes::stateLoad()
{
    QSettings ini( QString( qApp->applicationDirPath()
                            + "\\" + "settings.ini" ),
                        QSettings::IniFormat );
    restoreGeometry( ini.value( "Geometry" ).toByteArray() );
    restoreState( ini.value( "WindowState" ).toByteArray() );
}
void UConteinerExes::saveSettingsVisibleColumn()
{
    if ( ui->tView->isVisible() )
    {
        QBitArray arrayVisibleColumns( U::Size - 1 - 1 ); // Столбцы Check и Name не считать

        for ( uint i = 0; i < U::Size; i++ )
            arrayVisibleColumns.setBit( i, ( !ui->tView->isColumnHidden(i + U::Format) ) );

        QSettings ini( qApp->applicationDirPath()
                       + "\\" + "settings.ini", QSettings::IniFormat );
        ini.setValue( "VisibleColumn", arrayVisibleColumns );
    }
}
void UConteinerExes::loadSettingsVisibleColumn()
{
    QBitArray arrayVisibleColumns;
    QSettings ini( qApp->applicationDirPath()
                   + "\\" + "settings.ini", QSettings::IniFormat );
    arrayVisibleColumns = ini.value( "VisibleColumn" ).toBitArray();

    for ( int i = 0; i < arrayVisibleColumns.size(); i++ )
        ui->tView->setColumnHidden( i + U::Format, !arrayVisibleColumns.at(i) );
}

void UConteinerExes::createGUI()
{
    setHeaderLabels();
    ui->tView->setModel( &model );

    setAcceptDrops(true);

    createToolBars();
    createTrayIcon();

    lEditQuickSearch = new ULineEditWithClearButton();
    lEditQuickSearch->setPlaceholderText( "Быстрый поиск" );
    lEditQuickSearch->setIconButton( QPixmap( ":/IconClear" ) );
    ui->centralWidget->layout()->addWidget( lEditQuickSearch );

    setConnect();
}
void UConteinerExes::createToolBars()
{    
    QToolBar *tBar1 = new QToolBar();
    tBar1->setObjectName( "tBar1" );
    tBar1->addWidget(ui->tButtonStart);
    tBar1->addWidget(ui->tButtonStartAll);    

    QToolBar *tBar2 = new QToolBar();
    tBar2->setObjectName( "tBar2" );
    tBar2->addWidget( ui->tButtonAddFile );
    tBar2->addWidget( ui->tButtonAddFolder );
    tBar2->addWidget( ui->tButtonAddUrl );
    tBar2->addWidget( ui->tButtonRemove );
    tBar2->addWidget( ui->tButtonRemoveAll );

    QToolBar *tBar3 = new QToolBar();
    tBar3->setObjectName( "tBar3" );
    tBar3->addWidget(ui->tButtonDuplicate);
    tBar3->addWidget(ui->tButtonShowInFolder);
    tBar3->addWidget( ui->tButtonCopyFile );
    tBar3->addWidget( ui->tButtonCopyPathToFile );

    QToolBar *tBar4 = new QToolBar();
    tBar4->setObjectName( "tBar4" );    
    tBar4->addWidget(ui->tButtonSettings);
    tBar4->addWidget(ui->tButtonAbout);

    QToolBar *tBar5 = new QToolBar();
    tBar5->setObjectName( "tBar5" );
    tBar5->addWidget(ui->tButtonQuit);

    addToolBar(tBar1);
    addToolBar(tBar2);
    addToolBar(tBar3);
    addToolBar(tBar4);
    addToolBar(tBar5);

    foreach ( QToolBar * toolBar , findChildren < QToolBar * > () )
        toolBar->setMovable( false );
}
void UConteinerExes::createTrayIcon()
{
    systemTrayIcon.setIcon(QIcon(":/IconProgram"));
    systemTrayIcon.show();
}
void UConteinerExes::setHeaderLabels()
{
    QStringList labels;
    for( int i = 0; i < U::Size; i++ )
        labels << "";

    labels[U::Name] = "Имя";
    labels[U::Format] = "Формат";
    labels[U::Path] = "Путь";
    labels[U::Created] = "Дата создания";
    labels[U::LastModified] = "Дата последнего изменения";
    labels[U::LastRead] = "Дата последнего открытия";
    labels[U::Arguments] = "Аргументы";
    labels[U::Comment] = "Комментарий";
    labels[U::Tags] = "Тэги";

    model.setHorizontalHeaderLabels(labels);
}
void UConteinerExes::setConnect()
{
    connect( ui->tButtonAddFile, SIGNAL(clicked()), SLOT(addFile()) );
    connect( ui->tButtonAddFolder, SIGNAL(clicked()), SLOT(addFolder()) );
    connect( ui->tButtonRemove, SIGNAL(clicked()), SLOT(remove()) );
    connect( ui->tButtonRemoveAll, SIGNAL(clicked()), SLOT(removeAll()) );
    connect( ui->tButtonDuplicate, SIGNAL(clicked()), SLOT(duplicate()) );
    connect( ui->tButtonQuit, SIGNAL(clicked()), qApp, SLOT(quit()) );
    connect( ui->tButtonStart, SIGNAL(clicked()), SLOT(start()) );
    connect( ui->tButtonStartAll, SIGNAL(clicked()), SLOT(startAll()) );
    connect( ui->tButtonAbout, SIGNAL(clicked()), SLOT(about()) );
    connect( ui->tButtonShowInFolder, SIGNAL(clicked()), SLOT(showFileInFolder()) );
    connect( ui->tButtonSettings, SIGNAL(clicked()), SLOT(settings()) );
    connect( ui->tButtonCopyPathToFile, SIGNAL( clicked() ), SLOT( copyPathToFileToClipboard() ) );
    connect( ui->tButtonCopyFile, SIGNAL( clicked() ), SLOT( copyFileToClipboard() ) );
    connect( ui->tButtonAddUrl, SIGNAL( clicked() ), SLOT( addUrl() ) );

    connect( ui->actionAddFile, SIGNAL(triggered()), SLOT(addFile()) );
    connect( ui->actionAddFolder, SIGNAL(triggered()), SLOT(addFolder()) );
    connect( ui->actionRemove, SIGNAL(triggered()), SLOT(remove()) );
    connect( ui->actionRemoveAll, SIGNAL(triggered()), SLOT(removeAll()) );
    connect( ui->actionDuplicate, SIGNAL(triggered()), SLOT(duplicate()) );
    connect( ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()) );
    connect( ui->actionStart, SIGNAL(triggered()), SLOT(start()) );
    connect( ui->actionStartAll, SIGNAL(triggered()), SLOT(startAll()) );
    connect( ui->actionAbout, SIGNAL(triggered()), SLOT(about()) );
    connect( ui->actionShowInFolder, SIGNAL(triggered()), SLOT(showFileInFolder()) );
    connect( ui->actionSettings, SIGNAL(triggered()), SLOT(settings()) );
    connect( ui->actionCopyPathToFile, SIGNAL( triggered() ), SLOT( copyPathToFileToClipboard() ) );
    connect( ui->actionCopyFile, SIGNAL( triggered() ), SLOT( copyFileToClipboard() ) );
    connect( ui->actionAddUrl, SIGNAL( triggered() ), SLOT( addUrl() ) );

    connect( ui->actionSearch, SIGNAL( triggered() ),
             lEditQuickSearch, SLOT( setFocus()) );

    connect( ui->actionLoadDB, SIGNAL( triggered() ), SLOT( loadDB() ) );
    connect( ui->actionSaveDBAs, SIGNAL( triggered() ), SLOT( saveDBAs() ) );

    connect( &model, SIGNAL(itemChanged(QStandardItem*)), SLOT(changeItem(QStandardItem*)) );
    connect( ui->tView, SIGNAL(clicked(QModelIndex)), SLOT(clickCell(QModelIndex)) );

    connect(ui->tView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(selectionChange(QItemSelection,QItemSelection)));

    connect(ui->lWidgetTags, SIGNAL(itemChanged(QListWidgetItem*)),
            SLOT(itemListTagsChanged(QListWidgetItem*)));

    connect(lEditQuickSearch, SIGNAL(textChanged(QString)),
            SLOT(search(QString)));

    connect(&systemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(trayActivate(QSystemTrayIcon::ActivationReason)));

//    connect ( ui->tView, SIGNAL( doubleClicked(QModelIndex) ), SLOT( doubleClickCell(QModelIndex) ) );
}

void UConteinerExes::updateListTags()
{
    ui->lWidgetTags->clear();
    tags.clear();

    QList < QListWidgetItem * > itemTags;

    for( int row = 0; row < model.rowCount(); row++ )
    {
        QString strTags = model.item( row, U::Tags )->text();

        if( strTags.isEmpty() || strTags.isNull() )
            continue;

        foreach ( const QString & tag, U::listTags( strTags ) )
        {
            // подсчитаем сколько найдено тэгов
            tags[ tag ]++;

            // если не повтор
            if( ui->lWidgetTags->findItems( tag, Qt::MatchContains ).isEmpty() )
            {
                QListWidgetItem *item = new QListWidgetItem( tag );
                item->setCheckState( Qt::Unchecked );

                ui->lWidgetTags->insertItem( 0, item );
                itemTags << item;
            }
        }
    }   

    // установим очки
    foreach ( QListWidgetItem * item , itemTags )
    {
        item->setData( Qt::UserRole, tags[ item->text() ] );
        item->setToolTip( QString( "Тэг '%1' найдено: %2" )
                          .arg( item->text() )
                          .arg( tags[ item->text() ] ) );
        item->setStatusTip( item->toolTip() );

        ui->lWidgetTags->takeItem(0);
    }

    // отсортируем тэги по популярности
    qSort( itemTags.begin(), itemTags.end(), U::tagsLessThan );

    // поместим в виджет-список
    foreach ( QListWidgetItem * item , itemTags )
        ui->lWidgetTags->insertItem(0, item);
}
void UConteinerExes::setSizeColumnCheck()
{
    ui->tView->horizontalHeader()->resizeSection(U::Check, 20); // размер checkbox'а
    ui->tView->horizontalHeader()->setResizeMode(U::Check, QHeaderView::Fixed);
}

/// PUBLIC SLOTS
void UConteinerExes::load()
{
    QSqlQuery sqlQuery = UDBManagement::sendQuery( UQueryPatterns::select(), false );
    QSqlRecord sqlRecord = sqlQuery.record();

    uint number = UDBManagement::numberOfLines( nameTable );

    QProgressDialog progress( "Заполнение таблицы...",
                              "Отменить и закрыть программу",
                              0, number,
                              this,
                              Qt::Dialog
                              | Qt::WindowCloseButtonHint );

    progress.setWindowTitle( progress.labelText() );
    progress.setWindowModality( Qt::WindowModal );

    for ( uint i = 0; i < number; i++ )
    {
        progress.setValue( i );

        if ( progress.wasCanceled() )
        {
            QTimer::singleShot( 0, qApp, SLOT( quit() ) );
            break;
        }

        sqlQuery.next();

        QMap < QString, QVariant > values;
        values["id"]           = sqlQuery.value( sqlRecord.indexOf( "id" ) );
        values["name"]         = sqlQuery.value( sqlRecord.indexOf( "name" ) );
        values["format"]       = sqlQuery.value( sqlRecord.indexOf( "format" ) );
        values["path"]         = sqlQuery.value( sqlRecord.indexOf( "path" ) );
        values["created"]      = sqlQuery.value( sqlRecord.indexOf( "created" ) );
        values["lastModified"] = sqlQuery.value( sqlRecord.indexOf( "lastModified" ) );
        values["lastRead"]     = sqlQuery.value( sqlRecord.indexOf( "lastRead" ) );
        values["arguments"]    = sqlQuery.value( sqlRecord.indexOf( "arguments" ) );
        values["comment"]      = sqlQuery.value( sqlRecord.indexOf( "comment" ) );
        values["tags"]         = sqlQuery.value( sqlRecord.indexOf( "tags" ) );

        U::addRowToModel( &model, values, Phase::Load );

        statusBar()->showMessage( QString( "Всего строк: %1" ).arg( model.rowCount() ), 1000 );
    }

    progress.setValue( number );

    // если таблица не пуста, текущей (выделенной) строкой,
    // будет первая
    if( ui->tView->model()->rowCount() != 0 )
        ui->tView->selectRow(0);

    updateListTags();
    _updateEnabled();
}
void UConteinerExes::start()
{   
    QList <int> rows;
    QStandardItemModel *model = static_cast <QStandardItemModel *> (ui->tView->model());

    for(int row = 0; row < model->rowCount(); row++)
        if(model->item(row, U::Check)->checkState() == Qt::Checked)
            rows << row;

    // если нет ни одной выделенной галочкой строки
    if ( rows.isEmpty() )
    {
        int row = ui->tView->currentIndex().row();
        QString path = model->item(row, U::Path)->text();

        bool isUrl = model->item( row, U::Name )->data( Qt::UserRole + 2 ).toInt() == Url;
        if ( isUrl )
        {
            QDesktopServices::openUrl( QUrl( path ) );

            QString text = U::currentDateTime();
            int id = model->item( row, U::Name )->data( Qt::UserRole ).toInt();

            model->item( row, U::LastRead )->setText( text );
            UDBManagement::updateCell( "lastRead", text, id );

        }else
        {
            QString arguments = model->item(row, U::Arguments)->text();
            U::runFile(path, arguments);
        }

        statusBar()->showMessage(  "Открыт " + QString( isUrl ? "url-адрес" : "файл" ), 5000 );

        return;
    }

    if(rows.size() > 2)
    {
        QMessageBox msg;
        msg.setWindowTitle("Предупреждение");
        msg.setText(QString("<b>Запустить %1 файла(ов)?</b>").arg(rows.size()));
        msg.setInformativeText("Вы уверены в том, что хотите запустить файлы?");
        msg.setIcon(QMessageBox::Question);
        msg.addButton("Да", QMessageBox::AcceptRole);
        msg.addButton("Отмена", QMessageBox::RejectRole);
        msg.setDefaultButton(QMessageBox::Ok);

        int result = msg.exec();

        if( result == QMessageBox::RejectRole )
            return;
    }    

    foreach(int row, rows)
    {
        QString path = model->item(row, U::Path)->text();
        QString arguments = model->item(row, U::Arguments)->text();

        bool isUrl = model->item( row, U::Name )->data( Qt::UserRole + 2 ).toInt() == Url;
        if ( isUrl )
        {
            QDesktopServices::openUrl( QUrl( path ) );

            QString text = U::currentDateTime();
            int id = model->item( row, U::Name )->data( Qt::UserRole ).toInt();

            model->item( row, U::LastRead )->setText( text );
            UDBManagement::updateCell( "lastRead", text, id );

        }else
            U::runFile(path, arguments);
    }
}
void UConteinerExes::startAll()
{
    QMessageBox msg;
    msg.setWindowTitle("Предупреждение");
    msg.setText("<b>Запустить все файлы?</b>");
    msg.setInformativeText("Вы уверены в том, что хотите запустить все файлы?");
    msg.setIcon(QMessageBox::Question);
    msg.addButton("Да", QMessageBox::AcceptRole);
    msg.addButton("Отмена", QMessageBox::RejectRole);
    msg.setDefaultButton(QMessageBox::Ok);

    int result = msg.exec();

    if(result == QMessageBox::RejectRole)
        return;

    for(int row = 0; row < model.rowCount(); row++)
    {
        QString path = model.item(row, U::Path)->text();

        bool isUrl = model.item( row, U::Name )->data( Qt::UserRole + 2 ).toInt() == Url;
        if ( isUrl )
        {
            QDesktopServices::openUrl( QUrl( path ) );

            QString text = U::currentDateTime();
            int id = model.item( row, U::Name )->data( Qt::UserRole ).toInt();

            model.item( row, U::LastRead )->setText( text );
            UDBManagement::updateCell( "lastRead", text, id );

        }else
        {
            QString arguments = model.item(row, U::Arguments)->text();
            U::runFile(path, arguments);
        }
    }
}
void UConteinerExes::addFile()
{    
    QStringList paths = QFileDialog::getOpenFileNames( this,
                                                       "Выберите файл(ы)",
                                                       QString(),
                                                       "Все файлы (*.*)",
                                                       NULL,
                                                       QFileDialog::DontUseNativeDialog
                                                       | QFileDialog::DontResolveSymlinks);

    if( paths.isEmpty() )
        return;

    uint number = paths.size();

    QProgressDialog progress( "Добавление новых записей в таблицу...",
                              "Прекратить",
                              0, number,
                              this,
                              Qt::Dialog
                              | Qt::WindowCloseButtonHint );

    progress.setWindowTitle( progress.labelText() );
    progress.setWindowModality( Qt::WindowModal );

    for ( uint i = 0; i < number; i++ )
    {
        progress.setValue( i );

        if ( progress.wasCanceled() )
            break;

        add( paths[i] );

        statusBar()->showMessage( QString( "Добавлено записей: %1" )
                                  .arg( i + 1 ), 5000 );
    }

    progress.setValue( number );

    // Выделим первую строку
    ui->tView->selectRow(0);

    updateListTags();
    _updateEnabled();
}
void UConteinerExes::addFolder()
{
    QString path = QFileDialog::getExistingDirectory();

    if ( !U::isValid( path ) )
        return;

    add( path );

    statusBar()->showMessage( "Добавлена новая запись", 5000 );

    // Выделим первую строку
    ui->tView->selectRow(0);

    updateListTags();
    _updateEnabled();
}
void UConteinerExes::addUrl()
{    
    UAddUrlDialog * dialog = new UAddUrlDialog();

    if( !dialog->exec() )
        return;

    addUrl( QUrl( dialog->url() ), dialog->name() );

    statusBar()->showMessage( "Добавлена новая запись", 5000 );

    // Выделим первую строку
    ui->tView->selectRow(0);

    updateListTags();
    _updateEnabled();
}
void UConteinerExes::remove()
{
    QList <int> rows;

    for ( int row = 0; row < model.rowCount(); row++ )
        if( model.item( row, U::Check )->checkState() == Qt::Checked )
            rows << row;

    // если нет ни одной выделенной галочкой строки
    if ( rows.isEmpty() )
    {
        int row = ui->tView->currentIndex().row();
        remove( row );

        statusBar()->showMessage( "Удалена запись", 5000 );

        updateListTags();
        _updateEnabled();

        return;
    }

    if(rows.size() > 2)
    {
        QMessageBox msg;
        msg.setWindowTitle("Предупреждение");
        msg.setText(QString("<b>Удаление из таблицы %1 записей(и)?</b>").arg(rows.size()));
        msg.setInformativeText("Вы уверены в том, что хотите удалить записи?");
        msg.setIcon(QMessageBox::Question);
        msg.addButton("Да", QMessageBox::AcceptRole);
        msg.addButton("Отмена", QMessageBox::RejectRole);
        msg.setDefaultButton(QMessageBox::Ok);

        int result = msg.exec();

        if(result == QMessageBox::RejectRole)
            return;
    }


    int number = rows.size();

    QProgressDialog progress( "Удаление данных...",
                              "Прервать",
                              0, number,
                              this,
                              Qt::Dialog
                              | Qt::WindowCloseButtonHint );

    progress.setWindowTitle( progress.labelText() );
    progress.setWindowModality( Qt::WindowModal );

    for ( int i = 0; i < number; i++ )
    {
        progress.setValue( i );

        if ( progress.wasCanceled() )
            break;

        // удаляем строку
        remove( rows.takeLast() );

        statusBar()->showMessage( QString( "Удалено %1 из %2" )
                                  .arg( i + 1 ).arg( number ), 5000 );
    }

    progress.setValue( number );


    updateListTags();
    _updateEnabled();

    // после удаления уберем флаг с заголовка
    ( static_cast < CheckedHeader * > ( ui->tView->horizontalHeader() ) )->setChecked( false );
}
void UConteinerExes::remove(int row)
{
    QStandardItemModel *model = static_cast <QStandardItemModel *> (ui->tView->model());
    int id = model->item(row, U::Name)->data(Qt::UserRole).toInt();

    UDBManagement::deleteRow( id, false );
    model->removeRow( row );
}
void UConteinerExes::removeAll()
{
    QMessageBox msg;
    msg.setWindowTitle("Предупреждение");
    msg.setText("<b>Таблица будет очищена.</b>");
    msg.setInformativeText("Вы уверены в том, что хотите удалить все записи из таблицы?");
    msg.setIcon(QMessageBox::Question);
    msg.addButton("Да", QMessageBox::AcceptRole);
    msg.addButton("Отмена", QMessageBox::RejectRole);
    msg.setDefaultButton(QMessageBox::Ok);

    int result = msg.exec();

    if ( result == QMessageBox::RejectRole )
        return;

    // запрещаем вьюхе обновляться
    ui->tView->setUpdatesEnabled( false );

    uint number = model.rowCount();

    QProgressDialog progress( "Удаление записей из таблицы...",
                              "Прервать",
                              0, number,
                              this,
                              Qt::Dialog
                              | Qt::WindowCloseButtonHint);

    progress.setWindowTitle( progress.labelText() );
    progress.setWindowModality( Qt::WindowModal );

    for ( uint i = 0; i < number; i++ )
    {
        progress.setValue(i);

        if ( progress.wasCanceled() )
            break;

        remove( 0 );

        statusBar()->showMessage( QString( "Удалено %1 из %2" )
                                  .arg( i + 1 ).arg( number ), 5000 );
    }

    progress.setValue( number );

    ui->tView->setUpdatesEnabled( true );

    updateListTags();
    _updateEnabled();

    // после удаления уберем флаг с заголовка
    ( static_cast < CheckedHeader * > ( ui->tView->horizontalHeader() ) )->setChecked( false );
}
void UConteinerExes::duplicate()
{
    int row = ui->tView->currentIndex().row();    

    QList < QStandardItem * > list;
    for ( int column = 0; column < U::Size; column++ )
        list << model.item( row, column )->clone();

    model.insertRow( model.rowCount(), list );
    UDBManagement::insertToDB( &list );

    // выделим дупликат (последнюю строку)
    ui->tView->selectRow( model.rowCount() - 1 );

    updateListTags();
    _updateEnabled();
}
void UConteinerExes::about()
{
    UAboutProgram *about = new UAboutProgram();
    about->exec();
    about->deleteLater();
}
void UConteinerExes::search(QString text)
{   
    ui->tView->setUpdatesEnabled(false);

    bool dontSearch = !U::isValid(text);
    bool selectedTags = !ui->lWidgetTags->selectedItems().isEmpty();

    QStandardItemModel *model = static_cast <QStandardItemModel *> (ui->tView->model());

    if(!selectedTags)
    {
        for(int row = 0; row < model->rowCount(); row++)
            if(dontSearch)
                ui->tView->setRowHidden(row, false); // покажем все
            else
                ui->tView->setRowHidden(row, true); // спрячем все

    }else
    {
        if(!dontSearch)
            for(int row = 0; row < model->rowCount(); row++)
                ui->tView->setRowHidden(row, true); // спрячем все
        else
            for(int row = 0; row < model->rowCount(); row++)
            {
                // покажем все c флагом "тэг"
                if(model->item(row, U::Name)->data(Qt::UserRole + 1) == isTag)
                    ui->tView->setRowHidden(row, false);
            }
    }

    ui->tView->setUpdatesEnabled(true);

    if(dontSearch)
        return;

    ui->tView->setUpdatesEnabled(false);

    if(!selectedTags)
    {
        for(int row = 0; row < model->rowCount(); row++)
            ui->tView->setRowHidden(row, true);

        QList <int> rows;
        for(int column = U::Name; column < U::Size; column++)
            foreach(QStandardItem *item, model->findItems(text, Qt::MatchContains, column))
                rows << item->row();

        U::removeDuplicate(&rows);

        foreach(int row, rows)
            ui->tView->setRowHidden(row, false);

    }else
    {
        for(int row = 0; row < model->rowCount(); row++)
            ui->tView->setRowHidden(row, true);

        QList <int> rows;

        // сделаем эмиляцию изменения выделения тэгов
        // после нее останутся видимыми только те строки
        // тэги которых совпадают с выделенными
        changeSelectionTags();

        for(int row = 0; row < model->rowCount(); row++)
        {
            // если строка не скрыта
            if(!ui->tView->isRowHidden(row))
            {
                for(int column = U::Name; column < U::Size; column++)
                {
                    QStandardItem *item = model->item(row, column);
                    if(item->data(Qt::UserRole + 1).toInt() == isTag
                            && item->text().contains(text, Qt::CaseInsensitive))
                        rows << row;
                }
            }

            // после проверки, скроем строку
            ui->tView->setRowHidden(row, true);
        }

        U::removeDuplicate(&rows);

        foreach(int row, rows)
            ui->tView->setRowHidden(row, false);
    }

    ui->tView->setUpdatesEnabled(true);
}
void UConteinerExes::search()
{
    search( lEditQuickSearch->text() );
}
void UConteinerExes::setAllChecked( bool checked )
{    
    for ( int row = 0; row < model.rowCount(); row++ )
    {
        if ( !ui->tView->isRowHidden( row ) )
        {
            QStandardItem * item = model.item(row, U::Check);
            item->setCheckState( checked ? Qt::Checked : Qt::Unchecked );
        }
    }

    _updateEnabled();
}
void UConteinerExes::showFileInFolder()
{
    int row = ui->tView->currentIndex().row();
    QString path = model.item( row, U::Path )->text();
    QProcess::startDetached( QString( "explorer.exe /select,%1" )
                             .arg( QDir::toNativeSeparators( path ) ) );
}
void UConteinerExes::settings()
{
    USettings * settings = new USettings( this );
    settings->pageVisibleColumns.setTableView( ui->tView );

    settings->exec();
    settings->deleteLater();

    saveSettingsVisibleColumn();
}
void UConteinerExes::copyPathToFileToClipboard()
{
    int row = ui->tView->currentIndex().row();
    QString path = model.item( row, U::Path )->text();

    QClipboard * clipboard = qApp->clipboard();
    clipboard->setText( path );

    statusBar()->showMessage( "Путь к файлу/папке скопирован в буфер обмена...", 4000 );
}
void UConteinerExes::copyFileToClipboard()
{
    int row = ui->tView->currentIndex().row();
    QString path = model.item( row, U::Path )->text();

    QList < QUrl > files;
    files << QUrl::fromLocalFile( path );

    U::copyFilesToClipboard( files );

    statusBar()->showMessage( "Файл/папка скопирован в буфер обмена...", 4000 );
}
void UConteinerExes::saveDBAs()
{
    QString path = QFileDialog::getSaveFileName( this,
                                                 "Сохранить как...",
                                                 QString()/*QDesktopServices::storageLocation( QDesktopServices::HomeLocation )*/,
                                                 "Table ( *.list )" );

    if ( path.isEmpty() )
        return;

    QString pathCurrent = qApp->applicationDirPath()
            + QDir::separator()
            + nameDBFile;

    bool successful = QFile::copy( pathCurrent, path );

    if ( !successful )
        QMessageBox::information( this, "Информация", "При сохранении произошла ошибка" );
}
void UConteinerExes::loadDB()
{
    QString path = QFileDialog::getOpenFileName( this,
                                                 "Открыть...",
                                                 QString()/*QDesktopServices::storageLocation( QDesktopServices::HomeLocation )*/,
                                                 "Table ( *.list )" );

    if ( path.isEmpty() )
        return;

    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE", "Loading" );
    db.setDatabaseName( path );

    if( !db.open() )
    {
        qDebug() << "Не удалось подключиться к БД:"
                 << db.lastError().text();
        return;

    }else
        qDebug() << "Установлено подключение к базе данных" << path;

    // Получим таблицу в добавляемое бд
    QSqlQuery sqlQuery = UDBManagement::sendQuery( UQueryPatterns::select(),
                                                   db,
                                                   false );

    QSqlRecord sqlRecord = sqlQuery.record();

    uint number = UDBManagement::numberOfLines( nameTable, db );

    ui->tView->setUpdatesEnabled( false );

    QProgressDialog progress( "Добавление в таблицу записей...",
                              "Прервать",
                              0, number,
                              this,
                              Qt::Dialog
                              | Qt::WindowCloseButtonHint );

    progress.setWindowTitle( progress.labelText() );
    progress.setWindowModality( Qt::WindowModal );

    // переберем строки
    for ( uint i = 0; i < number; i++ )
    {
        progress.setValue( i );

        if ( progress.wasCanceled() )
            break;

        sqlQuery.next();

        QString name = sqlQuery.value(sqlRecord.indexOf("name")).toString();
        QString format = sqlQuery.value(sqlRecord.indexOf("format")).toString();
        QString path = sqlQuery.value(sqlRecord.indexOf("path")).toString();
        QString created = sqlQuery.value( sqlRecord.indexOf( "created" ) ).toString();
        QString lastModified = sqlQuery.value( sqlRecord.indexOf( "lastModified" ) ).toString();
        QString lastRead = sqlQuery.value( sqlRecord.indexOf( "lastRead" ) ).toString();
        QString arguments = sqlQuery.value(sqlRecord.indexOf("arguments")).toString();
        QString comment = sqlQuery.value(sqlRecord.indexOf("comment")).toString();
        QString tags = sqlQuery.value( sqlRecord.indexOf( "tags" ) ).toString();

        QStringList columns;
        for ( int i = 0; i < U::Size; i++ )
            columns << "";

        columns[ U::Name ] = name;
        columns[ U::Format ] = format;
        columns[ U::Path ] = path;
        columns[ U::Created ] = created;
        columns[ U::LastModified ] = lastModified;
        columns[ U::LastRead ] = lastRead;
        columns[ U::Arguments ] = arguments;
        columns[ U::Comment ] = comment;
        columns[ U::Tags ] = tags;

        // Добавим строки из новой в таблицы в старую
        UDBManagement::insertToDB( columns );
    }

    progress.setValue( number );

    // перезальем данные
    model.clear();
    setHeaderLabels();
    setSizeColumnCheck();
    load();
}

/// PRIVATE
void UConteinerExes::_updateEnabled()
{
    bool hasSelection = ui->tView->selectionModel()->hasSelection();
    bool hasChecked = false;    

    int rowCount = model.rowCount();

    for( int row = 0; row < rowCount; row++ )
        if ( model.item( row, U::Check )->checkState() == Qt::Checked )
        {
            hasChecked = true;
            break;
        }

    bool hasCheckedOrSelection = hasChecked || hasSelection;

    bool notEmpty = rowCount != 0;
    bool isEmpty = !notEmpty;

    // Если таблица пуста и защиты нет
    if ( isEmpty )
        glassDrop.install( ui->tView ); // Установим стекло
    else
        glassDrop.remove(); // Удалим стекло

    bool isFile = false;
    bool isUrl = false;

    if ( ui->tView->currentIndex().isValid() )
    {
        int row = ui->tView->currentIndex().row();

        QFileInfo info = QFileInfo( model.item( row, U::Path )->text() );
        isFile = info.isFile() || info.isDir();
        isUrl = model.item( row, U::Name )->data( Qt::UserRole + 2 ).toInt() == Url;
    }

    ui->tButtonRemove->setEnabled( notEmpty && hasCheckedOrSelection );
    ui->tButtonRemoveAll->setEnabled( notEmpty );
    ui->tButtonStart->setEnabled( notEmpty && hasCheckedOrSelection );
    ui->tButtonStartAll->setEnabled( notEmpty );
    ui->tButtonDuplicate->setEnabled( hasSelection );
    ui->tButtonShowInFolder->setEnabled( hasSelection && isFile );
    ui->tButtonCopyPathToFile->setEnabled( isFile || isUrl );
    ui->tButtonCopyFile->setEnabled( isFile );

    ui->actionRemove->setEnabled( ui->tButtonRemove->isEnabled() );
    ui->actionRemoveAll->setEnabled( ui->tButtonRemoveAll->isEnabled() );
    ui->actionStart->setEnabled( ui->tButtonStart->isEnabled() );
    ui->actionStartAll->setEnabled( ui->tButtonStartAll->isEnabled() );
    ui->actionDuplicate->setEnabled( ui->tButtonDuplicate->isEnabled() );
    ui->actionShowInFolder->setEnabled( ui->tButtonShowInFolder->isEnabled() );
    ui->actionCopyPathToFile->setEnabled( ui->tButtonCopyPathToFile->isEnabled() );
    ui->actionCopyFile->setEnabled( ui->tButtonCopyFile->isEnabled() );

    lEditQuickSearch->setEnabled( notEmpty );
}
void UConteinerExes::clickCell(QModelIndex index)
{
    if(index.column() == U::Check)
        _updateEnabled();
}
void UConteinerExes::itemListTagsChanged(QListWidgetItem *item)
{
    Q_UNUSED(item);
    changeSelectionTags();
}
void UConteinerExes::changeItem( QStandardItem *item )
{
    int id = model.item(item->row(), U::Name)->data(Qt::UserRole).toInt();

    QString value = item->text();

    if( item->column() == U::Name
            || item->column() == U::Path )
    {
        if ( item->data( Qt::UserRole + 2 ).toInt() != Url )
            return;

        QString lastModifiedText = U::currentDateTime();
        QString name = model.item( item->row(), U::Name )->text();
        QString path = model.item( item->row(), U::Path )->text();

        model.item( item->row(), U::LastModified )->setText( lastModifiedText );

        UDBManagement::updateCell("lastModified", lastModifiedText, id);
        UDBManagement::updateCell("name", name, id);
        UDBManagement::updateCell("path", path, id);

    }else if(item->column() == U::Arguments)
        UDBManagement::updateCell("arguments", value, id);

    else if(item->column() == U::Comment)
        UDBManagement::updateCell("comment", value, id);

    else if(item->column() == U::Tags)
    {
        UDBManagement::updateCell("tags", value, id);
        updateListTags();
    }
}

void UConteinerExes::trayActivate(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick)
        setVisible(!isVisible());    
}
void UConteinerExes::selectionChange(QItemSelection item1, QItemSelection item2)
{
    Q_UNUSED(item1)
    Q_UNUSED(item2)

    _updateEnabled();
}
void UConteinerExes::changeSelectionTags()
{
    QList < QListWidgetItem * > list;
    for ( int i = 0; i < ui->lWidgetTags->count(); i++ )
    {
        QListWidgetItem *item = ui->lWidgetTags->item(i);
        if ( item->checkState() == Qt::Checked )
            list << item;
    }

    // если ни один тег не выделен
    if ( list.isEmpty() )
    {
        ui->tView->setUpdatesEnabled( false );

        for(int row = 0; row < model.rowCount(); row++)
        {
            QStandardItem *item = model.item(row, U::Name);
            item->setData(noTag, Qt::UserRole + 1);
            ui->tView->setRowHidden(row, false);
        }

        ui->tView->setUpdatesEnabled(true);

        return;
    }

    QList < int > rows;

    foreach( QListWidgetItem * item, list )
        for( int row = 0; row < model.rowCount(); row++ )
            foreach( const QString & tag, U::listTags( model.item( row, U::Tags )->text() ) )
                if( tag.contains( QRegExp( QString( "\\b%1\\b" ).arg( item->text() ) ) ) )
                {
                    rows << row;
                    break;
                }

    U::removeDuplicate( &rows );

    ui->tView->setUpdatesEnabled(false);

    // скрываем все и ставим флаг "Не тэг"
    for ( int row = 0; row < model.rowCount(); row++ )
    {
        QStandardItem * item = model.item( row, U::Name );
        item->setData( noTag, Qt::UserRole + 1 );
        ui->tView->setRowHidden( row, true );
    }

    // проходим по всем строкам, с найденными тэгами
    foreach ( const int row, rows )
    {
        model.item( row, U::Name )->setData( isTag, Qt::UserRole + 1 );
        ui->tView->setRowHidden( row, false );
    }

    ui->tView->setUpdatesEnabled( true );
}

/// PROTECTED
void UConteinerExes::dragEnterEvent( QDragEnterEvent * event )
{
    const QList < QUrl > urls = event->mimeData()->urls();

    statusBar()->showMessage( QString( "Выбрано %1 элементов(а)" )
                              .arg( urls.size() ), 3000 );

    foreach ( const QUrl & url, urls )
        if ( url.isLocalFile() )
        {
            event->acceptProposedAction();
            break;
        }
}
void UConteinerExes::dropEvent( QDropEvent * event )
{
    const QList < QUrl > urls = event->mimeData()->urls();
    uint number = urls.size();

    QProgressDialog progress( "Добавление файлов...",
                              "Прервать",
                              0, number,
                              this,
                              Qt::Dialog
                              | Qt::WindowCloseButtonHint );

    progress.setWindowTitle( progress.labelText() );
    progress.setWindowModality( Qt::WindowModal );

    for ( uint i = 0; i < number; i++ )
    {
        progress.setValue(i);

        if ( progress.wasCanceled() )
            break;

        add( urls[i].toLocalFile() );

        statusBar()->showMessage( QString( "Добавлено %1 из %2" )
                                  .arg( i + 1 ).arg( number ), 5000 );
    }

    progress.setValue( number );

    // Выделим первую строку
    ui->tView->selectRow(0);

    updateListTags();
    _updateEnabled();
}
void UConteinerExes::closeEvent( QCloseEvent * event )
{
//    event->ignore();
//    hide();

    stateSave();
    saveSettingsVisibleColumn();

    QMainWindow::closeEvent(event);
}
