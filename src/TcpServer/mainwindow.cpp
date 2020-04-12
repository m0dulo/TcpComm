#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtNetwork>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#define CONVERT(x) QString::fromLocal8Bit((x))

void MainWindow::on_actStart_triggered() {
    QString IP = ui->comboIP->currentText();
    quint16 port = ui->spinPort->value();
    QHostAddress addr(IP);
    tcpServer->listen(addr, port);
    ui->plainTextEdit->appendPlainText(CONVERT("**开始监听**"));
    ui->plainTextEdit->appendPlainText(CONVERT("**服务器地址**: ") + tcpServer->serverAddress().toString());
    ui->plainTextEdit->appendPlainText(CONVERT("**服务器端口**: ") + QString::number(tcpServer->serverPort()));
    ui->actStart->setEnabled(false);
    ui->actStop->setEnabled(true);
    LabListen->setText(CONVERT("监听状态: 正在监听"));
}

void MainWindow::onNewConnection() {
    tcpSocket = tcpServer->nextPendingConnection();

    connect(tcpSocket, SIGNAL(connected()), this, SLOT(onClientConnected()));
    onClientConnected();
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()) );
    connect(tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onSocketStateChange(QAbstractSocket::SocketState)));
    onSocketStateChange(tcpSocket->state());
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
}

void MainWindow::onClientConnected() {
    ui->plainTextEdit->appendPlainText(CONVERT("客户端socket接入"));
    ui->plainTextEdit->appendPlainText(CONVERT("客户端地址: ") + tcpSocket->peerAddress().toString());
    ui->plainTextEdit->appendPlainText(CONVERT("客户端端口: ") + QString::number(tcpSocket->peerPort()));
}

void MainWindow::onClientDisconnected() {
    ui->plainTextEdit->appendPlainText(CONVERT("客户端socket连接断开"));
    tcpSocket->deleteLater();
}

void MainWindow::onSocketStateChange(QAbstractSocket::SocketState socketState) {
    switch(socketState) {
        case QAbstractSocket::UnconnectedState:
            LabSocketState->setText(CONVERT("socket状态: 未连接"));
            break;
        case QAbstractSocket::ConnectedState:
            LabSocketState->setText(CONVERT("socket状态: 已连接"));
    }
}

void MainWindow::on_actStop_triggered() {
    if (tcpServer->isListening()) {
        tcpServer->close();
        ui->actStart->setEnabled(true);
        ui->actStop->setEnabled(false);
        LabListen->setText(CONVERT("监听状态：已停止监听"));
    }
}

void MainWindow::on_btnSend_clicked() {
    QString msg = ui->editMsg->text();
    ui->plainTextEdit->appendPlainText("[out] " + msg);
    ui->editMsg->clear();
    ui->editMsg->setFocus();

    QByteArray str = msg.toUtf8();
    str.append('\n');
    tcpSocket->write(str);
}

void MainWindow::onSocketReadyRead() {
    while(tcpSocket->canReadLine()) {
        QString line = tcpSocket->readLine();
        QRegularExpression re("[a-z]");
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            ui->plainTextEdit->appendPlainText("[CONVERT in]" + line);
            QByteArray str = line.toUpper().toUtf8();
            tcpSocket->write(str);
            ui->plainTextEdit->appendPlainText("[CONVERT out] " + str);
        } else {
            ui->plainTextEdit->appendPlainText("[in]" + line);
        }
    }
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

void MainWindow::closeEvent(QCloseEvent* event) {
    if (tcpServer->isListening())
        tcpServer->close();
    event->accept();
}

void MainWindow::on_actClear_triggered() {
    ui->plainTextEdit->clear();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LabListen = new QLabel(CONVERT("监听状态"));
    LabListen->setMinimumWidth(150);
    ui->statusBar->addWidget(LabListen);

    LabSocketState = new QLabel(CONVERT("socket状态:"));
    LabSocketState->setMinimumWidth(200);
    ui->statusBar->addWidget(LabSocketState);

    QString localIP = getLocalIP();
    this->setWindowTitle(this->windowTitle() + CONVERT("本机IP:") + localIP);
    ui->comboIP->addItem(localIP);

    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

