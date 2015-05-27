#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class AppDelegate;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setCocosAppDelegate(AppDelegate *appDelegate);
protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;
    AppDelegate *m_appDelegate;
};

#endif // MAINWINDOW_H
