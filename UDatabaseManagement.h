#ifndef UDATABASEMANAGEMENT_H
#define UDATABASEMANAGEMENT_H

#include <QString>
#include <QDebug>
#include <QStandardItem>

#include <QtSql>

#include "USupport.h"
#include "UQueryPatterns.h"

#include <QFuture>
#include <QtConcurrentRun>
#include <QThreadPool>
#include <QFutureWatcher>
#include <QDebug>

class UDBManagement
{
public:
    static QSqlQuery sendQuery( QString query, bool runInSeparateThread = true )
    {
        QSqlQuery sqlQuery;
        bool successful = false;

        if ( runInSeparateThread )
            QtConcurrent::run( sqlQuery, &QSqlQuery::exec, query );
            
        else
        {
            successful = sqlQuery.exec( query );

            if( !successful )
                qDebug() << query << "ошибка:" << sqlQuery.lastError().text();
        }

        return sqlQuery;
    }

    static QSqlQuery sendQuery( QString query, QSqlDatabase db, bool runInSeparateThread = true )
    {
        QSqlQuery sqlQuery( query, db );

        bool successful = false;

        if ( runInSeparateThread )
        {
            QtConcurrent::run( sqlQuery, &QSqlQuery::exec, query );

        }else
        {
            successful = sqlQuery.exec( query );

            if( !successful )
                qDebug() << query << "ошибка:" << sqlQuery.lastError().text();
        }

        return sqlQuery;
    }

    static void createTable( bool runInSeparateThread = false )
    {
        sendQuery( UQueryPatterns::createTable(), runInSeparateThread );
    }

    static void deleteTable( bool runInSeparateThread = false )
    {
        sendQuery( UQueryPatterns::deleteTable(), runInSeparateThread );
    }
    static void recreateTable( bool runInSeparateThread = false )
    {
        deleteTable( runInSeparateThread );
        createTable( runInSeparateThread );
    }
    static void clearTable( bool runInSeparateThread = true )
    {
        sendQuery( UQueryPatterns::clearTable(), runInSeparateThread );
    }
    static void openDB()
    {
        QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE" );
        db.setDatabaseName( nameDBFile );        

        if( !db.open() )
        {
            qDebug() << "Не удалось подключиться к БД:"
                     << db.lastError().text();
            return;

        }else
            qDebug() << "Установлено подключение к базе данных" << nameDBFile;
    }

    static void insertToDB( QList < QStandardItem * > * list )
    {
        QString insert = UQueryPatterns::insertToTable();

        QString query = insert
                .arg( list->at( U::Name )->text() )
                .arg( list->at( U::Format )->text() )
                .arg( list->at( U::Path )->text() )
                .arg( list->at( U::Created )->text() )
                .arg( list->at( U::LastModified )->text() )
                .arg( list->at( U::LastRead )->text() )
                .arg( list->at( U::Arguments )->text() )
                .arg( list->at( U::Comment )->text() )
                .arg( list->at( U::Tags )->text() );

        QSqlQuery sqlQuery = sendQuery( query );

        list->at( U::Name )->setData( sqlQuery.lastInsertId(), Qt::UserRole );
    }
    static void insertToDB( const QStringList & list )
    {
        QString insert = UQueryPatterns::insertToTable();

        QString query = insert
                .arg( list.at( U::Name ) )
                .arg( list.at( U::Format ) )
                .arg( list.at( U::Path ) )
                .arg( list.at( U::Created ) )
                .arg( list.at( U::LastModified ) )
                .arg( list.at( U::LastRead ) )
                .arg( list.at( U::Arguments ) )
                .arg( list.at( U::Comment ) )
                .arg( list.at( U::Tags ) );

        sendQuery( query );
    }
    static void deleteRow( const int id, bool runInSeparateThread = true )
    {
        sendQuery( UQueryPatterns::deleteRow().arg( id ),
                   runInSeparateThread );
    }

    static void updateCell( QString nameColumn, QString value, int id )
    {
        QString query = UQueryPatterns::updateCellTable( nameTable,
                                                         nameColumn,
                                                         value,
                                                         QString( "id='%1'" )
                                                         .arg( id ) );

        sendQuery( query );
    }

    // Вернет количество строк в таблице базы данных
    static uint numberOfLines( const QString & table, QSqlDatabase db )
    {
        QString query = QString("SELECT COUNT(*) FROM %1").arg( table );
        QSqlQuery sqlQuery = sendQuery( query, db, false );
        sqlQuery.next();

        return sqlQuery.record().value( 0 ).toInt();
    }

    static uint numberOfLines( const QString & table )
    {
        QString query = QString("SELECT COUNT(*) FROM %1").arg( table );
        QSqlQuery sqlQuery = sendQuery( query, false );
        sqlQuery.next();

        return sqlQuery.record().value( 0 ).toInt();
    }

    static QMap < QString, QVariant > settingsPassword()
    {
        QMap < QString, QVariant > result;

        QSqlQuery sqlQuery = sendQuery( "SELECT * FROM Password", false );
        QSqlRecord record = sqlQuery.record();

        const QString column1 = "CorrectPassword";
        const QString column2 = "SecretQuestion";
        const QString column3 = "Answer";
        const QString column4 = "Enabled";

        sqlQuery.next();
        result[ column1 ] = sqlQuery.value( record.indexOf( column1 ) );
        result[ column2 ] = sqlQuery.value( record.indexOf( column2 ) );
        result[ column3 ] = sqlQuery.value( record.indexOf( column3 ) );
        result[ column4 ] = sqlQuery.value( record.indexOf( column4 ) );

        return result;
    }
    static void updateSettingsPasswordValue( const QString & name, const QString & value )
    {
        sendQuery( UQueryPatterns::updateCellTable( "Password",
                                                    name,
                                                    value,
                                                    "id='0'" ), false );
    }
};

#endif // UDATABASEMANAGEMENT_H
