# CConnectFour
Multiplayer connect four in C <br>
Player one will be **Host & Client** and player two will just be a **Client**

## Dependencies
> - GCC C17
> - SDL2
> - SDL2 Image 
> - SDL2 TTF 
> - Make

## Build
Clone project and run
```shell
make
```

## Run
To start the server run
```shell
./cconnectfour <port> <ip>
```
To join existing game run
```shell
./cconnectfour <port>
```

To clean up the build run
```shell
make clean
```

## License
[GLP-3.0](https://choosealicense.com/licenses/gpl-3.0/)