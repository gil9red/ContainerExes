#ifndef USUPPORT_H
#define USUPPORT_H

#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QTextCodec>
#include <QDesktopServices>
#include <QUrl>
#include <QDateTime>
#include <QDesktopWidget>
#include <QClipboard>
#include <QApplication>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>
#include <QInputDialog>
#include <QHeaderView>
#include <QDebug>
#include <QTableView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QStandardItemModel>
#include <QFileIconProvider>

#include <windows.h>

#include "ULineEditWithButton/ULineEditWithClearButton.h"

const char codec[] = "utf8";
const QString nameDBFile = "Files.list";
const QString nameTable = "Files";
const QString tagsSplit = ";";

const Qt::CheckState checkedItemTableInit = Qt::Unchecked;

namespace Phase
{
    enum
    {
        Load,
        AddFile,
        AddUrl
    };
}

enum { isTag, noTag };
enum { File, Url };

class U
{
public:
    enum UColumns
    {
        Check, Name, Format, Path,
        Created, LastModified, LastRead,
        Arguments, Comment, Tags,

        Size
    };

    static QList < QStandardItem * > addRowToModel( QStandardItemModel * model, const QMap < QString, QVariant > & values, int phase )
    {
        int id = values["id"].toInt();
        QString name = values["name"].toString();
        QString format = values["format"].toString();
        QString path = values["path"].toString();
        QString created = values["created"].toString();
        QString lastModified = values["lastModified"].toString();
        QString lastRead = values["lastRead"].toString();
        QString arguments = values["arguments"].toString();
        QString comment = values["comment"].toString();
        QString tags = values["tags"].toString();
        bool checked = values["checked"].toBool();

        QList < QStandardItem * > list;
        for( int i = 0; i < U::Size; i++ )
            list << NULL;

        QStandardItem *itemCheck = new QStandardItem();
        QStandardItem *itemName = new QStandardItem();
        QStandardItem *itemFormat = new QStandardItem();
        QStandardItem *itemPath = new QStandardItem();
        QStandardItem *itemCreated = new QStandardItem();
        QStandardItem *itemLastModified = new QStandardItem();
        QStandardItem *itemLastRead = new QStandardItem();
        QStandardItem *itemArguments = new QStandardItem();
        QStandardItem *itemComment = new QStandardItem();
        QStandardItem *itemTags = new QStandardItem();

        itemName->setText( name );
        itemFormat->setText( format );
        itemPath->setText( path );
        itemCreated->setText( created );
        itemLastModified->setText( lastModified );
        itemLastRead->setText( lastRead );
        itemArguments->setText( arguments );
        itemComment->setText( comment );
        itemTags->setText( tags );

        list[U::Check] = itemCheck;
        list[U::Name] = itemName;
        list[U::Format] = itemFormat;
        list[U::Path] = itemPath;
        list[U::Created] = itemCreated;
        list[U::LastModified] = itemLastModified;
        list[U::LastRead] = itemLastRead;
        list[U::Arguments] = itemArguments;
        list[U::Comment] = itemComment;
        list[U::Tags] = itemTags;

        if ( phase == Phase::Load )
        {
            bool isUrl = format.contains( "url", Qt::CaseInsensitive );

            // если не существует и не ссылка, не будем добавлять
            if( !QFile( path ).exists() && !isUrl )
                return list;

            itemCheck->setCheckable( true );
            itemCheck->setCheckState( checkedItemTableInit );

            itemCheck->setEditable( false );
            itemName->setEditable( isUrl );
            itemFormat->setEditable( false );
            itemPath->setEditable( isUrl );
            itemCreated->setEditable( false );
            itemLastRead->setEditable( false );
            itemLastModified->setEditable( false );
            itemArguments->setEditable( U::isExe( itemFormat->text() ) );

            itemName->setIcon( isUrl ? QIcon( ":/IconHyperlink" ) : QFileIconProvider().icon( QFileInfo( path ) ) );

            itemName->setData( id, Qt::UserRole );
            itemName->setData( noTag, Qt::UserRole + 1 );
            itemName->setData( isUrl ? Url : File, Qt::UserRole + 2 );

            itemFormat->setTextAlignment( Qt::AlignCenter );

            itemPath->setToolTip( itemPath->text() );
            itemCreated->setToolTip( itemCreated->text() );
            itemLastModified->setToolTip( itemLastModified->text() );
            itemLastRead->setToolTip( itemLastRead->text() );

        }else if ( phase == Phase::AddFile )
        {
            itemCheck->setCheckable(true);
            itemCheck->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

            itemName->setData( noTag, Qt::UserRole + 1 );
            itemName->setData( File, Qt::UserRole + 2 );

            itemName->setIcon( QFileIconProvider().icon( QFileInfo( itemPath->text() ) ) );

            itemPath->setToolTip( itemPath->text() );
            itemCreated->setToolTip( itemCreated->text() );
            itemLastModified->setToolTip( itemLastModified->text() );
            itemLastRead->setToolTip( itemLastRead->text() );

            itemFormat->setTextAlignment( Qt::AlignCenter );

            itemCheck->setEditable( false );
            itemName->setEditable( false );
            itemFormat->setEditable( false );
            itemPath->setEditable( false );
            itemCreated->setEditable( false );
            itemLastModified->setEditable( false );
            itemLastRead->setEditable( false );
            itemArguments->setEditable( U::isExe( itemFormat->text() ) );

        }else if ( phase == Phase::AddUrl )
        {
            itemCheck->setCheckable( true );
            itemCheck->setCheckState( checked ? Qt::Checked : Qt::Unchecked );

            itemName->setData( noTag, Qt::UserRole + 1 );
            itemName->setData( Url, Qt::UserRole + 2 );

            itemName->setIcon( QIcon( ":/IconHyperlink" ) );

            itemFormat->setTextAlignment( Qt::AlignCenter );

            itemPath->setToolTip( itemPath->text() );
            itemCreated->setToolTip( itemCreated->text() );

            itemCheck->setEditable( false );
            itemName->setEditable( true );
            itemFormat->setEditable( false );
            itemPath->setEditable( true );
            itemCreated->setEditable( false );
            itemLastModified->setEditable( false );
            itemLastRead->setEditable( false );
            itemArguments->setEditable( false );
        }

        model->appendRow( list );

        return list;
    }

