#include "ui_server.h"
#include <QMainWindow>
#include <map>
#include <string>
#include "Server.h"
class Window : public QMainWindow {
    Q_OBJECT
  public:
    explicit Window(QWidget *parent);

  private:
    Ui::MainWindow serverWindow;
    std::map<std::string, std::string> PathesFiles;
    Server server;
    void setServerPathes();

    const char *htmlPage = R"(<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Upload file</title>
    <style>
    * {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
    font-family: Arial, Helvetica, sans-serif;
}


header {
    position: fixed;
    width: 100%;
    background-color: #1964ed;
    color: rgb(250, 242, 242);
}

header h3 {
    padding: 0.8em 1em;
}


form {
    margin-top: 51.27px;
    display: inline-block;
    width: 100%;
}


#fileInput {
    display: none;
}



#fileStyle {
    width: 400px;
    display: flex;
    background-color: #f5f4f4;
    margin: 2em auto;
    cursor: pointer;
    text-align: center;
    padding: 20px;
    border-radius: 15px;
    border: 2px dashed #cdc8c8;
    align-items: center;
    flex-direction: column;
    font-size: x-large;
}

#fileStyle>#image-upload {
    width: 3.3em;
    margin-bottom: 0.5em;
}

#progressContainer {
    width: fit-content;
    display: none;
    margin: auto;
    align-items:center;
    flex-direction: column;

}

#downloadsTitle {
    font-size: 1.8em;
}

#downloadsContainer {
    padding: 2em 3em 0 3em;
    height: 100%;
    width: 100%;
    display: flex;
    flex-wrap: wrap;
    gap: 3em;
    justify-content: center;
}

.downloadLink {
    text-decoration: none;
    box-shadow: 0 0 5px black;
    padding: 1em 1em;
    font-size: larger;
    min-width: 10em;
    border-radius: 15px;
    margin-bottom: 2em;
    text-align: center;
}

    </style>
</head>

<body>
    <header>
        <h3>Files</h3>
    </header>
    <form id="uploadForm" enctype="multipart/form-data" method="post">
        <input type="file" name="FileToGet" id="fileInput">

        <label for="fileInput" id="fileStyle">
            <svg id="image-upload" viewBox="0 0 24 24" fill="none">
<path d="M17 17H17.01M15.6 14H18C18.9319 14 19.3978 14 19.7654 14.1522C20.2554 14.3552 20.6448 14.7446 20.8478 15.2346C21 15.6022 21 16.0681 21 17C21 17.9319 21 18.3978 20.8478 18.7654C20.6448 19.2554 20.2554 19.6448 19.7654 19.8478C19.3978 20 18.9319 20 18 20H6C5.06812 20 4.60218 20 4.23463 19.8478C3.74458 19.6448 3.35523 19.2554 3.15224 18.7654C3 18.3978 3 17.9319 3 17C3 16.0681 3 15.6022 3.15224 15.2346C3.35523 14.7446 3.74458 14.3552 4.23463 14.1522C4.60218 14 5.06812 14 6 14H8.4M12 15V4M12 4L15 7M12 4L9 7" stroke="#000000" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
</svg>
            upload
        </label>


        <div id="progressContainer">
            <span><progress id="progressBar" value="0" max="100"></progress>
                <span id="percentage"></span></span>
            <span id="loadedToTotals"></span>
            <span id="UploadState"></span>
        </div>
        <br>

    </form>
    <h3 id="downloadsTitle">Downloads</h3>
    <hr>
    <div id="downloadsContainer">
    </div>
    <script>
const form = document.getElementById("uploadForm");
const progressBar = document.getElementById("progressBar");
const percentageText = document.getElementById("percentage");
const loadedTOTotal = document.getElementById("loadedToTotals");
const fileInput = document.getElementById("fileInput");
const uploadState = document.getElementById("UploadState");
const progressContainer = document.getElementById("progressContainer");
let  downloadsContainer = document.getElementById("downloadsContainer");


getAvailableDownloads();

fileInput.addEventListener("change" , (e) => {
    e.preventDefault();
    progressContainer.style.display = 'flex';
    var formData = new FormData();
    formData.append("file" , fileInput.files[0] );
    console.log(fileInput.files[0]);
    var xhr = new XMLHttpRequest();

    xhr.upload.addEventListener("progress" , (p) => {
        if(p.lengthComputable) {
            let percentage = p.loaded / p.total * 100;
            progressBar.value = percentage;
            percentageText.textContent = `${percentage.toFixed(2)}%`;
            loadedTOTotal.textContent = `${getFormattedFileSize(p.loaded)} / ${getFormattedFileSize(p.total)}`;
        }
    },false);
    xhr.onreadystatechange = () => {
        if(xhr.readyState === 4)
        {
            if (xhr.status === 200) {
                uploadState.innerHTML = `${xhr.responseText}<br>${uploadState.textContent}`;
            } else {
                uploadState.innerHTML = `${xhr.responseText}<br>${uploadState.textContent}`;
                console.error("Upload failed");
            }
        }
    
    }

    xhr.open("POST", "/upload-endpoint" , true);
    xhr.send(formData);
} )



function getFormattedFileSize(s) {
    const levels = ["B","KB","MB","GB","TB","PB"];
    let level = 0;
    let isGood = false;
    while(!isGood) {
        if( s > 1200 && level < levels.length ) {
            s /= 1024;
            level++;
        }
        else {
            isGood = true;
        }
    }
    return `${s.toFixed(2)} ${levels[level]}`;
    
}

setInterval(getAvailableDownloads, 1000);
function getAvailableDownloads() {
    let xhr = new XMLHttpRequest();

    xhr.onload = () => {

        const newDownloads = document.createElement("div");
        newDownloads.id = "downloadsContainer";

        if(xhr.responseText !== ""){        
            let files = xhr.responseText.split('\n');
            files.forEach( e => {
                const anchor = document.createElement("a");
                anchor.className = 'downloadLink'
                anchor.textContent = e;
                anchor.href = "downloads/" + e;
                anchor.toggleAttribute("download", true);
                newDownloads.appendChild(anchor);
            })
        }
        downloadsContainer.remove()
        downloadsContainer = newDownloads;
        document.querySelector("hr").insertAdjacentElement('afterend', downloadsContainer);
    }

    xhr.open("POST", "/available-downloads", true);
    xhr.send();
}
</script>
</body>

</html>)";
  private slots:
    void StartServer();
    void RestartServer();
    void StopServer();

    void AddFileToSend();
    void RemoveFileToSend();
};