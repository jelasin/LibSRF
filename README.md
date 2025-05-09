# LibSRF

C语言共享内存环形队列, 环形队列采用[LibCSTL](https://github.com/jelasin/LibCSTL)中的通用环形队列模型实现.

## Usage

直接复制头文件和源文件到自己的项目中即可.

## API

* `int shmfifo_init(shmfifo_t *shm, size_t blknum, size_t blksz)`:创建shmfifo, 传入共享内存块数量和大小.
* `void shmfifo_destroy(shmfifo_t *shm);`:销毁shmfifo.
* `int shmfifo_write(shmfifo_t *shm, void *element, size_t size);`:写入操作,直接操作内存块,超过单块内存会写入失败.
* `int shmfifo_read(shmfifo_t *shm, void *element, size_t size);`:读取操作,直接操作内存块.

## Develop

* [ ] 支持自定义写入和读取函数, 方便用户自定义操作.
