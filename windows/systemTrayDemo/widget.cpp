#include "widget.h"
#include "ui_widget.h"

// Widget

Widget::Widget(QWidget *parent)
    : QTabWidget(parent)
    , ui(new Ui::Widget)
{
	ui->setupUi(this);

	m_Icon.addFile(":/res/default-icon.png");

	m_systemTrayIcon = new SystemTrayIcon_(NULL); // NULL here very important! bug in qt. should be fixed in qt 4.8
	m_systemTrayIcon->setIcon(m_Icon);
	m_systemTrayIcon->show();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onClick()));
}

Widget::~Widget()
{      
	delete m_systemTrayIcon;
	delete ui;  
}

void Widget::onClick()
{
    if (m_systemTrayIcon)
    {
        m_systemTrayIcon->showMessage("TEST", "message");
    }
}
