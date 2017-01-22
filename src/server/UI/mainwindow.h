// Copyright (C) 2014-2017 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/FireNET/blob/master/LICENSE

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui
{
    class MainWindow;
}

enum ELogType
{
    ELog_Debug,
    ELog_Info,
    ELog_Warning,
    ELog_Error,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public:
    void LogToOutput(ELogType type, const QString& msg);
public slots:
    void CleanUp();
	void UpdateServerStatus();
private slots:
    void on_Input_returnPressed();
private:
	void ClearOutput();
	void ClearStatus();
private:
    Ui::MainWindow *ui;
    int m_OutputItemID;
	QTimer m_UpdateTimer;
};

#endif // MAINWINDOW_H
