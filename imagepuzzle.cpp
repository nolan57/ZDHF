#include "imagepuzzle.h"
#include "tools.H"
#include "createalpha.h"
#include "pieceofimage.h"
#include <QRandomGenerator>
#include <cmath>

ImagePuzzle::ImagePuzzle(QWidget *parent) : QMainWindow(parent) {
    // 主窗口布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 工具栏
    QWidget *toolbar = new QWidget(this);
    toolbar->setStyleSheet(
        "QWidget {"
        "   border-bottom: 1px solid palette(dark);"
        "   background: palette(window);"
        "}"
    );
    QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setSpacing(15);
    toolbarLayout->setContentsMargins(15, 8, 15, 8);

    // 打开图片按钮
    QPushButton *openButton = new QPushButton("打开图片", this);
    openButton->setFixedSize(100, 35);
    openButton->setStyleSheet(
        "QPushButton {"
        "   padding: 5px;"
        "   background: palette(button);"
        "   border: 1px solid palette(dark);"
        "   border-radius: 3px;"
        "   color: palette(buttontext);"
        "}"
        "QPushButton:hover {"
        "   background: palette(light);"
        "}"
        "QPushButton:pressed {"
        "   background: palette(dark);"
        "}"
    );
    connect(openButton, &QPushButton::clicked, this, &ImagePuzzle::openImage);
    toolbarLayout->addWidget(openButton);

    // 分割数标签
    QLabel *nLabel = new QLabel("分割数:", this);
    nLabel->setFixedHeight(35);
    nLabel->setStyleSheet(
        "QLabel {"
        "   padding: 5px;"
        "   color: palette(text);"
        "}"
    );
    toolbarLayout->addWidget(nLabel);

    // 分割数输入框
    nInput = new QLineEdit(this);
    nInput->setFixedSize(60, 35);
    nInput->setAlignment(Qt::AlignCenter);
    nInput->setStyleSheet(
        "QLineEdit {"
        "   padding: 5px;"
        "   background: palette(base);"
        "   border: 1px solid palette(dark);"
        "   border-radius: 3px;"
        "   color: palette(text);"
        "}"
        "QLineEdit:focus {"
        "   border: 1px solid palette(highlight);"
        "}"
    );
    nInput->setValidator(new QIntValidator(2, 10000, this));
    toolbarLayout->addWidget(nInput);

    // 开始按钮
    QPushButton *startButton = new QPushButton("开始", this);
    startButton->setFixedSize(100, 35);
    startButton->setStyleSheet(
        "QPushButton {"
        "   padding: 5px;"
        "   background: palette(button);"
        "   border: 1px solid palette(dark);"
        "   border-radius: 3px;"
        "   color: palette(buttontext);"
        "}"
        "QPushButton:hover {"
        "   background: palette(light);"
        "}"
        "QPushButton:pressed {"
        "   background: palette(dark);"
        "}"
    );
    connect(startButton, &QPushButton::clicked, this, &ImagePuzzle::startPuzzle);
    toolbarLayout->addWidget(startButton);

    // 添加伸缩空间
    toolbarLayout->addStretch();

    // 内容区域
    QWidget *contentWidget = new QWidget(this);
    contentWidget->setStyleSheet("background: palette(window);");
    QHBoxLayout *contentLayout = new QHBoxLayout(contentWidget);

    // 左侧区域：显示分割后的图片
    leftArea = new DropWidget(this);
    leftArea->setStyleSheet("background: palette(window);");
    leftLayout = new QGridLayout(leftArea);
    contentLayout->addWidget(leftArea, 1);

    // 右侧区域：用于重组图片
    rightArea = new DropLabel(this);
    rightArea->setAlignment(Qt::AlignCenter);
    rightArea->setStyleSheet(
        "QLabel {"
        "   background: palette(base);"
        "   border: 2px dashed palette(dark);"
        "}"
    );
    contentLayout->addWidget(rightArea, 1);

    // 设置主布局
    mainLayout->addWidget(toolbar);
    mainLayout->addWidget(contentWidget, 1);  // 内容区域可拉伸
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    setCentralWidget(centralWidget);
    setWindowTitle("图片拼图游戏");
    resize(1000, 700);  // 增大窗口尺寸
}

void ImagePuzzle::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-puzzle-piece")) {
        event->acceptProposedAction();
    }
}

