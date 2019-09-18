#include <python3.7m/Python.h>
#include <QDoubleValidator>
#include <cstdlib>
#include "mainwindow.h"
#include "ui_mainwindow.h"

constexpr const char* MODULE_NAME = "topu";
static const char* METHODS[] =
{
    "dihotomic_search",
    "golden_ratio_search",
    "fibonachi_search"
};
static const char* MODES[] =
{
  "max",
  "min"
};
constexpr const char* GET_TABLE_NAME = "get_printable_table";
constexpr const int ARGC = 6;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->modes->setId(ui->check_max, 0);
    ui->modes->setId(ui->check_min, 1);

    ui->methods->setId(ui->check_dihotomic, 0);
    ui->methods->setId(ui->check_golden_ratio, 1);
    ui->methods->setId(ui->check_fibo, 2);

    auto validator = new QDoubleValidator();
    validator->setNotation(QDoubleValidator::Notation::ScientificNotation);
    validator->setLocale(QLocale(QLocale::English));
    ui->a->setValidator(validator);
    ui->b->setValidator(validator);
    ui->len->setValidator(validator);
    ui->eps->setValidator(validator);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_evaluate_clicked()
{
    double beg = ui->a->text().toDouble();
    double end = ui->b->text().toDouble();
    double eps = ui->eps->text().toDouble();
    double len = ui->len->text().toDouble();

    if (2*eps >= len) {
        print_error("Error: Double epsilon must be less than L");
        return;
    }

    if (beg > end) {
        print_error("Error: A must be less than B");
        return;
    }

    int mode = ui->modes->checkedId();
    int method = ui->methods->checkedId();

    Py_Initialize();

    PyObject *py_module_name = PyUnicode_FromString(MODULE_NAME);
    PyObject *py_module = PyImport_Import(py_module_name);
    Py_DecRef(py_module_name);

    PyObject *py_method = nullptr;
    PyObject *py_get_table = nullptr;
    PyObject *py_args = nullptr;
    PyObject *py_arg = nullptr;
    PyObject *py_list = nullptr;
    PyObject *py_table = nullptr;

    if (py_module)
    {
        py_method = PyObject_GetAttrString(py_module, METHODS[method]);
        if (!py_method or !PyCallable_Check(py_method)) {
            print_error(QString("Error: Python object \"") + METHODS[method] + "\" is not exists or is not callable");
            goto error;
        }

        py_get_table = PyObject_GetAttrString(py_module, GET_TABLE_NAME);
        if (!py_get_table or !PyCallable_Check(py_get_table)) {
            print_error("Error: Python object \"" + QString(GET_TABLE_NAME) + "\" is not exists or is not callable");
            goto error;
        }

        py_args = PyTuple_Pack(ARGC,
                            PyUnicode_FromString(ui->f->text().toUtf8().data()),
                            PyFloat_FromDouble(beg),
                            PyFloat_FromDouble(end),
                            PyUnicode_FromString(MODES[mode]),
                            PyFloat_FromDouble(eps),
                            PyFloat_FromDouble(len));


        py_list = PyObject_CallObject(py_method, py_args);
        if (!py_list) {
            print_error("Error: Invalid function \"" + ui->f->text() + "\"");
            goto error;
        }

        py_arg = PyTuple_Pack(1, py_list);
        py_table = PyObject_CallObject(py_get_table, py_arg);
        ui->output->append(PyUnicode_AsUTF8(py_table));

    } else {
        print_error("Error: Python module is not exists");
        goto error;
    }

    error:
    Py_DecRef(py_method);
    Py_DecRef(py_get_table);
    Py_DecRef(py_args);
    Py_DecRef(py_arg);
    Py_DecRef(py_list);
    Py_DecRef(py_module);    

    Py_Finalize();        
}

void MainWindow::print_error(const QString &qstr)
{
    ui->output->append("<font color=red>" + qstr + "</font>");
}

void MainWindow::on_clear_clicked()
{
    ui->output->clear();
}
