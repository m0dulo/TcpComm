#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTcpServer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QLabel* LabListen;
    QLabel* LabSocketState;

    QTcpServer* tcpServer;
    QTcpSocket* tcpSocket;

    QString getLocalIP();

protected:
    void  closeEvent(QCloseEvent* event);

private slots:
    void onNewConnection();
    void onSocketStateChange(QAbstractSocket::SocketState socketState);
    void onClientConnected();
    void onClientDisconnected();
    void onSocketReadyRead();

    void on_actStart_triggered();
    void on_actStop_triggered();
    void on_actClear_triggered();
    void on_btnSend_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
