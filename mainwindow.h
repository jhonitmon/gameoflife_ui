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

private:
    static const int HEIGHT = 200;
    static const int WIDTH = 200;
    int matrix[HEIGHT][WIDTH] = {}; // matrix containing the values
    int iter = 0; // current iteration
    int n_cells = 0;
    double speed = 0.05;
    bool paused = true;
    bool started = false;
    unsigned int microsecond = 1000000 * speed; // microsecond multiplier
    // vectors containing the coordinates of the cells to be created/deleted
    std::vector<int> a_coord_x;
    std::vector<int> a_coord_y;
    std::vector<int> d_coord_x;
    std::vector<int> d_coord_y;
    QThread *thread;
    QPixmap bitmap;
    // methods
    void drawMatrix(int x, int y);
    void start();
    void quickDraw(int x, int y);
    void checkRules();
    void updateMatrix();
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
