#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QHostAddress>
#include <QHostInfo>

#define CONVERT(x) QString::fromLocal8Bit((x))

void MainWindow::on_actConnect_triggered() {
    QString addr = ui->comboServer->currentText();
    quint16 port = ui->spinPort->value();
    tcpClient->connectToHost(addr, port);
}

void MainWindow::on_actDisconnect_triggered() {
    if (tcpClient->state() == QAbstractSocket::ConnectedState)
        tcpClient->disconnectFromHost();
}

void MainWindow::on_actClear_triggered() {
    ui->plainTextEdit->clear();
}

void MainWindow::onConnected() {
    ui->plainTextEdit->appendPlainText(CONVERT("**已连接到服务器"));
    ui->plainTextEdit->appendPlainText(CONVERT("服务器地址: ") + tcpClient->peerAddress().toString());
    ui->plainTextEdit->appendPlainText(CONVERT("服务器端口: ") + QString::number(tcpClient->peerPort()));
    ui->actConnect->setEnabled(false);
    ui->actDisconnect->setEnabled(true);
}

void MainWindow::onDisconnected() {
    ui->plainTextEdit->appendPlainText(CONVERT("**已断开与服务器的连接"));
    ui->actConnect->setEnabled(true);
    ui->actDisconnect->setEnabled(false);
}

void MainWindow::onSocketStateChange(QAbstractSocket::SocketState socketState) {
    switch(socketState) {
        case QAbstractSocket::UnconnectedState:
            LabSocketState->setText(CONVERT("scoket状态：UnconnectedState"));
            break;
        case QAbstractSocket::HostLookupState:
            LabSocketState->setText(CONVERT("scoket状态：HostLookupState"));
            break;
        case QAbstractSocket::ConnectingState:
            LabSocketState->setText(CONVERT("scoket状态：ConnectingState"));
            break;

        case QAbstractSocket::ConnectedState:
            LabSocketState->setText(CONVERT("scoket状态：ConnectedState"));
            break;

        case QAbstractSocket::BoundState:
            LabSocketState->setText(CONVERT("scoket状态：BoundState"));
            break;

        case QAbstractSocket::ClosingState:
            LabSocketState->setText(CONVERT("scoket状态：ClosingState"));
            break;

        case QAbstractSocket::ListeningState:
            LabSocketState->setText(CONVERT("scoket状态：ListeningState"));
    }
}

void MainWindow::on_btnSend_clicked() {
    QString msg = ui->editMsg->text();
    ui->plainTextEdit->appendPlainText("[out]" + msg);
    ui->editMsg->clear();
    ui->editMsg->setFocus();

    QByteArray str = msg.toUtf8();
    str.append('\n');
    tcpClient->write(str);
}

void MainWindow::onSocketReadyRead() {
    while (tcpClient->canReadLine())
        ui->plainTextEdit->appendPlainText("[in]" + tcpClient->readLine());
}

QString MainWindow::getLocalIP() {
    QString hostName = QHostInfo::localHostName();
    QHostInfo hostInfo = QHostInfo::fromName(hostName);
    QString localIP = "";

    QList<QHostAddress> addList = hostInfo.addresses();
    if (!addList.isEmpty()) {
        for (QHostAddress aHost : addList) {
            if (QAbstractSocket::IPv4Protocol == aHost.protocol()) {
                localIP = aHost.toString();
                break;
            }
        }
    }
    return localIP;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (tcpClient->state() == QAbstractSocket::ConnectedState)
        tcpClient->disconnectFromHost();
    event->accept();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpClient = new QTcpSocket(this);

    LabSocketState = new QLabel(CONVERT("socket状态: "));
    LabSocketState->setMinimumWidth(250);
    ui->statusBar->addWidget(LabSocketState);

    QString localIP = getLocalIP();
    this->setWindowTitle(this->windowTitle() + CONVERT(" 本机IP: ") + localIP);
    ui->comboServer->addItem(localIP);

    connect(tcpClient, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(tcpClient, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(tcpClient, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onSocketStateChange(QAbstractSocket::SocketState)));
    connect(tcpClient, SIGNAL(readyRead()),
            this, SLOT(onSocketReadyRead()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

