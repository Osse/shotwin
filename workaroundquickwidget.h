#ifndef WORKAROUNDQUICKWIDGET_H
#define WORKAROUNDQUICKWIDGET_H

#include <QQuickWidget>
#include <QQuickItem>

class WorkaroundQuickWidget : public QQuickWidget
{
public:
        explicit WorkaroundQuickWidget(QWidget* parent = Q_NULLPTR) :
                QQuickWidget(parent),
                inputHandling_(false)
        {
                qApp->installEventFilter(this);
        }

        WorkaroundQuickWidget(QQmlEngine* engine, QWidget* parent) :
                QQuickWidget(engine, parent),
                inputHandling_(false)
        {
                qApp->installEventFilter(this);
        }

        explicit WorkaroundQuickWidget(const QUrl &source, QWidget *parent = Q_NULLPTR) :
                QQuickWidget(source, parent),
                inputHandling_(false)
        {
                qApp->installEventFilter(this);
        }

        ~WorkaroundQuickWidget()
        {
                qApp->removeEventFilter(this);
        }

protected:
        bool eventFilter(QObject* object, QEvent* event) override
        {
                switch (event->type())
                {
                case QEvent::KeyPress:
                case QEvent::KeyRelease:
                        if (!inputHandling_)
                        {
                                auto item = qobject_cast<QQuickItem*>(object);
                                if (item && item->window() == quickWindow())
                                {
                                        inputHandling_ = true;
                                        quickWindow()->sendEvent(item, event);
                                        inputHandling_ = false;
                                        return true;
                                }
                        }
                        break;
                }

                return false;
        }

private:
        bool inputHandling_;
};
#endif // WORKAROUNDQUICKWIDGET_H
