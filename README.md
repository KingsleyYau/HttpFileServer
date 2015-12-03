# HttpFileServer

简单http文件服务器.</br>
1.支持上传下载功能.</br>

# 用法
1.上传cgi</br>
http://127.0.0.1:9875/upload.cgi</br> 
参考例子:https://github.com/KingsleyYau/HttpFileServer/tree/master/upload_file.html</br>
返回json</br>
{"ret":1:成功,0:失败,"data":{"filepath":"文件相对路径"}}</br>

2.下载cgi</br>
http://127.0.0.1:9875/download.cgi?filepath=相对路径</br>
返回文件流</br>

# 编译运行
1 chmod +x configure</br>
2 ./configure</br>
3 make clean && make all</br>
4 httpfileserver -f httpfileserver.config</br>
