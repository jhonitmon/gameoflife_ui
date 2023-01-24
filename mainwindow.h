#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>
#include <QPainter>
#include <iostream>
#include <string>
#include <QThread>
#include <unistd.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void iterate();

private slots:

    void on_btn_pause_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_box_start_currentTextChanged(const QString &arg1);

    void on_btn_reset_clicked();

private:
    static const int HEIGHT = 400; // height of the matrix
    static const int WIDTH = 400; // width of the matrix
    int matrix[HEIGHT][WIDTH] = {}; // matrix containing the values
    int iter = 0; // current iteration
    int n_cells = 0; // number of active cells
    int rel_height = HEIGHT / 2 - 1; // relative height
    double speed = 0.05; // iteration pause time in ms
    bool paused = true; // bool to check if game is paused
    bool started = false; // bool to check if the games has started at least one
    unsigned int microsecond = 1000000 * speed; // microsecond multiplier
    // vectors containing the coordinates of the cells to be created/deleted
    std::vector<int> a_coord_x;
    std::vector<int> a_coord_y;
    std::vector<int> d_coord_x;
    std::vector<int> d_coord_y;
    QThread *thread; // thread to calculate the matrix
    QPixmap bitmap; // bitmap in which the simulator will be drawn

    // methods
    void drawMatrix(int x, int y); // draws the matrix using x and y coordinates
    void showPreview(QString str, bool mode); // shows a preview of the starting figure base
    void start(); // starts the simulations
    void quickDraw(int x, int y, bool mode = false); // draws and mutates the matrix
    void checkRules(); // applies rules to all cells
    void updateMatrix(); // mutates the matrix based on the rules
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
