#ifndef CONNECTWINDOW_H
#define CONNECTWINDOW_H

#include <QWidget>
#include "titlebar.h"

namespace Ui {
class ConnectWindow;
}

class ConnectWindow : public QWidget
{
    Q_OBJECT

    using connect_func = std::function<bool(const QString &userName)>;

public:
    explicit ConnectWindow(QWidget *parent = nullptr);
    ~ConnectWindow();

    void paintEvent(QPaintEvent *);

    template<typename Func>
    void connectFunc(Func&& fn)
    {
        m_connect = std::forward<Func>(fn);
    }

private:
    Ui::ConnectWindow *ui;
    connect_func m_connect;

private slots:
    void on_connectButton_clicked();
};

#endif // CONNECTWINDOW_H
