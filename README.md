Необходимо написать программу, которая принимает блоки данных по UDP и отправляет их по TCP на сервер. 
Каждый UDP пакет может содержать только один блок данных длиной от 16 до 128 байт. 
Программа должна добавить перед принятым блоком данных четыре символа, которые задаются как параметр, 
и перенаправить этот блок на сервер через TCP соединение.
TCP соединение с сервером должно быть постоянным. 
В случае обрыва соединения программа должна его автоматически переустановить. 
Если данные по UDP приходят в тот момент когда TCP соединение не установлено, они отбрасываются. 
Любые данные принимаемые от TCP сервера должны игнорироваться. 
Во время работы, программа должна вести лог важных событий, например, установка или разрыв TCP соединений, 
ошибки передачи или приёма данных и т. п. 
Состав логируемых данных можно установить самостоятельно.
Программа должна удовлетворять следующим требованиям:

1. Код должен быть написан на языке C для платформы Linux.
2. Сетевое взаимодействие должно быть реализовано через socket API (socket, bind, connect и т.д.).
3. Сборка программы должна быть реализована на основе какой-либо системы сборки (make, cmake, meson и т. п.).
4. Программа должна работать асинхронно: выполнение программы не должно блокироваться из-за процесса установки соединения или приема/передачи данных.
5. При запуске программа должна принимать следующие параметры: UDP ip:port для получения данных; TCP ip:port для отправки измененных данных;
6. путь к лог-файлу; четыре символа для добавления к передаваемым данным. Все параметры задаются из командной строки.


It is necessary to write a program that receives blocks of data via UDP and sends them via TCP to the server.
Each UDP packet can contain only one block of data ranging from 16 to 128 bytes in length.
The program must prepend the operation with a block of four characters of data, which is given as a parameter,
and redirect this block to the server via a TCP connection.
The TCP connection to the server must be maintained.
If the connection is lost, the program should automatically reinstall it.
If data arrives via UDP when the TCP connection is not established, it is discarded.
Any data received by the TCP server must be opposite.
While running, the program should log important events, such as the establishment or termination of TCP connections.
errors in data transmission or reception, etc.
You can set the composition of the logged data yourself.
The program must meet the following requirements:

1. The code must be written in C language for the Linux platform.
2. Network communication must be implemented via the socket API (socket, bind, connect, etc.).
3. The program must be built on the basis of some kind of assembly system (make, cmake, meson, etc.).
4. The program must work asynchronously: program execution should not be blocked due to the process of establishing a connection or receiving/transmitting data.
5. When starting the program, you must specify the following parameters: UDP ip:port to receive data; TCP ip:port for sending changed data;
6. path to the log file; four characters to add to service data. All parameters are set from the command line.
