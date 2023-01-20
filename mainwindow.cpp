#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    bitmap = QPixmap(810,810);
    bitmap.fill(Qt::white);
    ui->setupUi(this);
    ui->grid->setPixmap(bitmap);
    thread = QThread::create([this]{
        while (1) {

            iterate();
            if (paused) {
                break;
            }
        }
    });
    paused = true;


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::iterate() //starts an iteration
{

    // sets pause between iterations
    usleep(microsecond);
    // iteration starts here
    checkRules();
    updateMatrix();
    iter++;
    //qDebug() << iter << " " << n_cells;
    ui->l_iter->setText(QString::number(iter));
    ui->l_active_cells->setText(QString::number(n_cells));

}

void MainWindow::drawMatrix(int x, int y) // displays the updated matrix inside the console
{
    //qDebug() << "paint!!!!";
    QPainter painter(&bitmap);
    painter.setPen(QPen(Qt::black,5));
    painter.drawPoint(x * 4 + 5,y * 4 + 5);
}

void MainWindow::start() // selects a random figure to start
{
    //qDebug() << ui->box_start->currentText();
    int rel_height = HEIGHT / 2 - 1;

    if (ui->box_start->currentText() == "Blinker") {
        quickDraw(rel_height, rel_height);
        quickDraw(rel_height, rel_height - 1);
        quickDraw(rel_height, rel_height + 1);
    } else if (ui->box_start->currentText() == "R-pentomino") {
        quickDraw(rel_height, rel_height);
        quickDraw(rel_height - 1, rel_height);
        quickDraw(rel_height, rel_height - 1);
        quickDraw(rel_height, rel_height + 1);
        quickDraw(rel_height + 1, rel_height - 1);
    } else if (ui->box_start->currentText() == "Diehard") {
        quickDraw(rel_height - 2, rel_height);
        quickDraw(rel_height - 3, rel_height);
        quickDraw(rel_height - 2, rel_height + 1);
        quickDraw(rel_height + 2, rel_height + 1);
        quickDraw(rel_height + 3, rel_height + 1);
        quickDraw(rel_height + 4, rel_height + 1);
        quickDraw(rel_height + 3, rel_height - 1);
    } else if (ui->box_start->currentText() == "Acorn") {
        quickDraw(rel_height + 1, rel_height);
        quickDraw(rel_height - 1, rel_height + 1);
        quickDraw(rel_height - 2, rel_height + 1);
        quickDraw(rel_height + 2, rel_height + 1);
        quickDraw(rel_height + 3, rel_height + 1);
        quickDraw(rel_height + 4, rel_height + 1);
        quickDraw(rel_height - 1, rel_height - 1);
    }
    ui->grid->setPixmap(bitmap);
    ui->box_start->setEnabled(false);
}

void MainWindow::quickDraw(int x, int y)
{
    matrix[HEIGHT - y][x] = 1;
    drawMatrix(x, y);
}

void MainWindow::checkRules() // checks all cells to apply the rules
{
    n_cells = 0;
    // iterates through the y coordinate
    for (int i = 0; i < WIDTH; i++) {
        // iterates through the x coordinate
        for (int j = 0; j < HEIGHT; j++) {
            int n_num = 0; // number of neighbours
            //qDebug() << "i " << i << " j " << j;
            if (matrix[j][i] == 1) {
                //qDebug() << "Test!";
                n_cells++;
            }
            // sums or rests from the number of neighbours depending on the boundary conditions
            if (i == 0 || i == WIDTH || j == 0 || j == HEIGHT) {
                n_num--;
            }
            if (matrix[i - 1][j] == 1) {
                n_num++;
            }
            if (matrix[i - 1][j - 1] == 1) {
                n_num++;
            }
            if (matrix[i - 1][j + 1] == 1) {
                n_num++;
            }
            if (matrix[i + 1][j] == 1) {
                n_num++;
            }
            if (matrix[i + 1][j - 1] == 1) {
                n_num++;
            }
            if (matrix[i + 1][j + 1] == 1) {
                n_num++;
            }
            if (matrix[i][j - 1] == 1) {
                n_num++;
            }
            if (matrix[i][j + 1] == 1) {
                n_num++;
            }
            // adds the coordinates of the cells to be deleted to the "d_coord_" vectors
            if (matrix[i][j] == 1 && (n_num < 2 || n_num > 3)) {
                d_coord_x.push_back(j);
                d_coord_y.push_back(i);
            }
            // adds the coordinates of the cells to be created to the "a_coord_" vectors
            else if ((matrix[i][j] == 0 && n_num == 3) || (matrix[i][j] == 1 && n_num == 2) || (matrix[i][j] == 1 && n_num == 3)) {
                a_coord_x.push_back(j);
                a_coord_y.push_back(i);
                //qDebug() << "Probe x " << j << " y " << i;
            }
            //qDebug() << "n_num "<<n_num;
        }
    }
}

void MainWindow::updateMatrix() // updates the matrix with the cells to be created/deleted
{
    bitmap.fill(Qt::white);
    // checks the coordinate vectors to create/delete cells
    for (unsigned long i = 0; i < d_coord_x.size(); i++) {
        matrix[d_coord_y[i]][d_coord_x[i]] = 0;
    }
    for (unsigned long i = 0; i < a_coord_x.size(); i++) {
        //qDebug() << "testeito " << a_coord_x[i] << " " << a_coord_y[i];
        matrix[a_coord_y[i]][a_coord_x[i]] = 1;
        drawMatrix(a_coord_x[i],a_coord_y[i]);
    }
    ui->grid->setPixmap(bitmap);
    // resets coordinate vectors
    a_coord_x = {};
    a_coord_y = {};
    d_coord_x = {};
    d_coord_y = {};
}

void MainWindow::on_btn_pause_clicked()
{
    //qDebug() << "paused " << paused;

    if (iter != 0) {
        thread = QThread::create([this]{
            while (1) {

                iterate();
                if (paused) {
                    break;
                }
            }
        });
    }
    if (paused) {
        paused = false;
        ui->btn_pause->setText("Pause");
        //qDebug() << "execute";
        if (!started) {
            //qDebug() << "execute3";
            start();
            started = true;
        }
        thread->start();
    } else {
       // qDebug() << "execute2";
        ui->btn_pause->setText("Start");
        paused = true;
    }
}


void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    speed = 0.05 * value;
    microsecond = 1000000 * speed;
}

