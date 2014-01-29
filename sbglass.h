#ifndef SBGLASS_H
#define SBGLASS_H

/*
****************************************
[5/22/2013 Bepec]Специально для prog.org.ru
Часть кода взята из статьи "Qt. Накрываем виджеты стеклом" (http://www.quizful.net/post/glass-qt-component)
****************************************
*/

#include <QObject>
#include <QLabel>
#include <QMovie>
#include <QEvent>
#include <QBrush>
#include <QDebug>
#include <QGraphicsOpacityEffect>
#include <QPainter>

class USimpleGlass : public QLabel
{
public:
    USimpleGlass( const QColor & color = Qt::transparent )
        : d_color( color ) { }

    void setColor( const QColor & color )
    {
        d_color = color;
        repaint();
    }

    QColor color()
    { return d_color; }

private:
    QColor d_color;

protected:
    void paintEvent( QPaintEvent * )
    {
        QPainter painter( this );
        painter.setPen( Qt::NoPen );
        painter.setBrush( d_color );
        painter.drawRect( rect() );
    }
};

class SBGlass: public QObject
{
	Q_OBJECT

public:
	SBGlass(QObject *parent = 0);
	~SBGlass();

    void enableColor( const QColor& color = Qt::gray );
	void disableColor();

	void enableOpacity(qreal opacity = 0.5);
	void disableOpacity();

	void enableInfoBlock(QMovie* movie = 0, const QString& text = QString::null);
	void disableInfoBlock();

	void enableAnimationBlock(QMovie* movie = 0);
	void disableAnimationBlock();

    void enableInfoTextBlock(const QString& text = QString::null, const QFont &font = QFont());
	void disableInfoTextBlock();

    // Виджет, на котором можно разместить другие виджеты,
    // например, кнопки
    void enableWidgetBlock( QLayout * layout = 0 );
    void disableWidgetBlock();

	QLabel& getInfoTextBlock();
	void setMovie(QMovie* movie);

public slots:
    void install(QWidget * w , bool d_hideWidgets = false );
    void remove();

protected:
	bool eventFilter(QObject* object, QEvent* event);

private:
	void showInfoTextBlock(const QString& text = QString::null);
	void showAnimationBlock(QMovie* movie = 0);

private:
    USimpleGlass * wGlass_;
	QLabel* wAnimationContainer_;
	QLabel* wInfoTextContaiter_;
	QMovie* movie_;
    QWidget * widgetContainer;

    Qt::FocusPolicy focusPolicy;

    bool hideWidgets;
    QMap < QWidget *, bool > visibleParentChildren;

signals:
    void installed();
    void removed();
};

#include "UPasswordWidget.h"
#include <QVBoxLayout>

class UPasswordGlass: public SBGlass
{
    Q_OBJECT

public:
    UPasswordGlass( QWidget * parent = 0 )
        : SBGlass( parent )
    {
        passwordWidget.setVisibleCancelButton( false );

        QVBoxLayout * layoutPassword = new QVBoxLayout();
        layoutPassword->setMargin( 0 );
        layoutPassword->addWidget( &passwordWidget, 0, Qt::AlignCenter );

//        Добавим на стекло виджет
        enableWidgetBlock( layoutPassword );

        connect( &passwordWidget, SIGNAL( successfully() ), SIGNAL( successfully() ) );
        connect( &passwordWidget, SIGNAL( fail() ), SIGNAL( fail() ) );
        connect( &passwordWidget, SIGNAL( correct(bool) ), SIGNAL( correct(bool) ) );
    }

    void setProtectedWidget( QWidget * widget )
    { d_protectedWidget = widget; }

    QWidget * protectedWidget()
    { return d_protectedWidget; }

    void setVisiblePageSecretQuestion( const bool visible )
    { passwordWidget.setVisiblePageSecretQuestion( visible ); }

private:
    void install( QWidget *w, bool hideWidgets = false )
    { SBGlass::install( w, hideWidgets ); }

public slots:
    void install()
    {
        // нельзя будет поставить защиту, если пароль не указан
        if ( passwordWidget.correctPassword().isEmpty() )
            return;

        install( d_protectedWidget, true );
    }

    void remove()
    { SBGlass::remove(); }

    void setCorrectPassword( const QString & text )
    { passwordWidget.setCorrectPassword( text ); }

    void setDisplayPassword( const bool display )
    { passwordWidget.setDisplayPassword( display ); }

    void setSecretQuestion( const QString & text )
    { passwordWidget.setSecretQuestion( text ); }

    void setAnswer( const QString & text )
    { passwordWidget.setAnswer( text ); }

    void setError( const QString & text )
    { passwordWidget.setError( text ); }

private:
    UPasswordWidget passwordWidget;
    QWidget * d_protectedWidget;

signals:
    void correct( bool );
    void successfully();
    void fail();

//protected:
//    bool eventFilter( QObject * object, QEvent * event )
//    {
//        return SBGlass::eventFilter( object, event );
//    }
};


#endif // SBGLASS_H
