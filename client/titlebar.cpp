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

TitleBar::TitleBar(QWidget *parent, bool withMaximize)
{
    // Don't let this widget inherit the parent's backround color
    setAutoFillBackground(true);

    m_mouse_down = false;

    // set style sheet
    QString css = readTextFile("://styles/titlebar.css");
    this->setStyleSheet(css);

    minimize = new QToolButton(this);
    minimize->setObjectName("minimizeButton");

    close = new QToolButton(this);
    close->setObjectName("closeButton");

    // Use the style to set the button pixmaps
    QPixmap pix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    close->setIcon(pix);

    pix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
    minimize->setIcon(pix);

    restorePix = style()->standardPixmap(QStyle::SP_TitleBarNormalButton);

    minimize->setMinimumHeight(20);
    close->setMinimumHeight(20);


    QLabel *label = new QLabel(this);
    label->setText("qChat");
    parent->setWindowTitle("qChat");

    QIcon icon("://images/icon.png");
    parent->setWindowIcon(icon);

    QPixmap ic("://images/icon.png");
    QLabel *winIcon = new QLabel(this);
    winIcon->setPixmap(ic.scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    winIcon->setObjectName("winIcon");

    QHBoxLayout *hbox = new QHBoxLayout(this);

    hbox->addWidget(winIcon);
    hbox->addWidget(label);
    hbox->addWidget(minimize);

    // set 'maximize' button if needed for current window
    if(withMaximize)
    {
        maximize = new QToolButton(this);
        maximize->setObjectName("maximizeButton");

        maxPix = style()->standardPixmap(QStyle::SP_TitleBarMaxButton);
        maximize->setIcon(maxPix);
        maximize->setMinimumHeight(20);
        hbox->addWidget(maximize);
        connect(maximize, SIGNAL( clicked() ), this, SLOT(showMaxRestore() ) );
    }

    hbox->addWidget(close);

    hbox->insertStretch(2, 500);
    hbox->setSpacing(0);


    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    maxNormal = false;

    connect(close, SIGNAL( clicked() ), parent, SLOT(close() ) );
    connect(minimize, SIGNAL( clicked() ), this, SLOT(showSmall() ) );
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
    if(!maximize)
        return;

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

void TitleBar::mousePressEvent(QMouseEvent *e)
{
    startPos = e->globalPosition().toPoint();
    clickPos = mapToParent(e->pos());

    m_old_pos =  mapToParent(e->pos());
    m_mouse_down = e->button() == Qt::LeftButton;
}

void TitleBar::mouseMoveEvent(QMouseEvent *e)
{
    if (maxNormal)
        return;

    int x = mapToParent(e->pos()).x();
    int y = mapToParent(e->pos()).y();

    if (m_mouse_down)
    {
        int dx = x - m_old_pos.x();
        int dy = y - m_old_pos.y();

        QRect g = parentWidget()->geometry();

        if (left)
            g.setLeft(g.left() + dx);
        if (right)
            g.setRight(g.right() + dx);
        if(top)
            g.setTop(g.top() + dy);

        if(!left && !right && !top)
        {
            parentWidget()->move(e->globalPosition().toPoint() - clickPos);
            return;
        }

        parentWidget()->setGeometry(g);

        m_old_pos = QPoint(!left ? mapToParent(e->pos()).x() : m_old_pos.x(), mapToParent(e->pos()).y());
    }
    else
    {
        QRect r = parentWidget()->rect();
        left = qAbs(x - r.left()) <= 5;
        right = qAbs(x - r.right()) <= 5;
        top = qAbs(y - r.top()) <= 5;
        bool hor = left | right;

        if (hor && top)
        {
            if (left)
                setCursor(Qt::SizeFDiagCursor);
            else
                setCursor(Qt::SizeBDiagCursor);
        }
        else if (hor)
        {
            setCursor(Qt::SizeHorCursor);
        }
        else if (top)
        {
            setCursor(Qt::SizeVerCursor);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    }
}

void TitleBar::mouseReleaseEvent(QMouseEvent *e)
{
    m_mouse_down = false;
}
