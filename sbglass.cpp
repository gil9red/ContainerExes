#include "sbglass.h"

#include <QScrollBar>
#include <QVBoxLayout>
#include <QList>
#include <QToolBar>
#include <QMenuBar>

SBGlass::SBGlass(QObject *parent)
	: QObject(parent)
    , movie_(0)
    , hideWidgets(false)
{
    wGlass_ = new USimpleGlass( );
    wAnimationContainer_ = new QLabel();
    wInfoTextContaiter_ = new QLabel();
    widgetContainer = new QWidget();

    QVBoxLayout * layout = new QVBoxLayout();
    wGlass_->setLayout( layout );

    Qt::Alignment align = Qt::AlignCenter;
    layout->addStretch();
    layout->addWidget( wAnimationContainer_, 0, align );
    layout->addSpacing( 15 );
    layout->addWidget( wInfoTextContaiter_, 0, align );
    layout->addWidget( widgetContainer, 0, align );
    layout->addStretch();
}

SBGlass::~SBGlass()
{
	wGlass_->deleteLater();
}

/// PUBLIC SLOTS
void SBGlass::install(QWidget* widget, bool d_hideWidgets )
{
    hideWidgets = d_hideWidgets;

    // Для начала удалим его с предыдущего виджета
    remove();

    // Запомним старый фокус и отберем его
    focusPolicy = widget->focusPolicy();
    widget->setFocusPolicy( Qt::NoFocus );

    foreach ( QWidget * child, widget->findChildren < QScrollBar * > () )
    {
        visibleParentChildren[ child ] = true;
        child->hide();
    }

    if ( hideWidgets )
    {
        // Запомним видимость всех виджетов
        foreach ( QWidget * child, widget->findChildren < QToolBar * > () )
        {
            visibleParentChildren[ child ] = true;
            child->hide();
        }
        foreach ( QWidget * child, widget->findChildren < QMenuBar * > () )
        {
            visibleParentChildren[ child ] = true;
            child->hide();
        }
    }

    // Установим стекло поверх виджета
    wGlass_->setParent( widget );

    // Покажем стекло
    wGlass_->show();

    // Начнем отлавливать все события, установив фильтр
    widget->installEventFilter( this );

    // Пошлем событие сами себе, чтобы стекло правильно
    // разместились на виджете
    QEvent event(QEvent::Resize);
    eventFilter(0, &event);

    emit installed();
}

void SBGlass::remove()
{
    QWidget * parent = wGlass_->parentWidget();

    // Если стекло было установлено, то удаляем его
    if ( parent )
    {
        // Вернем фокус
        parent->setFocusPolicy( focusPolicy );

        if ( hideWidgets )
        {
            // Вернем видимость
            QMapIterator < QWidget *, bool > it( visibleParentChildren );
            while ( it.hasNext() )
            {
                it.next();
                it.key()->setVisible( it.value() );
            }
        }

        // Перестаем отлавливать события на низлежащем виджете
        parent->removeEventFilter(this);

        // Скрываем все компоненты стекла
        wGlass_->hide();
        wGlass_->setParent(0);

        emit removed();
    }
}

// Реализуем фильтр событий на виджете. Назначение этого фильтра - не
// допустить работу с виджетом под стеклом и обеспечить актуальные размеры и перерисовку стекла
bool SBGlass::eventFilter(QObject* /* object */, QEvent* event) 
{
    // Если показывается виджет или изменились его размеры, изменяем размеры
    // стекла и позиционируем информационный блок
    if ((event->type() == QEvent::Show) || (event->type() == QEvent::Resize))
    {
        wGlass_->resize(wGlass_->parentWidget()->size());
        wGlass_->move(0, 0);
        return true;
    }

	// убираем возможность вызова сочетаний клавиш
    event->accept();

    return false;
}

// Реализуем методы показа информационного блока
void SBGlass::showInfoTextBlock(const QString& text)
{
    if (!text.isNull())
		wInfoTextContaiter_->setText(text);

	wInfoTextContaiter_->show();
}

void SBGlass::showAnimationBlock(QMovie* movie)
{
    if (movie != 0)
		setMovie(movie);

	wAnimationContainer_->show();
}

void SBGlass::enableColor(const QColor& color )
{
    wGlass_->setColor( color );
}

void SBGlass::disableColor()
{
    wGlass_->setColor( Qt::transparent );
}

void SBGlass::enableOpacity(qreal opacity )
{
    QGraphicsOpacityEffect * tmpEffect = new QGraphicsOpacityEffect();
	tmpEffect->setOpacity(opacity);

	wGlass_->setGraphicsEffect(tmpEffect);
}

void SBGlass::disableOpacity()
{
    wGlass_->setGraphicsEffect( NULL );
}

void SBGlass::enableInfoBlock(QMovie* movie, const QString& text)
{
	enableAnimationBlock(movie);
	enableInfoTextBlock(text);
}

void SBGlass::disableInfoBlock()
{
	wAnimationContainer_->hide();
	wInfoTextContaiter_->hide();
}	

void SBGlass::enableAnimationBlock(QMovie* movie )
{
	setMovie(movie);
	wAnimationContainer_->show();
}

void SBGlass::disableAnimationBlock()
{
	wAnimationContainer_->hide();
}

void SBGlass::enableInfoTextBlock( const QString & text, const QFont & font )
{
    wInfoTextContaiter_->setText( text );
    wInfoTextContaiter_->setFont( font );
	wInfoTextContaiter_->adjustSize();
	wInfoTextContaiter_->show();
}
void SBGlass::disableInfoTextBlock()
{
	wInfoTextContaiter_->hide();
}

void SBGlass::enableWidgetBlock( QLayout * layout )
{
    if ( layout )
        widgetContainer->setLayout( layout );

    widgetContainer->show();
}
void SBGlass::disableWidgetBlock()
{
    widgetContainer->hide();
}

QLabel& SBGlass::getInfoTextBlock()
{
	return *wInfoTextContaiter_;
}

void SBGlass::setMovie(QMovie* movie)
{
	delete movie_;
	movie_ = movie;
	if (movie_ != 0)
	{
		wAnimationContainer_->setMovie(movie_);
		movie_->jumpToFrame(0);
		wAnimationContainer_->resize(movie_->currentPixmap().size());

		wAnimationContainer_->show();
		movie_->start();
	}
	else
        wAnimationContainer_->setMovie( NULL );
}