    static QToolButton * cloneFrom( const QToolButton * button )
    {
        QToolButton * clone = new QToolButton();

        clone->setText( button->text() );
        clone->setToolTip( button->toolTip() );
        clone->setStatusTip( button->statusTip() );
        clone->setIcon( button->icon() );

        QObject::connect( clone, SIGNAL( clicked() ), button, SIGNAL( clicked() ) );

        return clone;
    }

    static bool tagsLessThan( const QListWidgetItem * i1, const QListWidgetItem * i2 )
    {
        return i1->data( Qt::UserRole ).toInt() < i2->data( Qt::UserRole ).toInt();
    }

    static QString fromDateTime(QDateTime dateTime)
    {
        return dateTime.toString(Qt::SystemLocaleLongDate);
    }

    /// Функция возвращает true, если формат исполняемого файла
    static bool isExe(QString format)
    {
        return format.contains(QRegExp("exe|lnk", Qt::CaseInsensitive));
    }

    static bool execute(QString path, QString arguments = QString(), QString *textError = 0)
    {
        QString dirPath = QFileInfo(path).absoluteDir().absolutePath();
        dirPath = QDir::toNativeSeparators(dirPath);

        int returnKey = (int)ShellExecuteA(NULL,
                                           NULL,
                                           path.toUtf8().data(),
                                           arguments.toUtf8().data(),
                                           dirPath.toUtf8().data(),
                                           SW_RESTORE);

        if(returnKey > 32)
            return true;

        QString text;
        switch(returnKey)
        {
        case 0:
            text = "Системе не хватает памяти или ресурсов.";
            break;

        case ERROR_BAD_FORMAT:
            text = ".EXE файл является неправильным (не-Win32 .EXE \nили ошибка в образе .EXE файла).";
            break;

        case ERROR_FILE_NOT_FOUND:
            text = "Заданный файл был не найден.";
            break;

        case ERROR_PATH_NOT_FOUND:
            text = "Заданный путь был не найден.";
            break;

        case SE_ERR_ACCESSDENIED:
            text = "Операционная система отказывает в доступе к указаному файлу.";
            break;

        case SE_ERR_ASSOCINCOMPLETE:
            text = "Имя ассоциированного файла не полное или не правильное.";
            break;

        case SE_ERR_DDEBUSY:
            text = "Транзакция динамического обмена данными (DDE transaction) \nне может быть завершена потому что выпоняются другие DDE транзакции.";
            break;

        case SE_ERR_DDEFAIL:
            text = "Транзакция динамического обмена данными провалилась.";
            break;

        case SE_ERR_DDETIMEOUT:
            text = "Транзакция динамического обмена данными не может быть завершена \nпотому что истекло время ожидания ответа.";
            break;

        case SE_ERR_DLLNOTFOUND:
            text = "Указаная DLL библиотека не найдена.";
            break;

        case SE_ERR_NOASSOC:
            text = "Нет приложений ассоциированных с данным расширением файла.";
            break;

        case SE_ERR_OOM:
            text = "Не достаточно памяти для завершения операции.";
            break;

        case SE_ERR_SHARE:
            text = "Нарушение совместного доступа.";
            break;
        }

        textError->clear();
        textError->append(text);

        return false;
    }

