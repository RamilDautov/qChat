#include "connectwindow.h"
#include "ui_connectwindow.h"
#include <QFile>

static QString readTextFile(const QString &path)
{
    QFile file(path);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        return in.readAll();
    }

    return "";
}

ConnectWindow::ConnectWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectWindow)
{
    // set style sheet
    QString css = readTextFile("://styles/connectwindow.css");
    this->setStyleSheet(css);

    ui->setupUi(this);

    // change color of placeholder
    connect(ui->nameLineEdit, &QLineEdit::textChanged, [=]{ style()->polish(ui->nameLineEdit); });
}

ConnectWindow::~ConnectWindow()
{
    if(!m_mainWindow)
        delete m_mainWindow;

    delete ui;
}

// override for correct styling
void ConnectWindow::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ConnectWindow::on_connectButton_clicked()
{
    ui->welcomeLabel->setText("connecting...");
    ui->welcomeLabel->repaint();
    ui->nameLineEdit->setDisabled(true);

    QString userName = ui->nameLineEdit->text();

    if(userName.size() == 0)
    {
        // make borders red to warn user
        ui->nameLineEdit->setStyleSheet("[text=\"\"]{border: 1px solid red;}");
        return;
    }

    QTime dieTime= QTime::currentTime().addSecs(3);

    bool success = false;
    while (QTime::currentTime() < dieTime && !success)
        success = m_connect(userName);

    if(!success)
    {
        ui->welcomeLabel->setText("No response from server.\nCheck your internet connection and try again");
    }
}
