#include "titlebar.h"
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

TitleBar::TitleBar(QWidget *parent)
{
    // Don't let this widget inherit the parent's backround color
    setAutoFillBackground(true);
    // Use a brush with a Highlight color role to render the background
    //setBackgroundRole(QPalette::Highlight);

    // set style sheet
    QString css = readTextFile("://styles/titlebar.css");
    this->setStyleSheet(css);

    minimize = new QToolButton(this);
    minimize->setObjectName("minimizeButton");

    maximize = new QToolButton(this);
    maximize->setObjectName("maximizeButton");

    close = new QToolButton(this);
    close->setObjectName("closeButton");

    // Use the style to set the button pixmaps
    QPixmap pix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    close->setIcon(pix);

    maxPix = style()->standardPixmap(QStyle::SP_TitleBarMaxButton);
    maximize->setIcon(maxPix);

    pix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
    minimize->setIcon(pix);

    restorePix = style()->standardPixmap(QStyle::SP_TitleBarNormalButton);

    minimize->setMinimumHeight(20);
    close->setMinimumHeight(20);
    maximize->setMinimumHeight(20);


    QLabel *label = new QLabel(this);
    label->setText("qChat");
    parent->setWindowTitle("qChat");

    QHBoxLayout *hbox = new QHBoxLayout(this);

    hbox->addWidget(label);
    hbox->addWidget(minimize);
    hbox->addWidget(maximize);
    hbox->addWidget(close);

    hbox->insertStretch(1, 500);
    hbox->setSpacing(0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    maxNormal = false;

    connect(close, SIGNAL( clicked() ), parent, SLOT(close() ) );
    connect(minimize, SIGNAL( clicked() ), this, SLOT(showSmall() ) );
    connect(maximize, SIGNAL( clicked() ), this, SLOT(showMaxRestore() ) );
}

// override for correct styling
void TitleBar::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void TitleBar::showSmall()
{
    parentWidget()->showMinimized();
}

void TitleBar::showMaxRestore()
{
    if (maxNormal) {
        parentWidget()->showNormal();
        maxNormal = !maxNormal;
        maximize->setIcon(maxPix);
    } else {
        parentWidget()->showMaximized();
        maxNormal = !maxNormal;
        maximize->setIcon(restorePix);
    }
}

void TitleBar::mousePressEvent(QMouseEvent *me)
{
    startPos = me->globalPos();
    clickPos = mapToParent(me->pos());
}
void TitleBar::mouseMoveEvent(QMouseEvent *me)
{
    if (maxNormal)
        return;
    parentWidget()->move(me->globalPos() - clickPos);
}