    static bool execute(QString path, QString *textError = 0)
    {
        return execute(path, QString(), textError);
    }

    static void runFile( QString path, QString arguments )
    {
        if( isExe( QFileInfo( path ).suffix() ) )
        {
            QString textError;
            bool successful = execute( path, arguments, &textError );

            if(!successful)
                QMessageBox::information( 0,
                                          "Информация",
                                          textError );

        }else
            if( !QDesktopServices::openUrl( QUrl::fromLocalFile( path ) ) )
                QMessageBox::information( 0,
                                          "Информация",
                                          "Не удалось открыть файл" );
    }

    static bool isValid(QString text)
    {
        // строка валидна, если не пустая и не нуль
        return !(text.isEmpty() || text.isNull());
    }

    static QStringList listTags(QString text)
    {        
        return text.split( tagsSplit );
    }

    template < typename T >
    static void removeDuplicate( QList < T > * list )
    {
        if( list->isEmpty() )
            return;

        qSort( list->begin(), list->end() );

        for( int i = 0; i < list->size() - 1; i++ )
            if( list->at(i) == list->at(i + 1) )            
                list->removeAt(i--);
    }

//    static void moveOnCenter( QWidget * w )
//    {
//        QRect rect = QDesktopWidget().availableGeometry();
//        int cX = ( rect.width() - w->width() ) / 2;
//        int cY = ( rect.height() - w->height() ) / 2;

//        w->move( cX, cY );
//    }

    static void copyFilesToClipboard( const QList < QUrl > & files )
    {
        QMimeData * mime = new QMimeData();
        mime->setUrls( files );

        QApplication::clipboard()->setMimeData( mime );
    }

    static QString currentDateTime()
    {
        return QDateTime::currentDateTime().toString( Qt::SystemLocaleLongDate );
    }
};

class CheckTableItemDelegate : public QStyledItemDelegate
{
public:
    CheckTableItemDelegate(QObject *parent = 0)
        : QStyledItemDelegate(parent)
    {
    }

    //!
    void paint ( QPainter * painter, const QStyleOptionViewItem & option,
                 const QModelIndex & index ) const
    {
        QStyleOptionViewItemV4 viewItemOption(option);
        viewItemOption.state &= ~QStyle::State_HasFocus;
        if (index.column() == 0)
        {
            const int textMargin =
                    QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
            QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                                QSize(option.decorationSize.width() + 5,
                                                      option.decorationSize.height()),
                                                QRect(option.rect.x() + textMargin, option.rect.y(),
                                                      option.rect.width() - (2 * textMargin), option.rect.height()));

            if ((option.state & QStyle::State_Active) &&
                    (option.state & QStyle::State_Selected))
            {
                painter->fillRect(option.rect,
                                  option.palette.brush(QPalette::Highlight));
            }
            else
            {
                if (option.state & QStyle::State_Selected)
                {
                    painter->fillRect(option.rect,
                                      option.palette.brush(QPalette::Inactive, QPalette::Highlight));
                }
            }

            viewItemOption.rect = newRect;
        }
        QStyledItemDelegate::paint(painter, viewItemOption, index);

