#include "mainwindow.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QQuickWidget>
#include <QQuickView>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QScreen>
#include <QQmlEngine>
#include <QQmlContext>
#include <QMessageBox>
#include <QDebug>
#include <QTextStream>
#include "constants.h"
#include "cemodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createWidgets();
    createMenus();

    QSize s = qApp->primaryScreen()->availableSize();
    int h = s.height();
    int w = s.width();
    setGeometry(w*0.25,h*0.25,w*0.5,h*0.5);
    connect(CEModel::getInstance(), &CEModel::resultsAvailable, this, &MainWindow::onResultsAvailable);
}

void MainWindow::createWidgets()
{
    QSplitter* mainWidget = new QSplitter;
    mainWidget->setChildrenCollapsible(false);

    //QQuickWidget* cfgQuickWidget = new QQuickWidget;
    QQuickView* cfgQuickView = new QQuickView;
    cfgQuickView->setSource(QUrl("qrc:/Visualizer/CFGView.qml"));
    QWidget *cfgQuickWidget = QWidget::createWindowContainer(cfgQuickView);

    QVBoxLayout* codeTextEditLayout  = new QVBoxLayout;
    QWidget* codeTextEditWidget = new QWidget;

    p_codeTextEditLabel = new QLabel(tr("Code"));
    p_codeEdit = new CodeTextEdit;
    codeTextEditLayout->addWidget(p_codeTextEditLabel);
    codeTextEditLayout->addWidget(p_codeEdit);
    codeTextEditWidget->setLayout(codeTextEditLayout);

    QVBoxLayout* outputViewLayout  = new QVBoxLayout;
    QWidget* outputViewWidget = new QWidget;

    QLabel* outputViewLabel = new QLabel(tr("Output"));
    p_outputView = new CEListView;
    outputViewLayout->addWidget(outputViewLabel);
    outputViewLayout->addWidget(p_outputView);
    outputViewWidget->setLayout(outputViewLayout);

    p_codeVisual = new QTabWidget;
    connect(p_codeVisual, &QTabWidget::currentChanged, this, &MainWindow::tabChanged);
    p_codeVisual->addTab(codeTextEditWidget, tr("Text"));
    p_codeVisual->addTab(cfgQuickWidget, tr("View"));
    p_codeVisual->setCurrentWidget(codeTextEditWidget);

    mainWidget->addWidget(p_codeVisual);
    mainWidget->addWidget(outputViewWidget); //codeTextEditWidget );

    setCentralWidget(mainWidget);
}

void MainWindow::createMenus()
{
    QMenu* fileMenu = new QMenu(tr("File"));
    QAction* openFileAction = fileMenu->addAction(tr("Open"), QKeySequence(Qt::ALT | Qt::Key_O));
    QMenu* openSampleFileAction = fileMenu->addMenu(tr("Open Sample"));
    QAction* saveFileAction = fileMenu->addAction(tr("Save"), QKeySequence(Qt::ALT | Qt::Key_S));
    QAction* saveAsFileAction = fileMenu->addAction(tr("Save as"));
    QAction* runAction = fileMenu->addAction(tr("Run"), QKeySequence(Qt::ALT | Qt::Key_R));
    QAction* closeAction = fileMenu->addAction(tr("Close"), QKeySequence(Qt::ALT | Qt::Key_C));
    menuBar()->addMenu(fileMenu);

    QAction* openSampleOneFileAction = openSampleFileAction->addAction(tr("Open Sample 1"), QKeySequence(Qt::ALT | Qt::Key_S));
    QAction* openSampleTwoFileAction = openSampleFileAction->addAction(tr("Open Sample 2"), QKeySequence(Qt::ALT | Qt::Key_O));
    QAction* openSampleThreeFileAction = openSampleFileAction->addAction(tr("Open Sample 3"), QKeySequence(Qt::ALT | Qt::Key_S));
    QAction* openSampleFourFileAction = openSampleFileAction->addAction(tr("Open Sample 4"), QKeySequence(Qt::ALT | Qt::Key_O));
    QAction* openSampleFiveFileAction = openSampleFileAction->addAction(tr("Open Sample 5"), QKeySequence(Qt::ALT | Qt::Key_S));
    QAction* openSampleSixFileAction = openSampleFileAction->addAction(tr("Open Sample 6"), QKeySequence(Qt::ALT | Qt::Key_O));

    connect(openSampleOneFileAction, &QAction::triggered, this, &MainWindow::openSampleOne);
    connect(openSampleTwoFileAction, &QAction::triggered, this, &MainWindow::openSampleTwo);
    connect(openSampleThreeFileAction, &QAction::triggered, this, &MainWindow::openSampleThree);
    connect(openSampleFourFileAction, &QAction::triggered, this, &MainWindow::openSampleFour);
    connect(openSampleFiveFileAction, &QAction::triggered, this, &MainWindow::openSampleFive);
    connect(openSampleSixFileAction, &QAction::triggered, this, &MainWindow::openSampleSix);

    connect(openFileAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(saveFileAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(saveAsFileAction, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(runAction, &QAction::triggered, this, &MainWindow::run);
    connect(closeAction, &QAction::triggered, this, &MainWindow::close);

    QToolBar* fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openFileAction);
    fileToolBar->addAction(saveFileAction);
    fileToolBar->addAction(saveAsFileAction);
    fileToolBar->addAction(runAction);
    fileToolBar->addAction(closeAction);
}

void MainWindow::openFile()
{
    p_fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), QString("C:\\workspace\\my_repo\\sca\\test"), tr("C Files (*.c)"));
    QFile file(p_fileName);
    if(!file.open(QIODevice::Text | QIODevice::ReadOnly))
        return;
    QTextStream ts(&file);
    p_codeEdit->setPlainText(ts.readAll());
    file.close();

    QString label(tr("Code: "));
    label.append(p_fileName);
    p_codeTextEditLabel->setText(label);

    statusBar()->showMessage(tr("File opened"), 2000);

    updateViewTab(p_codeVisual->currentIndex());
}

