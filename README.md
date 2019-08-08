# ld.so.preload

This project represents the creation of a Linux library that prevents race condition, specifically TOCTTOU, attacks. 

Please read this document in order to execute the Proof Of Concept (POC).

Index:
* Caveats
* Context
* Configuration
* Docs
* POC
* Logging
* License

## Caveats

Please bear in mind the present library is still under development and has been tested only on certain environments. It's strongly recommended to **NOT USE IT** in production environments, or your main operating system. Keep in mind the library persintently checks for existence of certain environment variables and files, **and it has system-wide range**. This is, it could affect normal behaviour of your system, e.g. the boot stage of the OS before $HOME variable is defined. 

## Context

LD_PRELOAD allows dynamically linked function hooking.

You might consider LD_PRELOAD a security hole but it is actually secured since it won't work if ruid != euid. 

Shared libraries specified in **/etc/
ld.so.preload** are loaded **before** those from LD_PRELOAD.

Statistically linked libraries are immune to LD_PRELOAD as well as functions defined within the very source code.

### Here you can find very useful LD_PRELOAD information:

* http://www.goldsborough.me/c/low-level/kernel/2016/08/29/16-48-53-the_-ld_preload-_trick/
* https://rafalcieslak.wordpress.com/2013/04/02/dynamic-linker-tricks-using-ld_preload-to-cheat-inject-features-and-investigate-programs/
* https://blog.fpmurphy.com/2012/09/all-about-ld_preload.html
* https://blog.jessfraz.com/post/ld_preload/
* https://stackoverflow.com/questions/426230/what-is-the-ld-preload-trick

## Configuration

There is very little configuration this library need in order to get executed by the [dynamic loader](http://man7.org/linux/man-pages/man8/ld.so.8.html). 

In the main directory, there is ``` configure.sh ```. We recommend you to run it since it will create a folder in your $HOME for the logs to be stored into and it will mount the folder in a TMPFS so there is almost no performance punishment. This implies two things:
- You must be able to run ```configure.sh``` with superuser privileges. 
- You must have $HOME environment variable pointing to some available directory.

**Please bear in mind** that TMPFS is a volatile file system which offer very low latencies but is erased everytime you restart or shut down the computer. This is, *your logs will be deleted* unless you backup them.

## POC

In order to execute the TOCTTOU POC simply execute the following commands.

There are two POC files. 

* [vulnerableAccessFopen.c](/vulnerableAccessFopen.c) - Vulnerable <access, fopen> sequence.
* [vulnerableLstatOpen.c](/vulnerableLstatOpen.c) - Vulnerable <lstat, open> sequence.

Both files represent TOCTTOU-vulnerable code. The way of exploiting TOCTTOU consists of an attacker with lesser privileges than the actual program creating a file that he/she has access to in first place and then, at the exact moment (called the vulnerability window), replacing that file with a symbolic link towards some critic file he/she has no access to. Since the program will be running with escalated privileges, generally steuid to root, the user will be able to actually read/write in the protected file. 

In order to test TOCTTOU exploitation simply follow these steps: (**Assuming you are working from the same directory where you cloned this repo and from where this README is**)

1. We need some protected file, so let's create one with root user.

```
echo "A root file" | sudo tee r00t.file
```

2. Now let's compile the vulnerable code you want to test. We'll call the binary *vulnerable*. So you execute either of these commands:

```
sudo gcc vulnerableAccessFopen.c -o vulnerable
sudo gcc vulnerableLstatOpen.c -o vulnerable
``` 

3. Now we need to activate setuid bit.

```
sudo chmod u+s vulnerable
```

4. You should now have the following files (amongst others):

```
-rwxr-xr-x 1 user user ... ... .. ..:.. exploit.sh
-rw-r--r-- 1 root root ... ... .. ..:.. r00t.file 
-rwsr-xr-x 1 root root ... ... .. ..:.. vulnerable
```

5. Now, in order to exploit the vulnerable code, simply execute the script:

```
./exploit.sh r00t.file 2> /dev/null
```
*If you want to see every output, edit the script and dont redirect its output. Then simply execute ./exploit.sh r00t.file*

6. Now simply wait for the script to finish. Sooner or later r00t.file will be edited via a *user* created file that's been replaced by a symbolic link at the right moment. Since TOCTTOU vulnerability is a non-deterministic one, we cannot know beforehand how long it will take to actually exploit the vulnerability. There are many factors to take into account, such as CPU performance, OS, enviroment variables, OS preempting, context switches, etc...

## Logging

Logging is carried out by Zlog, a C in-memory logging library. It allocates a buffer in memory for logging, and flush the buffer to the log file (or stdout) when the buffer is full, or it is instructed explicitly by the program or flushing thread. 

Zlog is originally written by Zhiqiang Ma and it was modified to fulfill the project needs. You can find the modified library here (https://github.com/RazviOverflow/zlog) and the original here (https://github.com/zma/zlog). 

## Docs

All the code is documented. You can either read the comments directly into the .h (headers) file or use the HTML generated docs. In order to use the latter, simply open [index.html](https://github.com/RazviOverflow/ld.so.preload/blob/master/docs/index.html) file located in docs folder.

## LICENSE

Copyright 2019 Razvan Raducu and Ricardo J. Rodriguez

This file is part of libTTThwart.so.

libTTThwart.so is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libTTThwart.so is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libTTThwart.so.  If not, see <https://www.gnu.org/licenses/>.