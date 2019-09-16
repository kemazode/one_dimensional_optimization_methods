#include <python3.7m/Python.h>
#include <cstdlib>
#include "mainwindow.h"
#include "ui_mainwindow.h"

static const char* module_name = "topu";
static const char* methods[] =
{
    "dihotomic_search",
    "golden_ratio_search",
    "fibonachi_search"
};

static const char* f_table_name = "get_printable_table";
static const int argc = 6;
static const char* mode_max = "max";
static const char* mode_min = "min";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_evaluate_clicked()
{
    bool correctness = true;
    double beg = ui->var_a->text().toDouble(&correctness);
    if (!correctness) {
        print_error("A has an invalid value.");
        return;
    }

    double end = ui->var_b->text().toDouble(&correctness);
    if (!correctness) {
        print_error("B has an invalid value.");
        return;
    }

    if (beg > end) {
        print_error("A must be less than B.");
        return;
    }

    double eps = ui->var_e->text().toDouble(&correctness);
    if (!correctness) {
        print_error("Epsilon is invalid.");
        return;
    }

    double len = ui->var_l->text().toDouble(&correctness);
    if (!correctness) {
        print_error("L has an invalid value.");
        return;
    }

    if (2*eps >= len) {
        print_error("Double epsilon must be less than L.");
        return;
    }

    const QString& func = ui->var_fx->text();

    bool min = ui->check_min->isChecked();
    bool max = ui->check_max->isChecked();

    bool methods_check[sizeof(methods)/sizeof(char *)];
    methods_check[0] = ui->check_dihotomic->isChecked();
    methods_check[1] = ui->check_golden_ratio->isChecked();
    methods_check[2] = ui->check_fibo->isChecked();

    if (not (min or max)) {
        print_error("Choose either options: Min or Max.");
        return;
    }

    bool all_false = true;
    for (auto i : methods_check)
        if (i) all_false = false;
    if (all_false) {
        print_error("Select the method.");
        return;
    }

    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");

    PyObject *py_module_name = PyUnicode_FromString(module_name);
    PyObject *py_module = PyImport_Import(py_module_name);
    Py_DecRef(py_module_name);

    PyObject* py_methods[sizeof(methods)/sizeof(char *)] = {nullptr};
    PyObject* f_table = nullptr;
    PyObject *args = nullptr;
    PyObject *table = nullptr;

    if (py_module)
    {
        for (int i = 0; i < sizeof(methods)/sizeof(char *); ++i) {
            py_methods[i] = PyObject_GetAttrString(py_module, methods[i]);
            if (!py_methods[i] or !PyCallable_Check(py_methods[i])) {
                print_error(QString("Python object \"") + methods[i] + "\" is not exists or is not callable.");
                goto error;
            }
        }

        f_table = PyObject_GetAttrString(py_module, f_table_name);
        if (!f_table or !PyCallable_Check(f_table)) {
            print_error("Python object \"" + QString(f_table_name) + "\" is not exists or is not callable.");
        }
        const char* mode = (max)? mode_max : mode_min;

        // get_printable_table(method(0,0,0....))
        args = PyTuple_New(argc);
        PyTuple_SetItem(args, 0, PyUnicode_FromString(func.toUtf8().data()));
        PyTuple_SetItem(args, 1, PyFloat_FromDouble(beg));
        PyTuple_SetItem(args, 2, PyFloat_FromDouble(end));
        PyTuple_SetItem(args, 3, PyUnicode_FromString(mode));
        PyTuple_SetItem(args, 4, PyFloat_FromDouble(eps));
        PyTuple_SetItem(args, 5, PyFloat_FromDouble(len));

        for (int i = 0; i < sizeof(methods)/sizeof(char *); ++i) {
            if (methods_check[i]) {
                table = PyObject_CallObject(py_methods[i], args);
                if (!table) {
                    print_error("Invalid function.");
                    goto error;
                }
                py_print_table(table, f_table);
            }
        }
    } else {
        print_error("Python module is not exists.");
        goto error;
    }

    error:
    for (auto &i : py_methods)
        Py_DecRef(i);
    Py_DecRef(f_table);
    Py_DecRef(args);
    Py_DecRef(table);
    Py_DecRef(py_module);
    Py_Finalize();        
}

void MainWindow::py_print_table(PyObject *table, PyObject *f_table)
{
    PyObject *arg_for_table = PyTuple_New(1);
    PyTuple_SetItem(arg_for_table, 0, table);

    PyObject *printable_table = PyObject_CallObject(f_table, arg_for_table);
    PyObject *printable_table_repr = PyObject_Str(printable_table);

    const char *char_table = PyUnicode_AsUTF8(printable_table_repr);

    ui->main_output->append(char_table);

    Py_DecRef(arg_for_table);
    Py_DecRef(printable_table_repr);
    Py_DecRef(printable_table);
}
void MainWindow::print_error(const QString &q)
{
    ui->main_output->setText(q);
}

void MainWindow::on_clear_clicked()
{
    ui->main_output->clear();
}
