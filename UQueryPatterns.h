#ifndef UQUERYPATTERNS_H
#define UQUERYPATTERNS_H

#include <QString>

#include "USupport.h"

class UQueryPatterns
{
public:
    static QString select()
    {
        return QString("SELECT * FROM %1").arg(nameTable);
    }
    static QString createTable()
    {
        return QString("CREATE TABLE %1("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "name TEXT, "
                       "format TEXT, "
                       "path TEXT, "
                       "created TEXT, "
                       "lastModified TEXT, "
                       "lastRead TEXT,"
                       "arguments TEXT, "
                       "comment TEXT, "
                       "tags TEXT "
                       ");").arg(nameTable);       
    }
    static QString deleteTable()
    {
        return QString("DROP TABLE %1").arg(nameTable);
    }
    static QString clearTable()
    {
        return QString( "DELETE FROM %1 WHERE 1" ).arg( nameTable );
    }
    static QString insertToTable()
    {
        return QString("INSERT INTO "
                       + nameTable
                       + "(name, format, path, created, lastModified, lastRead, arguments, comment, tags)"
                       + "VALUES('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');");
    }
    static QString deleteRow()
    {
        return QString("DELETE FROM " + nameTable + " WHERE id = '%1';");
    }
//    static QString updateCellTable()
//    {
//        return QString("UPDATE " + nameTable
//                       + " SET %1 = '%2' WHERE id = '%3';");
//    }
    static QString updateCellTable( const QString & table, const QString & name, const QString & value, const QString & where )
    {
        return QString(  "UPDATE %1 SET %2 = '%3' WHERE %4;" )
                .arg( table ).arg( name ).arg( value ).arg( where );
    }
};

#endif // UQUERYPATTERNS_H
