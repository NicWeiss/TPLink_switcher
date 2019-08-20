#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QTime>
#include <QString>
#include <QLabel>
#include <QLCDNumber>
#include <QTimeEdit>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QStringList>
#include <QDir>
#include <QFileInfoList>
#include <QMessageBox>
#include <QIODevice>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    /* Виртуальная функция родительского класса в нашем классе
     * переопределяется для изменения поведения приложения,
     *  чтобы оно сворачивалось в трей, когда мы этого хотим
     */
    void closeEvent(QCloseEvent * event);


private slots:
    void on_pushButton_clicked();
    void updateTime();
    void updateQuery();
    void on_pushButton_3_clicked();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void on_pushButton_4_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_5_clicked();

    void on_tableWidget_clicked(const QModelIndex &index);

    void on_pushButton_7_clicked();

private:
    Ui::MainWindow *ui;
    void post_request();
    void new_token_request();
    QNetworkAccessManager *network;
    QTimer *tmr;
    QTimer *tm_query;
    double realy_state;
    QString  responce_device;
    /* Объявляем объект будущей иконки приложения для трея */
    QSystemTrayIcon         * trayIcon;

public slots:
    //yakusu.ru
    void network_replyFinished(QNetworkReply* rep);



};

#endif // MAINWINDOW_H
