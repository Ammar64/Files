#include "mainWindow.h"
#include <QFileDialog>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QHostAddress>
#include <fstream>
Window::Window(QWidget *parent) : QMainWindow(parent), server()
{
    setServerPathes();
    setFixedSize(468, 430);
    serverWindow.setupUi(this);
    connect(serverWindow.B_StartServer, &QPushButton::clicked, this, &Window::StartServer);
    connect(serverWindow.B_RestartServer, &QPushButton::clicked, this, &Window::RestartServer);
    connect(serverWindow.B_StopServer, &QPushButton::clicked, this, &Window::StopServer);
    connect(serverWindow.AddFile, &QPushButton::clicked, this, &Window::AddFileToSend);
    connect(serverWindow.RemoveFile, &QPushButton::clicked, this, &Window::RemoveFileToSend);
    show();
}

QString getLocalIpAddress();
void Window::StartServer()
{
    serverWindow.L_RunningStatus->setStyleSheet("QLabel{ color: #ccc906; font-weight: bold}");
    serverWindow.L_RunningStatus->setText("Starting...");
    repaint();
    server.port = serverWindow.SP_Port->value();
    server.Start();

    serverWindow.L_RunningStatus->setStyleSheet("QLabel{ color: #0ce00c; font-weight: bold}");
    serverWindow.L_RunningStatus->setText("Running...");

    std::string link = "http://" + getLocalIpAddress().toStdString() + ":" + std::to_string(server.port);
    serverWindow.L_ServerInfo->setText(("<a style='color: blue;' href='" + link + "'>" + link + "</a>").c_str());
    serverWindow.L_ServerInfo->setOpenExternalLinks(true);
}

void Window::AddFileToSend()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), tr("All Files (*)"));
    if(filePath.isEmpty()) return;
    serverWindow.LW_FilesToSend->addItem(filePath);
    PathesFiles.insert(std::pair<std::string, std::string>(filePath.toStdString(), QFileInfo(filePath).fileName().toStdString()));
}

void Window::RemoveFileToSend()
{
    QListWidgetItem *currentItem = serverWindow.LW_FilesToSend->currentItem();
    if (currentItem)
    {
        std::map<std::string, std::string>::iterator it = PathesFiles.find(currentItem->text().toStdString());

        if (it != PathesFiles.end())
        {
            PathesFiles.erase(it);
        }
        serverWindow.LW_FilesToSend->takeItem(serverWindow.LW_FilesToSend->currentRow());
    }
}

void Window::RestartServer()
{
    serverWindow.L_RunningStatus->setStyleSheet("QLabel{ color: #ccc906; font-weight: bold}");
    serverWindow.L_RunningStatus->setText("Restarting...");
    repaint();
    server.port = serverWindow.SP_Port->value();
    server.Restart();
    serverWindow.L_RunningStatus->setStyleSheet("QLabel{ color: #0ce00c; font-weight: bold}");
    serverWindow.L_RunningStatus->setText("Running...");
    std::string link = "http://" + getLocalIpAddress().toStdString() + ":" + std::to_string(server.port);
    serverWindow.L_ServerInfo->setText(("<a style='color: blue;' href='" + link + "'>" + link + "</a>").c_str());
    serverWindow.L_ServerInfo->setOpenExternalLinks(true);
}

void Window::StopServer()
{
    serverWindow.L_RunningStatus->setStyleSheet("QLabel{ color: #ccc906; font-weight: bold}");
    serverWindow.L_RunningStatus->setText("Stopping...");
    repaint();
    server.Stop();

    serverWindow.L_RunningStatus->setStyleSheet("QLabel{ color: red; font-weight: bold}");
    serverWindow.L_RunningStatus->setText("Not Running...");
    serverWindow.L_ServerInfo->setText("Not Running");
    serverWindow.L_ServerInfo->setOpenExternalLinks(false);
}
void Window::setServerPathes()
{
    server.svr.Get("/", [&](const httplib::Request &req, httplib::Response &res)
                   {
        res.set_header("Content-Type", "text/html");
        res.set_content(htmlPage, "");
    });

    server.svr.Post("/upload-endpoint", [&](const httplib::Request &req, httplib::Response &res)
                    {
        if (req.has_file("file")) {
            const httplib::MultipartFormData &file = req.get_file_value("file");

            std::ofstream out(file.filename, std::ios::binary);
            out.write(file.content.c_str(), file.content.size());
            out.close();
            res.set_content(file.filename + " Uploaded successfully", "text/plain");
            serverWindow.LW_InfoFiles->addItem(file.filename.c_str());
        } });

    server.svr.Post("/available-downloads", [&](const httplib::Request &req, httplib::Response &res)
                    {
        std::string files = "";
        QListWidget *lw = serverWindow.LW_FilesToSend;
        for (int i = 0; i < lw->model()->rowCount(); i++) {
            QFileInfo fileinfo(lw->item(i)->text());
            files += fileinfo.fileName().toStdString() + (i == lw->model()->rowCount() - 1 ? "" : "\n");
        }
        res.set_content(files, "text/plain"); });

    server.svr.Get("/downloads/(.*)", [&](const httplib::Request &req, httplib::Response &res)
                   {
        std::string requested_path = req.path;

        std::string filename = requested_path.substr(strlen("/downloads/"));
        std::string fullPath;
        for( std::pair<std::string,std::string> pair : PathesFiles) {
            if(pair.second == filename){
                fullPath = pair.first;
                break;
            }
        }

        std::ifstream in(fullPath, std::ios::binary | std::ios::ate);
        size_t size = in.tellg();
        in.seekg(std::ios::beg);

        std::string content;
        content.resize(size);

        in.read(&content[0], size);
        res.set_content(content, "application/octet-stream"); });
}
QString getLocalIpAddress()
{
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address : QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
            if(address.toString().toStdString().substr(0,3) == "192" || address.toString().toStdString().substr(0,3) == "10."){
                return address.toString();
            };
    }
    return "localhost";
}