        if ((option.state & QStyle::State_Active) &&
                (option.state & QStyle::State_HasFocus))
        {
            QStyleOptionFocusRect o;
            o.rect = option.rect;
            o.state |= QStyle::State_KeyboardFocusChange;
            o.state |= QStyle::State_Item;
            QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled)
                    ? QPalette::Normal : QPalette::Disabled;
            o.backgroundColor = option.palette.color(cg,
                                                     (option.state & QStyle::State_Selected) ? QPalette::Highlight : QPalette::Window);
            QApplication::style()->drawPrimitive(QStyle::PE_FrameFocusRect,
                                                 &o, painter);
        }
    }

    //!
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index)
    {
        Q_ASSERT(event);
        Q_ASSERT(model);
        // make sure that the item is checkable
        Qt::ItemFlags flags = model->flags(index);
        if (!(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled))
            return false;
        // make sure that we have a check state
        QVariant value = index.data(Qt::CheckStateRole);
        if (!value.isValid())
            return false;
        // make sure that we have the right event type
        if (event->type() == QEvent::MouseButtonRelease)
        {
            const int textMargin =
                    QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
            QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                                  option.decorationSize,
                                                  QRect(option.rect.x() +
                                                        (2 * textMargin), option.rect.y(),
                                                        option.rect.width() - (2 * textMargin),
                                                        option.rect.height()));
            if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
                return false;
        }
        else if (event->type() == QEvent::KeyPress)
        {
            if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
                    static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
                return false;
        }
        else
        {
            return false;
        }
        Qt::CheckState state =
                (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked ?
                     Qt::Unchecked : Qt::Checked);
        return model->setData(index, state, Qt::CheckStateRole);
    }
};

//class TagsItemDelegate : public QStyledItemDelegate
//{
//public:
//    virtual bool editorEvent( QEvent *event, QAbstractItemModel *model,
//                             const QStyleOptionViewItem &option,
//                             const QModelIndex &index )
//    {
//        Q_UNUSED( event );
//        Q_UNUSED( option );

//        bool ok = false;

//        QString tag = QInputDialog::getText( 0,
//                                             "Добавление тэга",
//                                             "Введите тэг:",
//                                             QLineEdit::Normal,
//                                             QString(),
//                                             &ok );

//        if ( !ok )
//            return true;

//        QString text = index.data().toString();
//        text += tag + tagsSplit;

//        model->setData( index, text, Qt::DisplayRole );

//        return true;
//    }
//};

class CheckedHeader : public QHeaderView
{
    Q_OBJECT
public:
    CheckedHeader(Qt::Orientation orientation, QWidget *parent = 0)
        : QHeaderView(orientation, parent)
        , m_isOn(true)
    {
        // set clickable by default
        setClickable(true);
    }

    void setChecked(bool checked)
    {
        if(isEnabled() && m_isOn != checked)
        {
            m_isOn = checked;
            updateSection(0);
            emit toggled(m_isOn);
        }
    }
    bool isChecked()
    {
        return m_isOn;
    }

signals:
    void toggled(bool checked);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
    {
        painter->save();
        QHeaderView::paintSection(painter, rect, logicalIndex);
        painter->restore();
        if (logicalIndex == 0)
        {
            QStyleOptionButton option;
            if (isEnabled())
                option.state |= QStyle::State_Enabled;
            option.rect = checkBoxRect(rect);
            if (m_isOn)
                option.state |= QStyle::State_On;
            else
                option.state |= QStyle::State_Off;
            style()->drawControl(QStyle::CE_CheckBox, &option, painter);
        }
    }
    void mousePressEvent(QMouseEvent *event)
    {
        if (isEnabled() && logicalIndexAt(event->pos()) == 0)
        {
            m_isOn = !m_isOn;
            updateSection(0);
            emit toggled(m_isOn);
        }
        else QHeaderView::mousePressEvent(event);
    }


private:
    QRect checkBoxRect(const QRect &sourceRect) const
    {
        QStyleOptionButton checkBoxStyleOption;
        QRect checkBoxRect = style()->subElementRect(QStyle::SE_CheckBoxIndicator,
                                                     &checkBoxStyleOption);
        QPoint checkBoxPoint(sourceRect.x() +
                             sourceRect.width() / 2 -
                             checkBoxRect.width() / 2,
                             sourceRect.y() +
                             sourceRect.height() / 2 -
                             checkBoxRect.height() / 2);
        return QRect(checkBoxPoint, checkBoxRect.size());
    }

    bool m_isOn;
};

#endif // USUPPORT_H