void ImagePuzzle::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("application/x-puzzle-piece")) {
        QByteArray pieceData = event->mimeData()->data("application/x-puzzle-piece");
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        QPixmap pixmap;
        QPoint position;
        dataStream >> pixmap >> position;

        // 获取放置位置
        QPoint dropPos = event->position().toPoint();
        
        // 创建新的标签显示拼图块
        QLabel *pieceLabel = new QLabel(rightArea);
        pieceLabel->setPixmap(pixmap);
        pieceLabel->setStyleSheet("background: transparent; border: none;");
        
        // 保存原始位置信息
        pieceLabel->setProperty("originalCol", position.x());
        pieceLabel->setProperty("originalRow", position.y());

        pieceLabel->move(dropPos - QPoint(pixmap.width()/2, pixmap.height()/2));
        pieceLabel->setAttribute(Qt::WA_DeleteOnClose);
        pieceLabel->installEventFilter(this);
        pieceLabel->show();

        event->acceptProposedAction();

        // 检查是否完成拼图
        checkCompletion();
    }
}

void ImagePuzzle::openImage() {
    QString fileName = QFileDialog::getOpenFileName(this,
        "打开图片", "", "图片文件 (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        originalImage = cv::imread(fileName.toStdString(), cv::IMREAD_UNCHANGED);
        if (originalImage.empty()) {
            QMessageBox::warning(this, "错误", "无法打开图片文件");
        }
    }
}

void ImagePuzzle::startPuzzle() {
    if (originalImage.empty()) {
        QMessageBox::warning(this, "错误", "请先选择图片");
        return;
    }

    // 清除之前的拼图
    clearPuzzle();

    // 获取切割数量
    int n = nInput->text().toInt();
    if (n <= 2) {
        QMessageBox::warning(this, "错误", "请输入有效的切割数量");
        return;
    }

    // 计算分割后每块图片的大小
    double pieceWidth = (double)originalImage.cols / n;
    double pieceHeight = (double)originalImage.rows / n;
    
    // 检查分割后的图片是否太小
    if (pieceWidth < 10 || pieceHeight < 10) {
        QMessageBox::warning(this, "错误", 
            QString("分割数过大！分割后图片尺寸将小于10像素\n当前图片尺寸: %1x%2\n最大允许分割数: %3")
            .arg(originalImage.cols)
            .arg(originalImage.rows)
            .arg(std::min(originalImage.cols / 10, originalImage.rows / 10)));
        return;
    }

    // 调整右侧区域大小，保持与原图相同的宽高比
    double aspectRatio = (double)originalImage.cols / originalImage.rows;
    int rightAreaHeight = rightArea->height();
    int rightAreaWidth = rightAreaHeight * aspectRatio;
    
    // 如果计算出的宽度超过了可用空间，则按宽度计算高度
    if (rightAreaWidth > rightArea->width()) {
        rightAreaWidth = rightArea->width();
        rightAreaHeight = rightAreaWidth / aspectRatio;
    }
    
    // 设置右侧区域的固定大小
    rightArea->setFixedSize(rightAreaWidth, rightAreaHeight);

    // 保存网格大小，供其他函数使用
    gridCellWidth = rightAreaWidth / n;
    gridCellHeight = rightAreaHeight / n;

    // 创建工具对象并切割图片
    Tools tools;
    pieces = tools.toPartition(originalImage, n, n);
    for(size_t i = 0; i < pieces.size(); i++) {
        tools.toShapeImage(pieces[i]);
    }
    
    // 显示拼图块
    showPieces();
    
    // 重置正确放置的计数
    correctPieces = 0;
}

void ImagePuzzle::clearPuzzle() {
    if (leftLayout) {
        QLayoutItem *child;
        while ((child = leftLayout->takeAt(0)) != nullptr) {
            if (child->widget()) {
                delete child->widget();
            }
            delete child;
        }
    }
    rightArea->clear();
    rightArea->setPixmap(QPixmap());
}

