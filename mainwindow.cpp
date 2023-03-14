#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // creation of the bitmap with a 5px margin
    bitmap = QPixmap(810,810);
    // fills bitmap with white color
    bitmap.fill(Qt::white);
    ui->setupUi(this);
    // shows the preview of the currently selected stard
    showPreview(ui->box_start->currentText(), false);
    // shows the bitmap on the grid
    ui->grid->setPixmap(bitmap);
    // creates the thread that calculates the iterations
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
    // sum 1 to the iteration counter
    iter++;
    // sets both labels to current counters
    ui->l_iter->setText(QString::number(iter));
    ui->l_active_cells->setText(QString::number(n_cells));
}

void MainWindow::drawMatrix(int x, int y) // displays the updated matrix inside the console
{
    //qDebug() << "paint!!!!";
    QPainter painter(&bitmap);
    painter.setPen(QPen(Qt::black,2));
    painter.drawPoint(x * + 2,y * 2  + 2);
}

void MainWindow::showPreview(QString str, bool mode) // shows a preview of the selected starting figure
{
    // fills bitmap with white color
    bitmap.fill(Qt::white);
    // checks the current displayed text of the combo box and draws selected figure
    if (str == "Blinker") {
        quickDraw(rel_height, rel_height, mode);
        quickDraw(rel_height, rel_height - 1, mode);
        quickDraw(rel_height, rel_height + 1, mode);
    } else if (str == "R-pentomino") {
        quickDraw(rel_height, rel_height, mode);
        quickDraw(rel_height - 1, rel_height, mode);
        quickDraw(rel_height, rel_height - 1, mode);
        quickDraw(rel_height, rel_height + 1, mode);
        quickDraw(rel_height + 1, rel_height - 1, mode);
    } else if (str == "Diehard") {
        quickDraw(rel_height - 2, rel_height, mode);
        quickDraw(rel_height - 3, rel_height, mode);
        quickDraw(rel_height - 2, rel_height + 1, mode);
        quickDraw(rel_height + 2, rel_height + 1, mode);
        quickDraw(rel_height + 3, rel_height + 1, mode);
        quickDraw(rel_height + 4, rel_height + 1, mode);
        quickDraw(rel_height + 3, rel_height - 1, mode);
    } else if (str == "Acorn") {
        quickDraw(rel_height + 1, rel_height, mode);
        quickDraw(rel_height - 1, rel_height + 1, mode);
        quickDraw(rel_height - 2, rel_height + 1, mode);
        quickDraw(rel_height + 2, rel_height + 1, mode);
        quickDraw(rel_height + 3, rel_height + 1, mode);
        quickDraw(rel_height + 4, rel_height + 1, mode);
        quickDraw(rel_height - 1, rel_height - 1, mode);
    }
    // updates the pixmap
    ui->grid->setPixmap(bitmap);
}

void MainWindow::start() // selects a random figure to start
{
    showPreview(ui->box_start->currentText(), true);
    ui->box_start->setEnabled(false);
}

void MainWindow::quickDraw(int x, int y, bool mode)
{
    if (mode) {
        matrix[HEIGHT - y][x] = 1;
    }
    drawMatrix(x, y);
}

void MainWindow::checkRules() // checks all cells to apply the rules
{
    n_cells = 0;
    // iterates through the y coordinate
    for (int i = 0; i < WIDTH; i++) {
        // iterates through the x coordinate
        for (int j = 0; j < HEIGHT; j++) {
            int n_num = 0;
            if (matrix[j][i] == 1) {
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
            }
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
    if (iter != 0) {
        ui->btn_reset->setEnabled(true);
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
        ui->btn_reset->setEnabled(false);
        ui->btn_pause->setText("Pause");
        if (!started) {
            start();
            started = true;
        }
        thread->start();
    } else {
        ui->btn_pause->setText("Start");
        paused = true;
    }
}


void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    switch (value) {
    case 1:
        speed = 0.1;
        break;
    case 2:
        speed = 0.05;
        break;
    case 3:
        speed = 0.025;
        break;
    }
    microsecond = 1000000 * speed;
    }



void MainWindow::on_box_start_currentTextChanged(const QString &arg1)
{
    showPreview(arg1, false);
}



void MainWindow::on_btn_reset_clicked()
{
    iter = 0;
    n_cells = 0;
    started = false;
    ui->l_iter->setText(QString::number(iter));
    memset(matrix, 0, sizeof(matrix));
    ui->box_start->setEnabled(true);
    ui->l_active_cells->setText(QString::number(n_cells));
    showPreview(ui->box_start->currentText(), false);
    ui->btn_reset->setEnabled(false);
}