void MainWindow::saveFile()
{
    QString text = p_codeEdit->toPlainText();
    QFile file(p_fileName);
    if (!file.open(QIODevice::Text | QIODevice::WriteOnly))
        return;
    QTextStream ts(&file);
    ts << text;
    file.close();
    QString label("Code: ");
    label.append(p_fileName);
    p_codeTextEditLabel->setText(label);
    statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAsFile()
{
    p_fileName = QFileDialog::getSaveFileName(this,
        tr("Open File"), QString("C:\\workspace\\my_repo\\sca\\test"), tr("C Files (*.c)"));
    QString text = p_codeEdit->toPlainText();
    QFile file(p_fileName);
    if (!file.open(QIODevice::Text | QIODevice::WriteOnly))
        return;
    QTextStream ts(&file);
    ts << text;
    file.close();
    QString label("Code: ");
    label.append(p_fileName);
    p_codeTextEditLabel->setText(label);
    statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::updateViewTab(int index) {
    if (index == 1) {
        QString command("getCFG");
        command.append(' ');
        command.append(p_fileName);
        CEModel::getInstance()->sendCommand(command);

        QString viewLabel(tr("View: "));
        viewLabel.append(p_fileName);
        p_codeVisual->setTabText(1, viewLabel);
    }
    else {
        QString viewLabel(tr("View"));
        p_codeVisual->setTabText(1, viewLabel);
    }
}

void MainWindow::tabChanged(int index)
{
    qDebug() << "current tab " << index;
    updateViewTab(index);
}

void MainWindow::run()
{
    QString command("run");
    command.append(' ');
    command.append(p_fileName);
    CEModel::getInstance()->sendCommand(command);
    statusBar()->showMessage(tr("Run command"), 2000);
}

void MainWindow::onResultsAvailable(QVariantList results)
{
    qDebug()<< Q_FUNC_INFO<<results.size();
    QStandardItemModel* model = new QStandardItemModel(results.size(),0,this);
    CEOutputViewDelegate* delegate = new CEOutputViewDelegate(p_outputView);
    for(int i=0; i<model->rowCount(); i++)
    {
        QVariantMap map = results[i].toMap();
        qDebug() <<map["errorMessage"].toString();
        QStandardItem *item = new QStandardItem();
        item->setData(map["errorMessage"].toString(), Qt::DisplayRole);
        model->setItem(i, 0, item);
    }
    p_outputView->setModel(model);
    p_outputView->setItemDelegate(delegate);
}

void MainWindow::closeWindow()
{
    CEModel::getInstance()->disconnect();
    close();
}

MainWindow::~MainWindow()
{
    qDebug() <<Q_FUNC_INFO;
}

void MainWindow::openSample(QString fileName)
{
    p_fileName = fileName;
    QFile file(p_fileName);
    if (!file.open(QIODevice::Text | QIODevice::ReadOnly))
        return;
    QTextStream ts(&file);
    p_codeEdit->setPlainText(ts.readAll());
    file.close();
    QString label("Code: ");
    label.append(p_fileName);
    p_codeTextEditLabel->setText(label);
    statusBar()->showMessage(tr("File opened"), 2000);

    if (p_codeVisual->currentIndex() == 1) {
        QString command("getCFG");
        command.append(' ');
        command.append(p_fileName);
        CEModel::getInstance()->sendCommand(command);
    }
}

void MainWindow::openSampleOne() {
    openSample("f1.c");
    statusBar()->showMessage(tr("Sample 1 Opened"), 2000);
}

void MainWindow::openSampleTwo() {
    openSample("f2.c");
    statusBar()->showMessage(tr("Sample 2 Opened"), 2000);
}

void MainWindow::openSampleThree() {
    openSample("f3.c");
    statusBar()->showMessage(tr("Sample 3 Opened"), 2000);
}

void MainWindow::openSampleFour() {
    openSample("f4.c");
    statusBar()->showMessage(tr("Sample 4 Opened"), 2000);
}

void MainWindow::openSampleFive() {
    openSample("f5.c");
    statusBar()->showMessage(tr("Sample 5 Opened"), 2000);
}

void MainWindow::openSampleSix() {
    openSample("f6.c");
    statusBar()->showMessage(tr("Sample 6 Opened"), 2000);
}

