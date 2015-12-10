# HttpFileServer

Simple http file server.</br>
1.Support upload and download file.</br>

# Usage
1.upload cgi</br>
http://127.0.0.1:9875/upload.cgi</br> 
Example: https://github.com/KingsleyYau/HttpFileServer/tree/master/upload_file.html</br>
Respone: json</br>
{"ret":1:成功,0:失败,"data":{"filepath":"文件相对路径"}}</br>

2.donwload cgi</br>
http://127.0.0.1:9875/download.cgi?filepath=相对路径</br>
Respone: file stream</br>

# Compile and run
1 chmod +x configure</br>
2 ./configure</br>
3 make clean && make all</br>
4 httpfileserver -f httpfileserver.config</br>
