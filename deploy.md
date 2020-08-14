1- bajar e instalar commons 
git clone https://github.com/sisoputnfrba/so-commons-library.git
sudo make install
2- bajar proyecto de repo
git clone https://github.com/sisoputnfrba/tp-2020-1c-Skynet.git
3- compilar utils
cd hasta utils/debug. make
4- modificar .config de proceso a levantar con las ips locales de las maquinas y datos de prueba
nano lalal.config (modificar ips y etc)
5- export library de utils
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/tp-2020-1c-Skynet/utils/Debug
6- crear carpeta de log en caso de ser necesario
7- copiar configs y archivos de pruebas
cp -i ~/tp-2020-1c-Skynet/GameBoy/gameboy.config ~/tp-2020-1c-Skynet/GameBoy/Debug