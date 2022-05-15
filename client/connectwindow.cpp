#include "connectwindow.h"
#include "titlebar.h"
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

    // Make this a borderless window which can't
    // be resized or moved via the window system
    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);

    TitleBar *titleBar = new TitleBar(this, false);
    titleBar->setObjectName("titleBar");

    ui->setupUi(this);

    ui->verticalLayout->setContentsMargins(0,0,0,15);

    ui->verticalLayout->insertWidget(0, titleBar);

    QPixmap logo("://images/qChat.png");
    ui->logoLabel->setPixmap(logo.scaled(200, 200, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    // change color of placeholder
    connect(ui->nameLineEdit, &QLineEdit::textChanged, [=]{ style()->polish(ui->nameLineEdit); });
}

ConnectWindow::~ConnectWindow()
{
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
    QString userName = ui->nameLineEdit->text();

    if(userName.size() == 0)
    {
        // make borders red to warn user
        ui->nameLineEdit->setStyleSheet("[text=\"\"]{border: 1px solid red;}");
        return;
    }

    QFont font = ui->welcomeLabel->font();
    font.setPointSize(14);
    ui->welcomeLabel->setFont(font);

    ui->welcomeLabel->setText("connecting...");
    ui->welcomeLabel->repaint();
    ui->nameLineEdit->setEnabled(false);
    ui->connectButton->setEnabled(false);

    QTime dieTime= QTime::currentTime().addSecs(3);

    bool success = false;
    while (QTime::currentTime() < dieTime && !success)
        success = m_connect(userName);

    if(!success)
    {
        QFont font = ui->welcomeLabel->font();
        font.setPointSize(10);
        ui->welcomeLabel->setFont(font);
        ui->welcomeLabel->setText("No response from server.\nCheck your internet connection\nand try again...");
        ui->nameLineEdit->setEnabled(true);
        ui->connectButton->setEnabled(true);
    }
    else
    {
        this->hide();
    }
}
