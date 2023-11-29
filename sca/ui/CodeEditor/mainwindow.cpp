#include "mainwindow.h"
#include<QHBoxLayout>
#include<QMenu>
#include<QMenuBar>
#include<QApplication>
#include<QFile>
#include<QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget* mainWidget = new QWidget;
    QHBoxLayout* hLayout = new QHBoxLayout;

    p_codeEdit = new QTextEdit;
    p_outputView = new QListView;

    hLayout->addWidget(p_codeEdit);
    hLayout->addWidget(p_outputView);

    mainWidget->setLayout(hLayout);
    setCentralWidget(mainWidget);

    createMenus();

    setGeometry(50,50,1400,700);
}

void MainWindow::createMenus()
{
    QMenu* fileMenu = new QMenu(tr("File"));
    QAction* openFileAction = new QAction(tr("Open"), this);
    fileMenu->addAction(openFileAction);
    QAction* runAction = new QAction(tr("Run"), this);
    fileMenu->addAction(runAction);
    QAction* closeAction = new QAction(tr("Close"), this);
    fileMenu->addAction(closeAction);
    menuBar()->addMenu(fileMenu);

    connect(openFileAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(runAction, &QAction::triggered, this, &MainWindow::run);
    connect(closeAction, &QAction::triggered, this, &MainWindow::close);
}

void MainWindow::openFile()
{
    QFile file("C:\\workspace\\my_repo\\sca\\test\\instructions.c");
    if(!file.open(QIODevice::Text | QIODevice::ReadOnly))
        return;
    QTextStream ts(&file);
    p_codeEdit->setPlainText(ts.readAll());
    file.close();
}

void MainWindow::run()
{
    QStringListModel* model = new QStringListModel(this);
    QStringList messages;
    messages <<"a" <<"b" <<"c";
    model->setStringList(messages);
    p_outputView->setModel(model);
}

void MainWindow::closeWindow()
{
    close();
}

MainWindow::~MainWindow()
{
}