void ImagePuzzle::showPieces() {
    // 清除现有的拼图块
    QLayoutItem *child;
    while ((child = leftLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // 随机打乱拼图块的顺序
    auto rng = QRandomGenerator::global();
    for (int i = pieces.size() - 1; i > 0; --i) {
        int j = rng->bounded(i + 1);
        std::swap(pieces[i], pieces[j]);
    }

    // 计算每个piece的尺寸
    int pieceWidth = leftArea->width() / 3;
    int pieceHeight = leftArea->height() / 3;

    // 随机位置参数
    int padding = 20;
    int minX = padding;
    int maxX = leftArea->width() - pieceWidth - padding;
    int minY = padding;
    int maxY = leftArea->height() - pieceHeight - padding;

    for (const auto& piece : pieces) {
        QLabel *pieceLabel = new QLabel(leftArea);
        
        // 将OpenCV Mat转换为QImage
        cv::Mat rgbImage;
        cv::cvtColor(piece.shapedImage, rgbImage, cv::COLOR_BGRA2RGBA);
        QImage qImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step, QImage::Format_RGBA8888);
        
        // 创建QPixmap并缩放
        QPixmap pixmap = QPixmap::fromImage(qImage);
        pixmap = pixmap.scaled(pieceWidth, pieceHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
        // 设置随机位置
        int x = QRandomGenerator::global()->bounded(minX, maxX);
        int y = QRandomGenerator::global()->bounded(minY, maxY);
        
        pieceLabel->setPixmap(pixmap);
        pieceLabel->setGeometry(x, y, pieceWidth, pieceHeight);
        pieceLabel->setStyleSheet("background: transparent; border: none;");
        
        // 保存原始位置信息
        pieceLabel->setProperty("originalCol", piece.location.col);
        pieceLabel->setProperty("originalRow", piece.location.row);

        // 确保图片完整显示
        pieceLabel->setScaledContents(true);

        // 安装事件过滤器以处理拖放
        pieceLabel->setAttribute(Qt::WA_DeleteOnClose);
        pieceLabel->installEventFilter(this);
        pieceLabel->show();
    }
}

bool ImagePuzzle::checkPlacement(const QPoint &actual) {
    // 确保拖放位置在rightArea内
    QPoint localPos = rightArea->mapFrom(this, actual);
    if (!rightArea->rect().contains(localPos)) {
        return false;
    }

    return true;
}

bool ImagePuzzle::checkCompletion() {
    // 获取所有拼图块
    QList<QLabel*> pieces = rightArea->findChildren<QLabel*>();
    if (pieces.isEmpty()) return false;

    bool completed = true;
    
    // 允许的误差范围（像素）
    const int tolerance = 10;

    for (QLabel* piece : pieces) {
        // 获取当前位置
        QPoint currentPos = piece->pos();
        
        // 获取原始位置
        int originalCol = piece->property("originalCol").toInt();
        int originalRow = piece->property("originalRow").toInt();
        
        // 计算期望位置
        QPoint expectedPos(originalCol * gridCellWidth, originalRow * gridCellHeight);
        
        // 检查位置是否在允许的误差范围内
        if (abs(currentPos.x() - expectedPos.x()) > tolerance || 
            abs(currentPos.y() - expectedPos.y()) > tolerance) {
            completed = false;
            break;
        }
    }

    if (completed) {
        QMessageBox::information(this, "恭喜", "拼图完成！");
    }
    return completed;
}

bool ImagePuzzle::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QLabel *child = static_cast<QLabel*>(obj);
        if (!child)
            return false;

        QPixmap pixmap;
        // 如果是从右侧区域拖动，使用保存的原始大小图片
        if (child->parent() == rightArea && child->property("originalPixmap").isValid()) {
            pixmap = child->property("originalPixmap").value<QPixmap>();
        } else {
            pixmap = child->pixmap(Qt::ReturnByValue);
        }

        // 获取原始的行列位置
        QPoint originalPos(child->property("originalCol").toInt(),
                         child->property("originalRow").toInt());

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << pixmap << originalPos;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-puzzle-piece", itemData);
        // 存储原始QLabel的指针，以便在左侧区域放下时更新其位置
        mimeData->setProperty("sourceLabel", QVariant::fromValue((qintptr)child));

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        drag->setHotSpot(mouseEvent->pos());

        // 在开始拖动时立即隐藏原始图片
        child->hide();
        
        Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
        if (dropAction == Qt::MoveAction) {
            // 拖放成功，删除原图片
            child->close();
        } else {
            // 如果拖动取消，则重新显示原始图片
            child->show();
        }
        
        return true;
    } else if (event->type() == QEvent::DragEnter) {
        // 允许在左侧区域内拖放
        QDragEnterEvent *dragEvent = static_cast<QDragEnterEvent*>(event);
        if (dragEvent->mimeData()->hasFormat("application/x-puzzle-piece")) {
            dragEvent->acceptProposedAction();
        }
    }
    return QWidget::eventFilter(obj, event);
}
