#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

typedef struct _object PyObject;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_evaluate_clicked();

    void on_clear_clicked();

private:
    Ui::MainWindow *ui;    
    void print_error(const QString &q);
    void py_print_table(PyObject *, PyObject *);
};
#endif // MAINWINDOW_H
