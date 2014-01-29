#include <QApplication>
#include "UContainerExes.h"

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );
    app.setWindowIcon( QIcon( ":/IconProgram" ) );
    app.setApplicationName( "PathBasket" );
    app.setApplicationVersion( "0.0.1" );    

    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( codec ) );

    const Qt::CaseSensitivity cs = Qt::CaseInsensitive;

    foreach ( QString arg , app.arguments() )
    {
        if ( arg.contains( QRegExp( "^-create$", cs ) ) )
            UDBManagement::createTable( false );

        else if ( arg.contains( QRegExp( "^-delete$", cs ) ) )
            UDBManagement::deleteTable( false );

        else if ( arg.contains( QRegExp( "^-clear$", cs ) ) )
            UDBManagement::clearTable( false );

        else if ( arg.contains( QRegExp( "^-recreate$", cs ) ) )
            UDBManagement::recreateTable( false );
    }

 //    UDBManagement::sendQuery(QString("ALTER TABLE %1 ADD COLUMN created TEXT;")
//                             .arg(nameTable));

    // Создаем подключение к БД
    UDBManagement::openDB();

    //    UDBManagement::createTable( false );

//    UDBManagement::sendQuery( "DROP TABLE Password", false );
//    UDBManagement::sendQuery( "CREATE TABLE Password("
//                              "id INTEGER DEFAULT 0, "
//                              "CorrectPassword TEXT, "
//                              "SecretQuestion TEXT, "
//                              "Answer TEXT, "
//                              "Enabled TEXT);", false );

//    UDBManagement::sendQuery( QString( "INSERT INTO "
//                                       "Password (CorrectPassword, SecretQuestion, Answer, Enabled)"
//                                       "VALUES ('%1', '%2', '%3', '%4');" )
//                              .arg( "" )
//                              .arg( "" )
//                              .arg( "" )
//                              .arg( "False" ) );


//    UDBManagement::updateSettingsPasswordValue( "CorrectPassword", "Великий Ктулху" );
//    UDBManagement::updateSettingsPasswordValue( "SecretQuestion", "2 + 2 * 2 = " );
//    UDBManagement::updateSettingsPasswordValue( "Answer", "6" );
//    UDBManagement::updateSettingsPasswordValue( "Enabled", "true" );

    // Получим настройку защиты
    QMap < QString, QVariant > settings = UDBManagement::settingsPassword();
    QString password = settings[ "CorrectPassword" ].toString();
    QString secretQuestion = settings[ "SecretQuestion" ].toString();
    QString answer = settings[ "Answer" ].toString();
    bool enabledPassword = settings[ "Enabled" ].toBool();

//    qDebug() << "Пароль:" << password;
//    qDebug() << "Секретный вопрос:" << secretQuestion;
//    qDebug() << "Ответ:" << answer;
//    qDebug() << "Защита включена:" << enabledPassword;


    UConteinerExes conteinerExes;
    conteinerExes.setWindowTitle( QString( "%1 - %2 - Версия %3" )
                                  .arg( app.applicationName() )
                                  .arg( "Программа хранения путей и адресов Url" )
                                  .arg( app.applicationVersion() ) );
    conteinerExes.show();


    UPasswordGlass glassPassword;   
    glassPassword.enableColor( Qt::darkCyan );
    glassPassword.enableOpacity( 1.0 );
    glassPassword.enableInfoTextBlock( QString( "Доступ закрыт, введите пароль." ),
                                       QFont( "Times", 14, QFont::Bold ) );

    glassPassword.setCorrectPassword( password );
    glassPassword.setSecretQuestion( secretQuestion );
    glassPassword.setAnswer( answer );
    glassPassword.setError( "Введенный пароль оказался не верным :(" );

    // Выберем виджет, на который будем ставить защиту
    glassPassword.setProtectedWidget( &conteinerExes );

    // Если пароль отгадан, снимаем "стекло"
    QObject::connect( &glassPassword, SIGNAL( successfully() ),
                      &glassPassword, SLOT( remove() ) );

    // Если "стекло" сняли, вызовим инициализацию
    // подконтрольного виджета
    QObject::connect( &glassPassword, SIGNAL( removed() ),
                      &conteinerExes, SLOT( load() ) );


    // если защита включена
    if ( enabledPassword )
        glassPassword.install(); // Установим "защитное стекло"
    else
        conteinerExes.load();

    return app.exec();
}
